#include "ServerWindow.h"

ServerWindow::ServerWindow(size_t size) {
    packets.resize(size);

    //packets.clear();
    vip = 0;
}

size_t ServerWindow::GetSize(void) {
    return packets.size();
}

bool ServerWindow::IsFull(void) {
    for(int i = 0; i < packets.size(); i++) {
        if(packets.at(i).isEmpty()) {
            //std::cout << "Packet " << i << " is empty" << std::endl;
            return false;
        }
    }
//    std::cout << "SERVER WIN---> window size: " << packets.size() << std::endl;
//    std::cout << "SERVER WIN---> window capacity " << packets.capacity() << std::endl;
    // if(packets.size() >= packets.capacity()) {
    //     return true;
    // }

    return true;
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
    std::cout << "packet id: " << id << std::endl;

    uint32_t index = id % packets.size();
    std::cout << "packet index: " << index << std::endl;


    if(!packets[index].isEmpty()) {
        std::cerr << "Error: Overwriting a non-empty packet in window... This should never happen" << std::endl;
    }
    packets[index] = packet;
    std::cout << "Packets size: " << packets.size() << std::endl;
}

Packet ServerWindow::Peek(void) {
    Packet packet = packets.at(vip);
    return packet;
}

//Returns the VIP each time, only sliding forward if the VIP is not empty
Packet ServerWindow::Pop(void) {
    Packet packet = packets.at(vip);

    //Not empty... Roll out the red carpet (for a few clock cycles)
    std::cout << "Pop call" << std::endl;
    if(!packet.isEmpty()) {
        //Clear the VIP
        packets.at(vip).clear();
        std::cout << "Cleared packet at index: " << vip << std::endl;

        //Slide the window forward and check if we walked off the end
        if(++vip >= packets.size()) {
            vip = 0;
        }
    }

    //Return the VIP, empty or not
    return packet;
}
