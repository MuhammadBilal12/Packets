# Packets
Coverting data strings into packets having a structure or including header , size, data , crc value, trailer

Two programs of server and client. The server is creating a random sequence of packets the format of 
1) Header 0x02 , fixed value
2) Size : packet size , not including header and trailer
3) Data: random sequence bytes
4) CRC: A 16 bit CRC . The boost library has code to calculate CRCs
5) Trailer: 0x03 , fixed value
