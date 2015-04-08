#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <iostream>
#include <vector>

#include "Packet.h"
#include "Exception.h"

class ClientWindow {
public:
    ClientWindow(size_t size);
    size_t GetSize(void);
    size_t GetNumberOfPackets(void);
    bool IsEmpty(void);
    bool IsEmpty(uint32_t);
    void Clear(void);
    bool Push(const Packet & value);
    Packet Pop(void);
	bool IsOldPacketID(uint32_t id);

private:
    std::vector<Packet> packets;    //Packets, yay...
    uint32_t vip;                   //Very Important Packet
	size_t lowerBound, upperBound;
};

#endif//CLIENTWINDOW_H
