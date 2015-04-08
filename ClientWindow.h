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
    bool IsEmpty(void);
    bool IsEmpty(uint32_t);
    void Clear(void);
    bool Push(const Packet & value);
    Packet Pop(void);

private:
    std::vector<Packet> packets;    //Packets, yay...
    uint32_t vip;                   //Very Important Packet
};

#endif//CLIENTWINDOW_H
