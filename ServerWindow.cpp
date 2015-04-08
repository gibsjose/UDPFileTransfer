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

std::vector<Packet *> ServerWindow::GetTimedOutPackets()
{
    // 100ms (hardcoded timeout if not acked)
    struct timeval lTimeoutTime;
    lTimeoutTime.tv_sec = 0;
    lTimeoutTime.tv_usec = 50000;  // 50ms

    // Record now.
    struct timeval lNowTime;
    if(0 != gettimeofday(&lNowTime, NULL))
    {
        std::cerr << "Error: gettimeofday(): " << strerror(errno) << "\n";
        exit(-1);
    }

    struct timeval lResult;

    if(0 != timeval_subtract(&lResult, &lNowTime, &lTimeoutTime))
    {
        std::cerr << "Error: timeval_subtract(): " << strerror(errno) << "\n";
        exit(-1);
    }

    // Do some math with timevals to figure out if the time sent for each packet is too long ago.

    std::vector<Packet *> lPackets;
    for(size_t i = 0; i < packets.size(); i++)
    {
        struct timeval lDummy;
        struct timeval lTimeSent = packets[i].GetTimeSent();
        if(!packets[i].isEmpty() && !packets[i].isAcked() && (timeval_subtract(&lDummy, &lTimeSent, &lResult) == 1))
        {
            // -1 is returned when the result is negative (i.e. when the time the
            // packet was sent was less than the timeval specifying the oldest permissible time).
            lPackets.push_back(&(packets[i]));
        }
    }

    return lPackets;
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
    // std::cout << "AckPacketWithID(): start: " << this->mStart << " end: " << this->mEnd << std::endl;

    bool lFound = false;
    for(size_t i = 0; i < packets.size(); i++)
    {
        if(packets[i].GetID() == aID)
        {
            // Clear (ack) the packet
            packets[i].clear();
            lFound = true;

            // If the ACKed packet is the start packet, advance the start index
            // by 1.
            if(i == mStart)
            {
                mStart = (mStart + 1) % packets.size();
            }

            // Advance the start index until it points at a non-empty packet.
            while(packets[mStart].isEmpty() && mStart != mEnd){
                mStart = (mStart + 1) % packets.size();
                // std::cout << "Moved start to " << mStart << std::endl;;
            }

            break;
        }
    }
    if(!lFound)
    {
		this->Print();
        std::cout << ("Could not set the packet as acked since it is not in the window. ID: "
                                    + std::to_string(aID)) << std::endl;
    }
    // std::cout << "AckPacketWithID(): start: " << this->mStart << " end: " << this->mEnd << std::endl;
}

void ServerWindow::Print()
{
	std::cout << "Window size: " << this->GetPacketCount() << std::endl;

	for(size_t i = 0; i < packets.size(); i++)
	{
		std::cout << "Packet ID: " << packets[i].GetID() << std::endl;
	}
}

void ServerWindow::Push(const Packet & packet) {
    if(this->IsFull())
    {
        throw new GeneralException("Cannot push: the window is full.");
    }
    else
    {
        // std::cout << "Push(): start: " << this->mStart << " end: " << this->mEnd << std::endl;

        if(mStart == -1) mStart = 0;    // this is the first push

        packets[mEnd] = packet;
        mEnd = (mEnd + 1) % packets.size(); // Loop back around

        // std::cout << "Push(): start: " << this->mStart << " end: " << this->mEnd << std::endl;
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
            // std::cout << "Moved start to " << mStart << std::endl;;
        } while(packets[mStart].isEmpty() && mStart != mEnd);

        return packet;
    }
}

/* Subtract the ‘struct timeval’ values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */

int
ServerWindow::timeval_subtract (struct timeval * result, struct timeval * x, struct timeval * y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}
