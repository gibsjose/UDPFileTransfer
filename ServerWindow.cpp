#include "ServerWindow.h"

ServerWindow::ServerWindow(size_t size) {
    packets.resize(size);
    packets.clear();
    vip = 0;
}

size_t ServerWindow::GetSize(void) {
    return packets.size();
}

bool ServerWindow::IsEmpty(void) {
    for(int i = 0; i < packets.size(); i++) {
        if(!packets.at(i).isEmpty()) {
            return false;
        }
    }

    return true;
}

bool ServerWindow::IsEmpty(uint32_t index) {
    return packets.at(index).isEmpty();
}

void ServerWindow::Clear(void) {
    packets.clear();
}

void ServerWindow::Push(const Packet & packet) {
    uint32_t id = packet.GetID();
    uint32_t index = id % packets.size();

    if(!packets.at(index).isEmpty()) {
        std::cerr << "Error: Overwriting a non-empty packet in window... This should never happen" << std::endl;
    }

    packets[index] = packet;
}

//Returns the VIP each time, only sliding forward if the VIP is not empty
Packet ServerWindow::Pop(void) {
    Packet packet = packets.at(vip);

    //Not empty... Roll out the red carpet (for a few clock cycles)
    if(!packet.isEmpty()) {
        //Clear the VIP
        packets.at(vip).clear();

        //Slide the window forward and check if we walked off the end
        if(++vip >= packets.size()) {
            vip = 0;
        }
    }

    //Return the VIP, empty or not
    return packet;
}
