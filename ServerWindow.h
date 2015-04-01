#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <iostream>
#include <vector>

#include "Packet.h"
#include "Exception.h"

class ServerWindow {
public:
    ServerWindow(size_t size);
    size_t GetSize(void);
    size_t GetPacketCount(void);
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
};

#endif//SERVERWINDOW_H
