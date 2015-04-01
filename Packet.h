#ifndef PACKET_H
#define PACKET_H

#include <bitset>
#include <stdint.h>
#include <iostream>
#include "Exception.h"


/*
 Packet Header: ID, Checksum, Flags, Data
 */

// Max size of the packet.
#define PACKET_SIZE 1024

// Flag definitions
#define IS_ACK_PACKET 0x01
#define LAST_PACKET 0x10
#define IS_ACKED 0x100

class Packet{
public:
    Packet();
    Packet(char * aRawData, size_t aNumBytes);
    Packet(char * aContentData, size_t aNumBytes, uint32_t aID, uint16_t aFlags);
    ~Packet();

    // Getters
    char * GetRawData();
    size_t GetSize();
    uint16_t GetID() const { return mID; }
    char * GetMData() const { return mData; }
    size_t GetMDataSize() const { return mNumDataBytes; }
    const uint16_t getCheckSum() const { return mCheckSum; }

    const bool isAcked() const { return (mFlags & IS_ACKED) != 0; }
    const bool isAckPacket() const { return (mFlags & IS_ACK_PACKET) != 0; }
    const bool isLastPacket() const { return (mFlags & LAST_PACKET) != 0; }
    const bool isEmpty() const { return mData == NULL; }

    void Print() const;

    // Setters
    void setId(uint32_t id) { mID = id; }
    void setIsAcked() { mFlags |= IS_ACKED; }
    void setAckPacket() { mFlags |= IS_ACK_PACKET; }
    void setLastPacket() { mFlags |= LAST_PACKET; }
    void clear();

private:
    // From: http://minirighi.sourceforge.net/html/ip_8c-source.html
    uint16_t ip_checksum(const void *buf, size_t hdr_len);

    // Member variables:
    char * mData;
    size_t mNumDataBytes;
    uint32_t mID;
    uint16_t mCheckSum;
    uint16_t mFlags;
};


#endif//PACKET_H
