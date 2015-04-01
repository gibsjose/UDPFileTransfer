#include "ServerWindow.h"

ServerWindow::ServerWindow(size_t size) {
    packets.resize(size);
    mStart = -1;
    mEnd = 0;
}

size_t ServerWindow::GetSize(void) {
    return packets.size();
}

size_t ServerWindow::GetPacketCount(void)
{
    size_t lTally = 0;
    for(size_t i = 0; i < packets.size(); i++)
    {
        if(!packets[i].isEmpty())
        {
            lTally++;
        }
    }
    return lTally;
}

bool ServerWindow::IsFull(void) {
    return (mStart == mEnd) && !packets[mStart].isEmpty();
}

bool ServerWindow::IsEmpty(void) {
    if( (mStart == -1 && mEnd == 0) ||  //initial case where nothing was ever added
        (mStart == mEnd && packets[mStart].isEmpty()) //start and end are at same spot and the packet is empty
        )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void ServerWindow::Clear(void) {
    // Actually just go clear each packet.
    for(size_t i = 0; i < packets.size(); i++)
    {
        packets[i].clear();
    }
}

void ServerWindow::AckPacketWithID(uint16_t aID)
{
    bool lFound = false;
    for(size_t i = 0; i < packets.size(); i++)
    {
        if(packets[i].GetID() == aID)
        {
            //packets[i].setIsAcked();
            packets[i].clear();
            lFound = true;

            // If the packet that was ACKed was at the start of the window,
            // advance the start index.
            while(packets[mStart].isEmpty() && mStart != mEnd){
                mStart = (mStart + 1) % packets.size();
                std::cout << "Moved start to " << mStart << std::endl;;
            }

            break;
        }
    }
    if(!lFound)
    {
        throw new GeneralException("Could not set the packet as acked since it is not in the window. ID: "
                                    + std::to_string(aID));
    }
}

void ServerWindow::Push(const Packet & packet) {
    if(this->IsFull())
    {
        throw new GeneralException("Cannot push: the window is full.");
    }
    else
    {
        if(mStart == -1) mStart = 0;    // this is the first push

        packets[mEnd] = packet;
        mEnd = (mEnd + 1) % packets.size(); // Loop back around
    }
}

Packet ServerWindow::Pop(void) {
    if(this->IsEmpty())
    {
        throw new GeneralException("Cannot pop: the window is empty.");
    }
    else
    {
        // Remove and return the packet at the start.
        Packet packet = packets[mStart];

        //Clear the window's copy of the packet being returned so another can replace it.
        packets[mStart].clear();

        // Move the start forward, wrapping around, until it points to a non-empty packet or is at the end.
        do
        {
            mStart = (mStart + 1) % packets.size();
            std::cout << "Moved start to " << mStart << std::endl;;
        } while(packets[mStart].isEmpty() && mStart != mEnd);

        return packet;
    }
}
