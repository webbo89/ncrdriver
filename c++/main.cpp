#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <stdio.h>
#include <boost/thread.hpp>
#include "BufferedAsyncSerial.h"
#include "../libqrencode/qrencode.h"


using namespace std;
using namespace boost;

// Shortcode function
string getShortcode (int date)
{
    /*string base = "abcdefghjkmnpqrstwxz23456789";
    int length = base.length();
    string out1 = "";

    while(date > length - 1)
    {
        out1 = base[fmod(date, length)] += out1;
        date = floor( date / length );
    }

    string shorturl = base[date] += out1;
        
    return shorturl;*/
}


int main(int argc, char* argv[])
{
        string USBPort = "/dev/ttyUSB0";
        bool cut = true;
        int precutNewlines = 3; 
        string message = "";
        string line = "";
        int time;
        string shortURL;
        
        int width;
        unsigned char * data;
        stringstream ss;
        QRcode *qrcode;
        
        
        // Set USB port for serial communication - default to /dev/ttyUSB0
        if(argc > 0){
           // USBPort = "/dev/ttyUSB" += argv[1];
        } else {
            USBPort = "/dev/ttyUSB0";
        }
        
        // Set time of interaction (epoch expected)
        if(argc > 1){
            time = atoi(argv[4]);
        }
        
        // Set file path to message - default to message.txt in current directory
        if(argc > 2){
           //ifstream messageFile(argv[2]);
        } else {
          // ifstream messageFile("message.txt");
        }
        
        // Set cut value, default to true
        if(argc > 3){
            if(argv[2] == "false" | argv[2] == "no"){
                cut = false;
            }
        }
        
        // Set new line count before cut - default to 3
        if(argc > 4){
            precutNewlines = atoi(argv[3]);
        }
        
        
        //read text file into message
        //while(messageFile.good()){
         //   getline(messageFile,line);
          //  message += line += "\n";
        //}
        
        //make shorturl
        shortURL = "http://icrobot.net/eqrfzeqrfz";// += getShortcode(time);
        
        //create qr
        qrcode = QRcode_encodeString(shortURL.c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 0);
        
        //create bitmap
        data = qrcode->data;
        width = qrcode->width;
        
        ofstream geoff("bmpting.bmp");
        
        unsigned int size = 62 + width*width;
        
        string header = "";
        header.assign("\x42\x4D",2);
        geoff << header;
        geoff.write((char*)&size, 1);
        header.assign("\x00\x00\x00\x00\x00\x00\x00\x3E\x00\x00\x00\x28\x00\x00\x00",15);
        geoff << header;
        geoff.write((char*)&width, 1);
        header.assign("\x00\x00\x00",3);
        geoff << header;
        geoff.write((char*)&width, 1);
        header.assign("\x00\x00\x00\x01\x00\x01\x00\x00\x00\x00\x00\x64\x00\x00\x00\xc4\x0e\x00\x00\xc4\x0e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\x00", 39);
        geoff << header;
        
        
        unsigned int modbit = 0;
        unsigned int bit = 0;
        unsigned int byte = 0;
        unsigned int endLine = 128; 
        int j = 0;
        int k = 0;
        int i = 0;
        
        
        for(int f = width*width-1; f >= 0; f--){
            i = (width*(f/width)) + (width-(f % width)) - 1;
            //cout <<"s I " << i << " :: K " << k << " :: J " << j << endl;
            modbit = data[i] % 2;
            if(modbit == 1){
                bit = 0;
            } else {
                bit = 1;
            }

            //cout <<" MOD " << modbit << " I " << i  << " : K " << k << endl;
            byte = byte + (bit * pow(2,(7-(k % 8))));
            //cout << " fresh byte " << byte << endl;
                       
            if(k % 8 == 7){ // 7, 15, 23 etc.
                //write byte
                geoff.write((char*)&byte, 1);
                byte = 0;
                j++;
                
            }
            
            if(k == (width - 1)) {
                //endLine = bit * pow(2,0);
                geoff.write((char*)&byte, 1);
                j = 0;
                k = 0;
                byte = 0;
                //return 0;
            } else {
                k++;
            }
            if(i == 60){
                //return 0;
            }
            //cout <<"e I " << i << " :: K " << k << " :: J " << j << endl;
        }

        geoff.close();
       
        string stringblock;
        char* memblock;
        int size2;
        try {
            BufferedAsyncSerial serial(USBPort, 9600);
            
            ifstream geoff2("bmpting.bmp", ios::in|ios::binary|ios::ate);
            if (geoff2.is_open()) {
                size2 = geoff2.tellg();
                memblock = new char [size2];
                geoff2.seekg (0, ios::beg);

                stringblock = "";

                stringblock+=("\x1b");

                geoff2.read(memblock, size2);
                geoff2.close();
            }
            for (int i = 0; i < size2; i++) { // doesn't work with '\0' either
                stringblock+=(memblock[i]);
            }
            serial.writeString(stringblock);
            cout << stringblock;
            serial.writeString("\x1d\x2f\x3");

            serial.close();
  
        } catch(boost::system::system_error& e) {
                cout<<"Error: "<<e.what()<<endl;
                return 1;
        }
}