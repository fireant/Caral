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
