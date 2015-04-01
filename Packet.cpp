#include "Packet.h"

Packet::Packet()
{
    mData = NULL;
    mNumDataBytes = 0;
    mID = 0;
    mCheckSum = 0;
    mFlags = 0;
}

Packet::Packet(char * aRawData, size_t aNumBytes)
{
    // Parse the packet fields from the raw data.
    char * p = aRawData;
    memcpy(&mID, p, sizeof(uint32_t));
    p += sizeof(uint32_t);

    memcpy(&mCheckSum, p, sizeof(uint16_t));
    p += sizeof(uint16_t);

    memcpy(&mFlags, p, sizeof(uint16_t));
    p += sizeof(uint16_t);

    size_t lDataNumBytes = aNumBytes - (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t));
    mData = (char *)malloc(lDataNumBytes);
    memcpy(&mData, p, lDataNumBytes);
    mNumDataBytes = lDataNumBytes;
}

Packet::Packet(char * aContentData, size_t aNumBytes, uint32_t aID, uint16_t aFlags)
{
    if(PACKET_SIZE >= (sizeof(mID) + sizeof(mCheckSum) + sizeof(mFlags) + aNumBytes))
    {
        // Copy the data into this object.
        mNumDataBytes = aNumBytes;
        mData = (char *)malloc(mNumDataBytes);
        memcpy(mData, aContentData, mNumDataBytes);

        // Calculate the 16 bit IP checksum of the data.
        mCheckSum = ip_checksum(mData, mNumDataBytes);

        // Save the flags.
        mFlags = aFlags;

        // Save the ID.
        mID = aID;
    }
    else
    {
        // Throw an exception since the packet is too large for the specifications.
        throw new GeneralException("Packet too large.");
    }
}

Packet::~Packet()
{
    if(mData != NULL)
    {
        // free(mData);
        mData = NULL;
    }
    mNumDataBytes = 0;
}

char * Packet::GetRawData()
{
    // Return a new copy of the packet data.
    char * lReturnData = (char *)malloc(this->GetSize());
    char * lTemp = lReturnData;

    memcpy(lTemp, &mID, sizeof(uint32_t));
    lTemp += sizeof(uint32_t);
    memcpy(lTemp, &mCheckSum, sizeof(uint16_t));
    lTemp += sizeof(uint16_t);
    memcpy(lTemp, &mFlags, sizeof(uint16_t));
    lTemp += sizeof(uint16_t);
    memcpy(lTemp, mData, mNumDataBytes);

    return lReturnData;
}

size_t Packet::GetSize()
{
    return mNumDataBytes + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t);
}

void Packet::clear()
{
    if(mData != NULL)
    {
        // std::cout << "Freeing data: " << std::endl;
        //free(mData);
        mData = NULL;
        // printf("Data: %p\n", mData);
    }
    mNumDataBytes = 0;
    mID = 0;
    mCheckSum = 0;
    mFlags = 0;
}

uint16_t Packet::ip_checksum(const void *buf, size_t hdr_len)
{
     unsigned long sum = 0;
     const uint16_t *ip1 = (const uint16_t*)buf;
     while (hdr_len > 1)
     {
             sum += *ip1++;
             if (sum & 0x80000000)
                     sum = (sum & 0xFFFF) + (sum >> 16);
             hdr_len -= 2;
     }
     while (sum >> 16)
             sum = (sum & 0xFFFF) + (sum >> 16);
     return(~sum);
}
