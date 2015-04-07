#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <iostream>
#include <vector>
#include <sys/time.h>

#include "Packet.h"
#include "Exception.h"

class ServerWindow {
public:
    ServerWindow(size_t size);
    size_t GetSize(void);
    size_t GetPacketCount(void);
    std::vector<Packet *> GetTimedOutPackets();
    bool IsFull(void);
    bool IsEmpty(void);
    bool IsEmpty(uint32_t);
    void Clear(void);
    void AckPacketWithID(uint16_t aID);
    void Push(const Packet & value);
    Packet Pop(void);

private:
    std::vector<Packet> packets;    //Packets, yay...
    int mStart, mEnd;

    /* Subtract the ‘struct timeval’ values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */

   int timeval_subtract (struct timeval * result, struct timeval * x, struct timeval * y);

};

#endif//SERVERWINDOW_H
