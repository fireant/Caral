/* Copyright (c) 2011 Mosalam Ebrahimi <m.ebrahimi@ieee.org>
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <boost/asio.hpp>

// SimpleSerial class from http://gitorious.org/serial-port
class SimpleSerial
{
public:
    SimpleSerial(std::string port, unsigned int baud_rate)
    : io(), serial(io,port)
    {
        serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    }

    void writeString(std::string s)
    {
        boost::asio::write(serial,boost::asio::buffer(s.c_str(),s.size()));
    }

    std::string readLine()
    {
        //Reading data char by char, code is optimized for simplicity, not speed
        using namespace boost;
        char c;
        std::string result;
        for(;;)
        {
            asio::read(serial,asio::buffer(&c,1));
            switch(c)
            {
                case '\r':
                    break;
                case '\n':
                    return result;
                default:
                    result+=c;
            }
        }
    }

private:
    boost::asio::io_service io;
    boost::asio::serial_port serial;
};

using namespace std;

int main() {

	SimpleSerial serial("/dev/ttyUSB1",9600);

	FILE* gfeed = popen("feedGnuplot --lines --ymin -1 --ymax 361 --stream -xlen 200", "w");
	for (;;) {
		string line = serial.readLine();
		if (strncmp(line.c_str(), "Current", 4) != 0) 
			continue;
		cout<<line<<endl;
		float y = 0.0f;
		sscanf(line.c_str(), "Current heading: %f degrees\n", &y);
		fprintf(gfeed, "%f \n", y);
		fprintf(gfeed, "replot\n");
		fflush(gfeed);

		usleep(10000);
	}
	pclose(gfeed);
	return 0;
}
