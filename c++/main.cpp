#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <sstream>
#include "BufferedAsyncSerial.h"
#include "../libqrencode/qrencode.h"


using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
        string stringblock;
        unsigned char * data;
        int width;
        stringstream ss;
        QRcode *qrcode;
        
        try {
            BufferedAsyncSerial serial("/dev/ttyUSB0",9600);
            
            stringblock = ('\x1b');
            
            //memblock = QRcode_encodeString("http://www.google.co.uk", 0, QR_ECLEVEL_H, QR_MODE_8, 0)->data;
            
            qrcode = QRcode_encodeString("abcdefghijk123456789012", 0, QR_ECLEVEL_H, QR_MODE_8, 0);

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
        }
}
