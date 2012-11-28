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

string getHex(int byte){
    stringstream stream;
    stream << hex << byte;
    
    return"0x" + stream.str();
}

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
        string USBPort = "";
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
        shortURL = "http://icrobot.net/";// += getShortcode(time);
        
        //create qr
        qrcode = QRcode_encodeString(shortURL.c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 0);
        
        //create bitmap
        data = qrcode->data;
        width = qrcode->width;
        
        
        //cout << width << endl;
 
        string header = "";
        header.assign("\x42\x4D\xA2\x00\x00\x00\x00\x00\x00\x00\x3E\x00\x00\x00\x28\x00\x00\x00\x19\x00\x00\x00\x19\x00\x00\x00\x01\x00\x01\x00\x00\x00\x00\x00\x64\x00\x00\x00\xc4\x0e\x00\x00\xc4\x0e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\x00", 62);
        
        ofstream geoff("bmpting.bmp");
        
        geoff << header;
        
        
        unsigned int modbit = 0;
        unsigned int bit = 0;
        unsigned int byte = 0;
        unsigned int endLine = 128; 
        int j = 0;
        int k = 0;
        int i = 0;
        
        for(int f = 624; f >= 0; f--){
            i = (25*(f/25)) + (25-(f % 25)) - 1;
            cout <<"s I " << i << " :: K " << k << " :: J " << j << endl;
            modbit = data[i] % 2;
            if(modbit == 1){
                bit = 0;
            } else {
                bit = 1;
            }

            cout <<" MOD " << modbit << " I " << i  << " : K " << k << endl;
            byte = byte + (bit * pow(2,(7-(k % 8))));
            cout << " fresh byte " << byte << endl;
                       
            if(k % 8 == 7){ // 7, 15, 23 etc.
                //write byte
                geoff.write((char*)&byte, 1);
                byte = 0;
                j++;
                
            }
            
            if(j > 2 && k == 24) {
                endLine = bit * pow(2,7);
                geoff.write((char*)&endLine, 1);
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
            cout <<"e I " << i << " :: K " << k << " :: J " << j << endl;
        }

        geoff.close();
       
        
        /*
        try {
            BufferedAsyncSerial serial(USBPort, 9600);
            
            stringblock = ('\x1b');
            
            //memblock = QRcode_encodeString("http://www.google.co.uk", 0, QR_ECLEVEL_H, QR_MODE_8, 0)->data;
            
            qrcode = 

            data = qrcode->data;
            width = qrcode->width;
            
            cout << width;
            QRcode_free(qrcode);
            
            ss << data;
            
            stringblock += ss.str();
            
            serial.writeString(stringblock);
            
            serial.writeString("\x1d\x2f\x0");

            serial.close();
  
        } catch(boost::system::system_error& e) {
                cout<<"Error: "<<e.what()<<endl;
                return 1;
        }*/
}