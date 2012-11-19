#include <iostream>
#include <fstream>
#include <boost/thread.hpp>

#include "BufferedAsyncSerial.h"

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
	string line;
	ifstream::pos_type size;
	char * memblock;

	try {
        BufferedAsyncSerial serial("/dev/ttyUSB0",9600);
		if(argc > 1) {
			ifstream inputfile (argv[1], ios::in|ios::binary|ios::ate);
			if (inputfile.is_open())
			{
				serial.writeString("\x1b"); // BMP to RAM pg164 of NCR7167
				size = inputinputfile.tellg();
				memblock = new char [size];
				inputfile.seekg (0, ios::beg);
				inputfile.read (memblock, size);
				inputfile.close();

				serial.write(memblock, size);
			
				serial.writeString("\x1d\x2f\x0"); // print RAM image normal density pg171 of NCR7167

				delete[] memblock;
		}

        serial.close();
  
    } catch(boost::system::system_error& e)
    {
        cout<<"Error: "<<e.what()<<endl;
        return 1;
    }
}