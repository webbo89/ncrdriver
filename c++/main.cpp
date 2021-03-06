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
        bool debug = false;
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
        USBPort = "/dev/ttyUSB";
        USBPort += argv[1];
        
        // Set time of interaction (epoch expected)
        string strTime = "";
        strTime = argv[2];
        strTime = strTime.substr(3);
        time = atoi(strTime.c_str());
        
        // Set file path to message
        messageFile.open(argv[3]);
        
        // Set cut value, default to true
        if(string(argv[4]) == "false"){
            cut = false;
        }
        
        // Set new line count before cut - default to 3
        precutNewlines = atoi(argv[5]);
        
        // Set beep value, default to true
        if(string(argv[6]) == "false"){
            beep = false;
        }
        

        
        // Make shorturl
        shortURL = "http://icrobot.net/" + getShortcode(time);
        
        // Create QRCode
        qrcode = QRcode_encodeString(shortURL.c_str(), 0, QR_ECLEVEL_H, QR_MODE_8, 0);
        
        // Create bitmap
        data = qrcode->data;
        width = qrcode->width;
        
        string filename = "../qrcodes/temp"+getShortcode(time)+".bmp";
        ofstream bmpfile(filename.c_str(), ofstream::binary);

        int realwidth = width*4;
        unsigned int size = 62 + (realwidth*(realwidth + (8- (realwidth % 8))))/8;
        
        bmpfile.write("\x42\x4D",2);
        bmpfile.write("\x00\x00\x00\x00\x00\x00\x00\x00\x3E\x00\x00\x00\x28\x00\x00\x00",16);
        bmpfile.write((char*)&realwidth, 1);
        bmpfile.write("\x00\x00\x00",3);
        bmpfile.write((char*)&realwidth, 1);
        bmpfile.write("\x00\x00\x00\x01\x00\x01\x00\x00\x00\x00\x00\x40\x07\x00\x00\xc4\x0e\x00\x00\xc4\x0e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xff\x00", 39);

        // QR to bitmap algorithm
        unsigned int modbit = 0;
        unsigned int bit = 0;
        unsigned int byte = 0;

        int i = 0, j = 0, k = 0, yline = 0, xline = 0, h= 0;
        int p = 0; int zero = 0; // Pixel
						//lines   x pixel
        int bitmapArray[width][width];
        
        for(i = 0; i < width; i++){
            for(j = 0; j < width; j++){
                bitmapArray[i][j] = 0;
            }
        }
        
        // ***** QR_ENCODE TO BITMAP BUILDER ***** //
        for (int e = width*width-1; e>=0; e--) {
            yline = e/width;
            xline = e%width;
            modbit = data[e] % 2;
            if(modbit == 1){
                    bit = 0;
            } else {
                    bit = 1;
            }		
            bitmapArray[yline][xline] = bit;
        }

        // ***** QR_ENCODE TO BITMAP DEBUG COUT ***** //
        if (debug) {
            cout << "QR encode to BITMAP COUT width: " << width << endl;
            for (int nRow = 0; nRow < width; nRow++)	{
                for (int nCol = 0; nCol < width; nCol++) {
                    if(bitmapArray[nRow][nCol] == 1){
                            cout << ".";
                    } else {
                            cout << "#";
                    }
                    //cout << bitmapArray[nRow][nCol];
                }

                cout << endl;
            }
        }
		
        int bitmapArrayX4[realwidth][realwidth];
        // ***** BITMAP to BITMAP X4 ***** //
        for (int yloop = 0; yloop < width; yloop++) {
            for (int e = 0; e < 4; e++) {	// Line repeater by 4
                for (int xloop = 0; xloop < width; xloop++) {
                    bitmapArrayX4[4*yloop+e][xloop*4] = bitmapArray[yloop][xloop];
                    bitmapArrayX4[4*yloop+e][xloop*4+1] = bitmapArray[yloop][xloop];
                    bitmapArrayX4[4*yloop+e][xloop*4+2] = bitmapArray[yloop][xloop];
                    bitmapArrayX4[4*yloop+e][xloop*4+3] = bitmapArray[yloop][xloop];
                }
            }
        }
				
		
        // ***** BITMAP X4 DEBUG COUT ***** //
        if (debug) {
            cout << "BITMAP X4 COUT width: " << realwidth << endl;
            for (int nRow = 0; nRow < realwidth; nRow++)	{
                for (int nCol = 0; nCol < realwidth; nCol++) {
                    if(bitmapArrayX4[nRow][nCol] == 1){
                            cout << ".";
                    } else {
                            cout << "#";
                    }
                }

                cout << endl;
            }
        }

        for (int yloop = realwidth-1; yloop >= 0; yloop--) {
        //Print a new bmp line
        k = 0;
        byte = 0;
        
            for (int xloop = 0; xloop < realwidth; xloop++) {
                //bmp pixel
                
                bit = bitmapArrayX4[yloop][xloop];
                byte = byte + (bit * pow(2,(7-(k % 8))));
                //Saves a bmp byte
                if(k % 8 == 7){ // 7, 15, 23 etc.
                    bmpfile.write((char*)&byte, 1);
                    byte = 0;
                }
                k++;
            
            }//End of line of pixels
            if(k % 8 != 7){ // 7, 15, 23 etc.
                bmpfile.write((char*)&byte, 1);
                byte = 0;
            }
            if((realwidth % 8) != 0){
                if((((realwidth % 8) + 1) % 2) == 1){
                    //odd put zero char
                    bmpfile.write((char*)&zero,1);
                }
            } else {
                if((((realwidth % 8)) % 2) == 1){
                    //odd put zero char
                    bmpfile.write((char*)&zero,1);
                }
            }
        }
				
        bmpfile.close();
       
        char* memblock;
        ifstream::pos_type qrsize;
        try {
            // Open serial connection
            BufferedAsyncSerial serial(USBPort, 9600);
            
            // Print message text
            // Read text file into message
            while(messageFile.good()){
               getline(messageFile,line);
               serial.writeString(line+'\n');
            }
            messageFile.close();
            
            // Print some newlines before the QR code + link text
            serial.writeString("\n\n");
            serial.writeString(shortURL);
            serial.writeString("\n\n");
            
            
            // Read bitmap and print 
            ifstream qrbitmap(filename.c_str(), ios::in|ios::binary|ios::ate);
            if (qrbitmap.is_open()) {
                qrsize = qrbitmap.tellg();
                memblock = new char [qrsize];
                qrbitmap.seekg (0, ios::beg);

                serial.writeString("\x1b");

                qrbitmap.read(memblock, qrsize);
                qrbitmap.close();
            }
            
            for (int i = 0; i < qrsize; i++) {
                serial.write((char *)&memblock[i], 1);
            }
            
            serial.writeString("\n");
            serial.writeString("\x1d\x2f\x3");
            serial.writeString("\n\n\n\n\n\n");
            
            // Do spaces required
            for(int z = 0; z < precutNewlines; z++){
                serial.writeString("\n");
            }

            // Do cut if required
            if(cut){
                serial.writeString("\x1b\x69");
            }
            
            // Beep if required
            if(beep){
                serial.writeString("\x1b\x07");
            }
            
            
            // Close serial port
            serial.close();
            
        } catch(boost::system::system_error& e) {
                cout<<"Error: "<<e.what()<<endl;
                return 1;
        }
          
}