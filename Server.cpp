
#include <iostream>
#include <fstream>
#include <vector>
# include <string>
#include<cstring>
#include <stdio.h>
#include<stdint.h>
#include <iomanip>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/chrono.hpp>
#include <boost/chrono/include.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <ctime>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/crc.hpp>      // for boost::crc_basic, boost::crc_optimal
#include <boost/cstdint.hpp>  // for boost::uint16_t
#include <algorithm>  // for std::for_each
#include <cstddef>    // for std::size_t
#include <thread>
#include <random>
#include <chrono>
#include <cstdlib>
#include <boost/bind.hpp>

using namespace std;
using boost::asio::ip::tcp;
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;


string hexStr(vector<uint8_t> vect)
{
	int len = vect.size();
	stringstream ss;
	ss << hex;
	for (int i = 0; i < len; i++)
		ss << setw(2) << setfill('0') << (int)vect[i] << " ";
	return ss.str();

}

void vect_append(vector<uint8_t>& vect, int value)

{
	unsigned char byte_lower = value & 0xff;
	unsigned char byte_upper = (value >> 8) & 0xff;
	//cout << "size lower byt" << " " << unsigned(byte_lower) << " " << "size upper byt" << " " << unsigned(byte_upper) << endl;
	vect.push_back(byte_lower);
	vect.push_back(byte_upper);

}


vector<uint8_t> string_to_packet(string data)
{

	// Create an empty vector
	vector<uint8_t> vect;
	int header = 0x02;
	unsigned char ahead = (unsigned char)header;
	vect.push_back(ahead);
	//std::cout << "Random data string:" << data << '\n';

	// packet size excluding data string size
	int pacsize = 6;
	auto size1 = data.size() + pacsize;
	//cout << "packet size including string is:" << " " << size1 << endl;
	// size lower and upper byte
	int hsize = data.size() + 6;
	//calling function to append values for size bytes
	vect_append(vect, hsize);
	// insert random string byte by byte in empty vector
	vector<uint8_t>::iterator it;
	it = vect.begin();
	vect.insert(it + 3, data.begin(), data.end());


	// Simulate CRC-CCITT
	boost::crc_basic<16>  crc_ccitt1(0x1021, 0xFFFF, 0, false, false);
	crc_ccitt1.process_bytes(&vect[1], vect.size() - 1);
	int crcvalue = crc_ccitt1.checksum();
	//cout << "crc value is :" << " " << crcvalue << endl;

	// calling function to append crc lower and upper byte values
	vect_append(vect, crcvalue);

	// trailor conversion & append vector with trailor byte
	int trailor = 0x03;
	unsigned char tra = (unsigned char)trailor;
	// cout << "trailor is:" << unsigned(tra) << endl;
	vect.push_back(tra);

	//  calling function to calculate hex values of vector packet

	string c = hexStr(vect);
	// cout << "The hex value of packet is :" << c << endl;
	return vect;

}


vector<uint8_t> get_random_packet(vector<string> data)
{

	// Create an empty vector
	vector<uint8_t> vect;
	auto size = data.size();
	int index = rand() % size; // index is now random
	vect = string_to_packet(data[index]);
	return vect;
}

vector<uint8_t> get_vector(vector<string> data) {
	vector <uint8_t> v;
	for (int i = 0; i < 20; i++) {
		v = get_random_packet(data);
	}
	return v;
}


// write data from vector
class session
{

public:
	session(boost::asio::io_service& io_service)
		: socket_(io_service), 
		io_service_(io_service)
	{

		
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		handle_write(boost::system::error_code(), 0);

	}
private:
	void handle_write(const boost::system::error_code& error, size_t bytes_transferred)
	{
	
		vector<string> data = { "JKLrrr", "alphaaa", "betaaa", "gammaa", "ashhhhhh", "pack234", "packetabc", "packet234", "packe1hz", "mypacket" };
		count++;
		if (!error && (count < 20))
		{

			v1 = get_vector(data);
		    //Construct a timer with an absolute expiry time.
			boost::asio::steady_timer timer(io_service_, std::chrono::steady_clock::now() + std::chrono::milliseconds(5000));
			 //Start an asynchronous wait.
			timer.async_wait([](auto ... err)
				{
					mt19937_64 eng{ random_device{}() };  // or seed however you want
					uniform_int_distribution<> dist{ 1000, 6000 };
		
					std::cout << boost::posix_time::second_clock::local_time() << " " << "enter.\n";
					// random pause between packets from 1 to 6 sec
					std::this_thread::sleep_for(std::chrono::milliseconds{ dist(eng) });
					std::cout << boost::posix_time::second_clock::local_time() << " " << "leave .\n";
				}
			);

			// function for sending data
			boost::asio::async_write(socket_,
				boost::asio::buffer(v1, v1.size()),
				boost::bind(&session::handle_write, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
			
			
		}
		else
		{
			delete this;

		}
	}

private:
	boost::asio::io_service& io_service_;
	tcp::socket socket_;
	vector <uint8_t> v1;
	int count = 0;

};

//// server: build tcp ip server class
class server
{
	// the constructor initialise an acceptor to listen on tcp port 1234
public:
	server(boost::asio::io_service& io_service)
		: io_service_(io_service),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), 5005))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		session* new_session = new session(io_service_);
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}
	// function handle_accept() creates socket and initiate asynchronous accept operation for new connection
	void handle_accept(session* new_session,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_session->start();
		}
		else
		{
			delete new_session;
		}

		start_accept();

	}

	boost::asio::io_service& io_service_; // io service
	tcp::acceptor acceptor_; // tcp connection
};



int main()
{

	try
	{
		// run the io_service object so that it perform asynchronous operation
		boost::asio::io_service io_service;
		server s(io_service);
		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;


}






