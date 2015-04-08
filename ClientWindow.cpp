#include "ClientWindow.h"

ClientWindow::ClientWindow(size_t size) {
    packets.resize(size);
    vip = 0;
	lowerBound = 0;
	upperBound = size - 1;
}

bool ClientWindow::IsOldPacketID(uint32_t id)
{
	return id < lowerBound;
}

size_t ClientWindow::GetSize(void) {
    return packets.size();
}

size_t ClientWindow::GetNumberOfPackets(void) {
    size_t count = 0;

    for(int i = 0; i < packets.size(); i++) {
        if(!packets.at(i).isEmpty()) {
            count++;
        }
    }

    return count;
}

bool ClientWindow::IsEmpty(void) {
    for(int i = 0; i < packets.size(); i++) {
        if(!packets.at(i).isEmpty()) {
            return false;
        }
    }

    return true;
}

bool ClientWindow::IsEmpty(uint32_t index) {
    return packets.at(index).isEmpty();
}

void ClientWindow::Clear(void) {
    packets.clear();
}

bool ClientWindow::Push(const Packet & packet) {
    uint32_t id = packet.GetID();
	if(id >= lowerBound || id <= upperBound)
	{
		uint32_t index = id % packets.size();

		if(!packets.at(index).isEmpty()) {
		    std::cerr << "Failed to overwrite packet with id " << packets.at(index).GetID() << " at index " << index << " in window with packet with id " << id << std::endl;

		    return false;
		}

		//Overwrite empty packet
		else {
		    packets[index] = packet;
		    return true;
		}
	}
	else
	{
		// The ID was not in the window, so do nothing with it.
		return false;
	}
}

//Returns the VIP each time, only sliding forward if the VIP is not empty
Packet ClientWindow::Pop(void) {
    Packet packet = packets.at(vip);

    //Not empty... Roll out the red carpet (for a few clock cycles)
    if(!packet.isEmpty()) {
        //Clear the VIP
        packets.at(vip).clear();

        //Slide the window forward and check if we walked off the end
        if(++vip >= packets.size()) {
            vip = 0;
        }

		lowerBound++;
		upperBound++;
    }

    //Return the VIP, empty or not
    return packet;
}
