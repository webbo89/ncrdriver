#include <iostream>
#include <fstream>
#include <boost/thread.hpp>

#include "BufferedAsyncSerial.h"

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
	string line;

	try {
        BufferedAsyncSerial serial("/dev/ttyUSB0",9600);
		if(argc > 1) {
			ifstream inputfile (argv[1]);
			if (inputfile.is_open())
			{
			serial.writeString("\x1b"); // BMP to RAM pg164 of NCR7167

				while ( inputfile.good() )
				{
					getline(inputfile,line);
					serial.write(line);
				}
				inputfile.close();
			}
			
			serial.writeString("\x1d\x2f\x0"); // print RAM image normal density pg171 of NCR7167
	
		}

        //Simulate doing something else while the serial device replies.
        //When the serial device replies, the second thread stores the received
        //data in a buffer.
        //this_thread::sleep(posix_time::seconds(2));

        //Always returns immediately. If the terminator \r\n has not yet
        //arrived, returns an empty string.
        //cout<<serial.readStringUntil("\r\n")<<endl;

        serial.close();
  
    } catch(boost::system::system_error& e)
    {
        cout<<"Error: "<<e.what()<<endl;
        return 1;
    }
}
