#ifndef GLOBALS_H
#define GLOBALS_H

#define PACKET_SIZE             1024    //Full packet size in bytes, including data and headers
#define PACKET_HEADERS_SIZE     8       //Packet header size in bytes (32 bit ID, 16 bit checksum, 16 bit flags)
#define PACKET_DATA_SIZE        1016    //Packet data size in bytes

#define ACK_DATA                "ACK"   //Data inside of an ACK packet
#define ACK_DATA_SIZE           3       //Size of ACK Data (no \0)

#endif//GLOBALS_H
