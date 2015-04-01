#include "Client.h"

void Client::Run(void) {
    CreateServerSocket();

    //Request files from the server
    while(true) {
        RequestFileFromServer();

        Packet empty = Packet();
        ClientWindow window = ClientWindow(5);

        std::ofstream file(this->destination, std::ofstream::binary);

        //Check if file is open
        if(!file.is_open()) {
            std::cerr << "Could not open destination file: " << this->destination << "\n" << std::endl;
            exit(-1);
        }

        //File is open
        else {
            bool finished = false;

            //Declare window of size 5 for the client
            ClientWindow window(5);
            Packet packet;

            //Loop until we get the last packet
            while(!finished) {

                //Recieve a vector of up to 5 packets from the server
                std::vector<Packet> packets = GetPacketsFromServer();

                for(size_t i = 0; i < packets.size(); i++) {

                    //Compute checksum on packet before we push it and send ACK
                    if(packets.at(i).CompareChecksum(packets.at(i).GetChecksum())) {
                        //Push the packet to the window
                        window.Push(packets.at(i));

                        //Send an ACK to the server
                        SendAckToServer(packets.at(i).GetID());

                        //Try to pop and write as many packets as we can
                        while(!(packet = window.Pop()).isEmpty()) {
                            //Write the file to the packet
                            file.write(packet.GetData(), packet.GetDataSize());

                            //Check if it is the last packet
                            if(packet.isLastPacket()) {
                                finished = true;
                                break;
                            }
                        }
                    }
                }
            }

            file.close();
        }
    }
}

void Client::CreateServerSocket(void) {
    //Create the socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    //Create timeval struct
    struct timeval to;
    to.tv_sec = 1;
    to.tv_usec = 0;

    //Make socket only wait for 10 seconds w/ setsockopt
    //  socket descriptor
    //  socket level (internet sockets, local sockets, etc.)
    //  option we want (SO_RCVTIMEO = Receive timeout)
    //  timeout structure
    //  size of structure
    //setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));

    //Socket error
    if(sock < 0) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
    }

    //Assign the port and address of the server
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = inet_addr(address.c_str());
}

void Client::RequestFileFromServer(void) {
    //Clear filepath
    filepath.clear();

    //Get filepath
    std::cout << std::endl;
    std::cout << "Enter the path of the remote file: ";
    std::cin >> filepath;

    std::cout << "Enter the destination path: ";
    std::cin >> destination;

    //Remove newlines and leading/trailing whitespace
    StringUtilities::Trim(filepath);
    StringUtilities::Trim(destination);

    char buffer[1024];
    strncpy(buffer, filepath.c_str(), 1024);

    //Send the filepath request to the server
    int n = sendto(sock, buffer, strlen(buffer) + 1, 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr));

    if(n < 0) {
        std::cerr << "Error sending filepath request to server: " << strerror(errno) << std::endl;
    }

    std::cout << "Sent " << n << " bytes to server" << std::endl;
    std::cout << "Requested: " << buffer << std::endl;
}

void Client::SendAckToServer(uint32_t id) {
    //Create a blank packet and set the ID and set the ACK Flag
    Packet ack = Packet();
    ack.setAckPacket();
    ack.setId(id);

    //Send the filepath request to the server
    int n = sendto(sock, ack.GetRawData(), PACKET_SIZE, 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr));

    std::cout << "Sent ack to server for packet: " << id << std::endl;
}

std::vector<Packet> Client::GetPacketsFromServer(void) {
    std::vector<Packet> packets;

    char buffer[PACKET_SIZE];
    unsigned int len = sizeof(struct sockaddr);

    while(true) {
        //Non-blocking receive
        int n = recvfrom(sock, buffer, PACKET_SIZE, MSG_DONTWAIT, (struct sockaddr *)&serverAddress, &len);

        if(n < 0) {
            //std::cerr << "Error receiving file from server: " << strerror(errno) << std::endl;
            return packets;
        }
        else if(n == 1) {
            if((unsigned char)buffer[0] == 0xEE) {
                std::cerr << "File \"" << filepath << "\" does not exist" << std::endl;
                return packets;
            }
        }
        else if(n == 0) {
            //No more data, so return the packets
            std::cout << "No more packets..." << std::endl;
            return packets;
        }
        else
        {
            std::cout << "Creating packet..." << std::endl;
            Packet lPacket(buffer, n);
            packets.push_back(lPacket);
        }
    }
}

char * Client::substr(char* arr, int begin, int len)
{
    char* res = new char[len];
    for (int i = 0; i < len; i++)
    res[i] = *(arr + begin + i);
    res[len] = 0;
    return res;
}
