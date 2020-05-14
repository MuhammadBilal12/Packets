
//Muhammad Bilal 
 

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
#include <boost/asio/deadline_timer.hpp>
#include <ctime>
#include <boost/crc.hpp>      // for boost::crc_basic, boost::crc_optimal
#include <boost/cstdint.hpp>  // for boost::uint16_t
#include <algorithm>  // for std::for_each
#include <cstddef>    // for std::size_t
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
   
    std::stringstream ss;
    ss << std::hex;

   
    for (int i = 0; i < len; i++)
        ss << std::setw(2) << std::setfill('0') << (int)vect[i] << " ";
    return ss.str();

}
    void func1(vector<uint8_t>& vect, int value) {
    unsigned char sizelower = value & 0xff;
    unsigned char sizeupper = (value >> 8) & 0xff;
   // cout << "size lower byt" << " " << sizelower << " " << "size upper byt" << " " << sizeupper << endl;


  
    vect.push_back(sizelower);
    vect.push_back(sizeupper);
   
    


}




int main()
{
    // header, trailor
    int header = 0x02;
    int trailor = 0x03;
    // conversion of int values to unsigned char  
    unsigned char ahead;
    ahead = (unsigned char)header;
    cout << "Header is:" << unsigned(ahead) << endl;
    // trailor conversion
    unsigned char tra;
    tra = (unsigned char)trailor;
    cout << "trailor is:" << unsigned(tra) << endl;


    // Create an empty vector
    std::vector<std::uint8_t> vect;

    // calculate size from whole vector
    //  vector string for data
    std::vector<std::string> data = { "JKL", "alpha", "beta", "gamma" };
    //int index; // Counter variable
    auto size = data.size();
    cout << "size of data string is:" << size << endl;

    // loop to update hex values for each packet in vector
    for (int index = 0; index < size; index++)

    {
        vect.clear();
        // append vector with header marker
        vect.push_back(ahead);
        //print out each string in vector data
        std::cout << "data string is:" << data[index] << '\n';

        // caclulating size of whole packet
        std::string s = data[index];
        // packet size excluding data string size
        int pacsize = 6;
        auto size1 = data[index].size() + pacsize;
        cout << "packet size including string is:" << " " << size1 << endl;
        // size lower and upper byte
        int hsize = data[index].size() + 6;

         func1(vect, hsize);
        //vector<uint8_t> vect = func1(vect,hsize);
        //////////////////common size part///////////////////
              // appending upper and lower values of size
    //    unsigned char sizelower = hsize & 0xff;
    //    unsigned char sizeupper = (hsize >> 8) & 0xff;
    //    cout << "size lower byt" << " " << sizelower << " " << "size upper byt" << " " << sizeupper << endl;
    //    vect.push_back(sizelower);
    //    vect.push_back(sizeupper);



        /////////////////////////////////////

        // insert string byte by byte in empty vector
        std::vector<std::uint8_t>::iterator it;
        it = vect.begin();
        vect.insert(it + 3, data[index].begin(), data[index].end());

        // CRC
        std::uint8_t* a = &vect[1]; // this is just pointer excluding header marker 02
        std::size_t const    data_len = vect.size() - 1;
        cout << "length of crc data : " << data_len << endl;
        // Simulate CRC-CCITT
        boost::crc_basic<16>  crc_ccitt1(0x1021, 0xFFFF, 0, false, false);
        crc_ccitt1.process_bytes(a, data_len);
        int crcvalue = crc_ccitt1.checksum();
        cout << "my new value :" << " " << crcvalue << endl;



          func1(vect, crcvalue);



        /////////////////////common//////////////////
      //  unsigned crc = crcvalue;
      //  unsigned crclower = crc & 0xff;
      //  unsigned crcupper = (crc >> 8) & 0xff;
      //  cout << "crc lower byt" << " " << crclower << " " << "crc upper byt" << " " << crcupper << endl;
        // append empty vector with crc
     //   vect.push_back(crclower);
     //   vect.push_back(crcupper);

        /////////////////////////////////



        // append vector with trailor byte
        vect.push_back(tra);



        //  function to calculate hex values of vector packet
        cout << "packet" << " " << index << " " << "is :" << endl;
        string c = hexStr(vect);
        cout << "The hex value of the string is :" << c << endl;


    }






}
