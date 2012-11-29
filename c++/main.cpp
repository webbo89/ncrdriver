#include <iostream>
#include <fstream>
#include <boost/thread.hpp>
#include "BufferedAsyncSerial.h"
#include <string>

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
	string line;
	ifstream::pos_type size;
	char * memblock;
	char * cmdblock;
	string stringblock;

	try {
        //BufferedAsyncSerial serial("/dev/ttyUSB0",9600);
		if(argc > 1) {
			ifstream inputfile (argv[1], ios::in|ios::binary|ios::ate);
			if (inputfile.is_open())
			{
				size = inputfile.tellg();
				memblock = new char [size];
				inputfile.seekg (0, ios::beg);

				//cout <<'\x1b';
				//serial.writeString("\x1b"); // BMP to RAM pg164 of NCR7167
				stringblock = "";

				//serial.writeString("\x1b");

				inputfile.read (memblock, size);
				inputfile.close();

				for (int i = 0; i < size; i++) //  doesn't work with '\0' either
				{
					stringblock += memblock[i];//append((char*)&memblock[i], 1);
					if(memblock[i] != ' '){
					//serial.write((char *)&memblock[i], 1);					
					cout << memblock[i];
					}
				}
				//cout << stringblock << endl;
				
				//serial.writeString('\x1b' + stringblock);

				//serial.write(stringblock.c_str(), (int)size);

				//serial.writeString("\n");
				
				//cout << "Written memblock and vblf \n";
			
				//serial.writeString("\x1d\x2f\x3"); // print RAM image normal density pg171 of NCR7167

				//cout << "Done" << "\n";

				delete[] memblock;
			}
		}
		cout << "closing serial ";
		//serial.close();
  
    } catch(boost::system::system_error& e)
    {
        cout<<"Error: "<<e.what()<<endl;
        return 1;
    }
}
