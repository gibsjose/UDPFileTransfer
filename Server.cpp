#include "Server.h"


void Server::Run(void) {
    CreateClientSocket();

    while(true) {
        ReceiveRequestFromClient();
        SendFileToClient();
    }
}

void Server::CreateClientSocket(void) {
    //Create the socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    //Create timeval struct
    struct timeval to;
    to.tv_sec = 0;
    to.tv_usec = 100;

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

void Server::ReceiveRequestFromClient(void) {
    char buffer[1024];

    unsigned int len = sizeof(struct sockaddr);

    //Clear filepath
    filepath.clear();

    //Receive request from client
    int n = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&clientAddress, &len);

    if(n < 0) {
        //Ignore 'temporarily unreachable...'
        if(errno != 35) {
            std::cout << std::endl;
            std::cerr << errno << std::endl;
            std::cerr << "Error receiving request from client: " << strerror(errno) << std::endl;
        }

        return;
    }

    filepath = std::string(buffer);

    std::cout << std::endl;
    std::cout << "Received " << n << " bytes from client" << std::endl;
    std::cout << "Client requested: " << filepath << std::endl;
}

void Server::SendFileToClient(void) {
    ServerWindow window(5); // sliding window

    char buffer[PACKET_SIZE];   // file buffer
    int packID = 0;     // starting packet ID (sequence number)
    int n = 0;
    unsigned int totalBytesRead = 0;
    unsigned int len = sizeof(struct sockaddr);

    if(filepath.empty())
    {
        std::cerr << "Error: file path is empty!" << std::endl;
        return;
    }
    else
    {
        //Attempt to open the file pointed to by the file path.
        // FILE * file = fopen(filepath.c_str(), "rb");
        std::ifstream lFileStream(filepath.c_str(), std::ifstream::binary);

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

            bool lFinished = false; // Mark if finished reading from the file.

            while(!lFinished) {
                while(!window.IsFull()) {    //Read until window full
                    //Read 960 bytes at a time
                    int bytesRead = lFileStream.readsome(buffer, PACKET_DATA_SIZE);
                    totalBytesRead += bytesRead;

                    if(lFileStream.bad()) {
                        std::cerr << "Error reading the file: " << strerror(errno) << std::endl;
                        return;
                    } else if(lFileStream.eof()) {
                        //Done reading the file
                        std::cout << "Done reading the file." << std::endl;
                        lFinished = true;
                        break;
                    }
                    else if(bytesRead == 0)
                    {
                        // No more data to read.
                        lFinished = true;
                        break;
                    }

                    //Contruct a packet using the read in data
                    Packet pckt((char *)buffer, bytesRead, packID++, 0);

                    // If this is the end, mark the packet as such.
                    if(lFinished)
                    {
                        pckt.setLastPacket();
                    }

                    // Send the packet
                    std::cout << "Sending packet with ID=" << pckt.GetID() << std::endl;

                    n = sendto(sock, pckt.GetRawData(), pckt.GetSize(), 0,
                                (struct sockaddr *)&clientAddress, sizeof(struct sockaddr));

                    //Push packet onto the server window.
                    window.Push(pckt);
                }

                // Resend packets that have timed out.


                // Receive ACKs if they come back from the client and mark the corresponding packets that were
                // sent as ACKed.
                while(true)
                {
                    int n = recvfrom(sock, buffer, PACKET_SIZE, 0, (struct sockaddr *)&serverAddress, &len);
                    if(n <= 0) {
                        std::cerr << "Error?: " << strerror(errno) << std::endl;
                        break;
                    }
                    else
                    {
                        Packet lPacket(buffer, n);
                        if(lPacket.isAckPacket())
                        {
                            window.AckPacketWithID(lPacket.GetID());
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
