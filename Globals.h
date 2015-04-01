#ifndef GLOBALS_H
#define GLOBALS_H

#define PACKET_SIZE             1024    //Full packet size in bytes, including data and headers
#define PACKET_HEADERS_SIZE     8       //Packet header size in bytes (32 bit ID, 16 bit checksum, 16 bit flags)
#define PACKET_DATA_SIZE        1016    //Packet data size in bytes

#endif//GLOBALS_H
