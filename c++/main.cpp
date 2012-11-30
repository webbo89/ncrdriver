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
    int length = 28;
    char base[29] = "abcdefghjkmnpqrstwxz23456789";
    string out = "";
    string shorturl = "";

    while(date > length - 1)
    {
        out = base[date % length] + out;
        date = floor( date / length );
    }

    shorturl += base[date];
    shorturl += (out);
        
    return shorturl;
}


int main(int argc, char* argv[])
{
        string USBPort = "";
        bool cut = true;
        int precutNewlines = 0; 
        string message = "";
        string line = "";
        int time;
        string shortURL;
        bool beep = true;
        ifstream messageFile;
        
        int width;
        unsigned char * data;
        stringstream ss;
        QRcode *qrcode;

        // Check enough parameters
        if (argc != 7){
            cout << "Incorrect number of parameters (expecting ./async port(int) time(epoch) messagepath cut(bool) lines(int) beep(bool)" << endl;
            cout << argc << " parameters given" << endl;
            return 1;
        }
        
        // Set USB port for serial communication
        if(argc > 0){
            USBPort = "/dev/ttyUSB";
            USBPort += argv[1];
        }
        
        // Set time of interaction (epoch expected)
        string strTime = "";
        if(argc > 1){
            strTime = argv[2];
            strTime = strTime.substr(3);
            time = atoi(strTime.c_str());
        }
        
        // Set file path to message
        if(argc > 2){
            messageFile.open(argv[3]);
        }
        
        // Set cut value, default to true
        if(argc > 3){
            if(argv[4] == "false" | argv[4] == "no"){
                cut = false;
            }
        }
        
        // Set new line count before cut - default to 3
        if(argc > 4){
            precutNewlines = atoi(argv[5]);
        }
        
        // Set beep value, default to true
        if(argc > 5){
            if(argv[6] == false){
                beep = false;
            }
        }
        
        // Read text file into message
        while(messageFile.good()){
           getline(messageFile,line);
           message += line + "\n";
        }
        
        // Make shorturl
        shortURL = "http://icrobot.net/" + getShortcode(time);
        
        // Create QRCode
        qrcode = QRcode_encodeString(shortURL.c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 0);
        
        // Create bitlmap
        data = qrcode->data;
        width = qrcode->width;
        
        ofstream geoff("bmpting.bmp", ofstream::binary);
        

		int realwidth = width*4;
        unsigned int size = 62 + (realwidth*(realwidth + (8- (realwidth % 8))))/8;
        
        string header = "";
        geoff.write("\x42\x4D",2);
        geoff.write((char*)&size, 1);

        header.assign("\x00\x00\x00\x00\x00\x00\x00\x3E\x00\x00\x00\x28\x00\x00\x00",15);
        geoff << header;
        geoff.write((char*)&realwidth, 1);
        header.assign("\x00\x00\x00",3);
        geoff << header;
        geoff.write((char*)&realwidth, 1);
        header.assign("\x00\x00\x00\x01\x00\x01\x00\x00\x00\x00\x00\x64\x00\x00\x00\xc4\x0e\x00\x00\xc4\x0e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\x00", 39);
        geoff << header;

        // QR to bitmap algorithm
        unsigned int modbit = 0;
        unsigned int bit = 0;
        unsigned int byte = 0;
        int i = 0, j = 0, k = 0, vline = 0;
		int p = 0; // Pixel
        
        for(int f = width-1; f >= 0; f--){
			vline = ((f/width));
			for (int g=0; g <4; g++) {
				for (i = realwidth-1; i >= 0; i--) {
					p = vline*width + (i%width);
					modbit = data[p] % 2;
					if(modbit == 1){
						bit = 0;
					} else {
						bit = 1;
					}

					byte = byte + (bit * pow(2,(7-(k % 8))));
							   
					if(k % 8 == 7){ // 7, 15, 23 etc.
						geoff.write((char*)&byte, 1);
						byte = 0;
						j++;
					}
					
					if(k == (width - 1)) {
						geoff.write((char*)&byte, 1);
						j = 0;
						k = 0;
						byte = 0;
					} else {
						k++;
					}
				}
			}
        }

        geoff.close();
       
        
        /*
        string stringblock;
        char* memblock;
        ifstream::pos_type size2;
        try {
            BufferedAsyncSerial serial(USBPort, 9600);
            
            ifstream geoff2("bmpting.bmp", ios::in|ios::binary|ios::ate);
            if (geoff2.is_open()) {
                size2 = geoff2.tellg();
                memblock = new char [size2];
                geoff2.seekg (0, ios::beg);

                stringblock = "";

                stringblock += ("\x1b");

                geoff2.read(memblock, size2);
                geoff2.close();
            }
            for (int i = 0; i < size2; i++) {
                stringblock += (memblock[i]);
            }
            serial.writeString(stringblock);
            cout << stringblock;
            serial.writeString("\n");
            serial.writeString("\x1d\x2f\x3");
            serial.writeString("image worked!!!!");
            serial.close();
  
        } catch(boost::system::system_error& e) {
                cout<<"Error: "<<e.what()<<endl;
                return 1;
        }*/
        
        /*
        ifstream::pos_type filesize;
        char * memblock;
        string stringblock;

        try {
            BufferedAsyncSerial serial("/dev/ttyUSB0",9600);
                
            ifstream inputfile ("smallface.bmp", ios::in|ios::binary|ios::ate);
            if (inputfile.is_open())
            {
                filesize = inputfile.tellg();
                memblock = new char [filesize];
                inputfile.seekg (0, ios::beg);

                //cout <<'\x1b';
                serial.writeString("\x1b"); // BMP to RAM pg164 of NCR7167
                stringblock = "";

                stringblock+=("\x1b");

                inputfile.read (memblock, filesize);
                inputfile.close();

                for (int i = 0; i < filesize; i++) // doesn't work with '\0' either
                {
                    stringblock+=(memblock[i]);
                }

                //serial.writeString("\x1b");

                cout << memblock << endl;
                cout << "Just about to write memblock " << filesize << endl;

                serial.writeString(stringblock);

                serial.writeString("\n");

                cout << "Written memblock and vblf" << endl;

                serial.writeString("\x1d\x2f\x0"); // print RAM image normal density pg171 of NCR7167

                cout << "Done" << endl;

                delete[] memblock;
            }
            cout << "closing serial" << endl;
            serial.close();
                } catch(boost::system::system_error& e)
                {
                    cout<<"Error: "<<e.what()<<endl;
                    return 1;
                }  */    
}