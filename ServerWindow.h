#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <vector>

#include "Packet.h"
#include "Exception.h"

class ServerWindow {
public:
    ServerWindow(size_t size);
    void GetSize(void);
    bool IsFull(void);
    bool IsEmpty(void);
    bool IsEmpty(uint32_t);
    void Clear(void);
    void Push(const Packet & value);
    Packet Pop(void);

private:
    std::vector<Packet> packets;    //Packets, yay...
    uint32_t vip;                   //Very Important Packet
};

#endif//SERVERWINDOW_H
