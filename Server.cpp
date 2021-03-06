#include "Server.h"


void Server::Run(void) {
    CreateClientSocket();

    while(true) {
        std::string lFilePath = ReceiveRequestFromClient();
        if(!lFilePath.empty())
        {
            SendFileToClient(lFilePath);
        }
    }
}

void Server::CreateClientSocket(void) {
    //Create the socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    //Create timeval struct
    struct timeval to;
    to.tv_sec = 1;
    to.tv_usec = 0;

    //Make socket only wait for 5 seconds w/ setsockopt
    //  socket descriptor
    //  socket level (internet sockets, local sockets, etc.)
    //  option we want (SO_RCVTIMEO = Receive timeout)
    //  timeout structure
    //  size of structure
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));

    //Create server/client address structures:
    //Server needs to know both the address of itself and the client
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //Socket error
    if(sock < 0) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
    }

    //Bind, on the server side, tells all incoming data on the server address to go to the specified socket
    bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
}

std::string Server::ReceiveRequestFromClient(void) {
    char buffer[1024];

    unsigned int len = sizeof(struct sockaddr);

    //Receive request from client
    int n = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&clientAddress, &len);

    if(n < 0) {
        //Ignore 'temporarily unreachable...'
        if(errno != 35 && errno != 11) {
            std::cout << std::endl;
            std::cerr << errno << std::endl;
            std::cerr << "Error receiving request from client: " << strerror(errno) << std::endl;
        }

        return "";
    }

    std::string filepath = std::string(buffer);

    std::cout << std::endl;
    std::cout << "Received " << n << " bytes from client" << std::endl;
    std::cout << "Client requested: " << filepath << std::endl;

    return filepath;
}

void Server::SendFileToClient(std::string aFilePath) {
    ServerWindow window(5); // sliding window

    char buffer[PACKET_SIZE];   // file buffer
    int packID = 0;     // starting packet ID (sequence number)
    int n = 0;
    unsigned int totalBytesRead = 0;
    unsigned int len = sizeof(struct sockaddr);

    if(aFilePath.empty())
    {
        std::cerr << "Error: file path is empty!" << std::endl;
        return;
    }
    else
    {
        //Attempt to open the file pointed to by the file path.
        std::ifstream lFileStream(aFilePath.c_str(), std::ifstream::binary);

        if(!lFileStream) {
            std::cerr << "Error: File could not be served: " << strerror(errno) << std::endl;

            char err = 0xEE;

            //Send message to client indicating file does not exist
            n = sendto(sock, &err, 1, 0, (struct sockaddr *)&clientAddress, sizeof(struct sockaddr));

            std::cout << "File does not exist; notified client" << std::endl;
            return;
        }
        else
        {
            // For now, just read in the file until the window is full, send everything in the window and empty it, and repeat
            // until the file is all sent.  We will implement ACKing soon, and then the ServerWindow class has to be changed
            // along with this code to mark individual packets as ACKed.  These packets that are marked as ACKed will then be
            // considered empty slots in the sliding window that can be filled in with new packets created from new file data.

            // get length of file:
            lFileStream.seekg (0, lFileStream.end);
            int lFileLength = lFileStream.tellg();
            lFileStream.seekg (0, lFileStream.beg);

            bool lFinished = false; // Mark if finished handling the entire file request (read, send, acks).
            bool lFinishedReading = false;  // Mark if finished reading from the file.

            int bytesRead = 0;

            while(!lFinished) {
                if(!lFinishedReading)
                {
                    while(!window.IsFull() && !lFinishedReading)
                    {
                        // Read the next chunk of data in.
                        bytesRead = lFileStream.readsome(buffer, PACKET_DATA_SIZE);
                        totalBytesRead += bytesRead;

                        if(lFileStream.bad()) {
                            std::cerr << "Error reading the file: " << strerror(errno) << std::endl;
                            exit(-1);
                        } else if(lFileStream.eof()) {
                            //Done reading the file
                            std::cout << "Found eof.  This should not happen." << std::endl;
                            break;
                        }
                        else if(bytesRead == 0)
                        {
                            // No more data to read.
                            std::cout << "No more data to read.  This should not happen." << std::endl;
                            break;
                        }

                        // If the current location in the file is at the end, this is the end.  Boom.
                        lFinishedReading = (lFileLength == lFileStream.tellg());

                        //Contruct a packet using the read in data
                        Packet pckt(buffer, bytesRead, packID++, 0);

                        // If this is the end, mark the packet as such.
                        // std::cout << "Finished reading: " << lFinishedReading << std::endl;
                        if(lFinishedReading)
                        {
                            std::cout << "\tMarking last packet with ID=" << pckt.GetID() << std::endl;
                            pckt.setLastPacket();
							pckt.RecomputeChecksum();
                        }

                        // Send the packet
                        std::cout << "Sending packet with ID=" << pckt.GetID() << std::endl;

                        n = sendto(sock, pckt.GetRawData(), pckt.GetSize(), 0,
                                    (struct sockaddr *)&clientAddress, sizeof(struct sockaddr));

                        // Mark the time this packet was sent as now so that later it can be reset if it is not ACKed.
                        struct timeval lTime;
                        if(0 != gettimeofday(&lTime, NULL))
                        {
                            std::cerr << "Error: gettimeofday(): " << strerror(errno) << "\n";
                            exit(-1);
                        }
                        pckt.setTimeSent(lTime);

                        //Push packet onto the server window.
                        window.Push(pckt);
                    }
                }

                // Resend packets that have timed out.
                std::vector<Packet*> lPacketsToResend = window.GetTimedOutPackets();

                //std::cout << "Resending " << lPacketsToResend.size() << " packets" << std::endl;

                for(size_t i = 0; i < lPacketsToResend.size(); i++)
                {
                    // Resend the packet
                    std::cout << "Resending packet with ID=" << lPacketsToResend[i]->GetID() << std::endl;

                    n = sendto(sock,
                                lPacketsToResend[i]->GetRawData(),
                                lPacketsToResend[i]->GetSize(),
                                0,
                                (struct sockaddr *)&clientAddress,
                                sizeof(struct sockaddr));

                    // Update the time this packet was sent.
                    struct timeval lTime;
                    if(0 != gettimeofday(&lTime, NULL))
                    {
                        std::cerr << "Error: gettimeofday(): " << strerror(errno) << "\n";
                        exit(-1);
                    }
                    lPacketsToResend[i]->setTimeSent(lTime);
                }

                // Receive ACKs if they come back from the client and mark the corresponding packets that were
                // sent as ACKed.
                while(true)
                {
                    int n = recvfrom(sock, buffer, PACKET_SIZE, MSG_DONTWAIT, (struct sockaddr *)&serverAddress, &len);
                    if(n <= 0) {
                        if(errno != 11) //resource temporarily unavailable
                        {
                            std::cerr << "Error?: " << strerror(errno) << std::endl;
                        }
                        break;
                    }
                    else
                    {
                        Packet lPacket(buffer, n);
                        if(lPacket.isAckPacket())
                        {
                            // Compare checksums of an ACK packet (everything except the checksum matters)
                            // and the received packet.
                            char buffer[ACK_DATA_SIZE];
                            strncpy(buffer, ACK_DATA, ACK_DATA_SIZE);
                            Packet lTempACK = Packet(buffer, ACK_DATA_SIZE, lPacket.GetID(), IS_ACK_PACKET);
			    			lTempACK.setAckPacket();

                            if(lTempACK.RecomputeChecksum() == lPacket.GetChecksum())
                            {
                                // If the checksum is good, accept the ACK.
                                std::cout << "ACKing ID=" << lPacket.GetID() << std::endl;
                                window.AckPacketWithID(lPacket.GetID());
                            }
                            else
                            {
								std::cout << "Temp ACK packet:" << std::endl;
								lTempACK.Print();
								std::cout << "\nReal ACK packet:" << std::endl;
								lPacket.Print();
                                std::cout << "Rejecting ACK packet (ID=" << lPacket.GetID() << ") due to checksum mismatch on data." << std::endl;
                            }
                        }

                        if(lFinishedReading && window.IsEmpty())
                        {
                            // This must have been the last packet.
                            lFinished = true;
                            break;
                        }
                    }
                }
            }

            //Close the file
            lFileStream.close();

            std::cout << "Read " << totalBytesRead << " bytes from file" << std::endl;
        }
    }
}
