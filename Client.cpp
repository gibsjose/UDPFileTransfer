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
            bool foundLastPacket = false;

            //Declare window of size 5 for the client
            ClientWindow window(5);
            Packet packet;

            //Loop until we get the last packet
            while(!finished) {

                //Receive a vector of up to 5 packets from the server
                std::vector<Packet> packets = GetPacketsFromServer();

                for(size_t i = 0; i < packets.size(); i++) {

                    //Do not push packets that have already been acked: This means the ACK to the
                    // server likely failed, so it is sending it again. ACK and move on.
                    // Don't forget periods.
                    if(window.IsOldPacketID(packets.at(i).GetID()))
                    {
                        std::cout << "Received packet " << packets.at(i).GetID() << " multiple times: ACKing and moving on" << std::endl;
                        SendAckToServer(packets.at(i).GetID());
                        continue;
                    }

                    //Compute checksum on packet before we push it and send ACK
		   			uint16_t lChecksum = packets.at(i).GetChecksum();
					uint16_t lRecomputedChecksum = packets.at(i).RecomputeChecksum();
					//std::cout << "Checksum: " << lChecksum << std::endl;
					//std::cout << "Recomputed Checksum: " << lRecomputedChecksum << std::endl;
                    if(lChecksum == lRecomputedChecksum) {
                        //Push the packet to the window
                        if(window.Push(packets.at(i))) {
                            std::cout << "\tPushed packet to window; ID=" << packets.at(i).GetID() << std::endl;
                            //Send an ACK to the server
                            SendAckToServer(packets.at(i).GetID());
                        }

						std::cout << "\t\t# Of packets in window: " << window.GetNumberOfPackets() << std::endl;

                        if(packets.at(i).isLastPacket()) {
							std::cout << "Found last packet." << std::endl;
                            foundLastPacket = true;
                        }

                        //Try to pop and write as many packets as we can
                        while(!(packet = window.Pop()).isEmpty()) {
                            //Write the file to the packet
                            std::cout << "\t\t\tWrote packet with ID=" << packet.GetID() << std::endl;
                            file.write(packet.GetData(), packet.GetDataSize());
                        }
                    }
                    else
                    {
                        std::cout << "Failed checksum: dropping packet with ID=" << packets.at(i).GetID() << std::endl;
                    }
                }

                //Check if it is the last packet
                if(foundLastPacket && window.IsEmpty()) {
					std::cout << "Finished." << std::endl;
                    finished = true;
                    break;
                }
            }

            file.close();
        }
    }
}

void Client::CreateServerSocket(void) {
    //Create the socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

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
    char buffer[ACK_DATA_SIZE];
    strncpy(buffer, ACK_DATA, ACK_DATA_SIZE);

    //Create a packet and ACK it
    Packet ack = Packet(buffer, ACK_DATA_SIZE, id, IS_ACK_PACKET);

    //Send the filepath request to the server
    int n = sendto(sock, ack.GetRawData(), ack.GetSize(), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr));

    std::cout << "Sent ACK to server for packet: " << id << std::endl;
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
            //Server has shut down
            std::cout << "Server has shut down..." << std::endl;
            return packets;
        }
        else {
            // std::cout << "Creating packet..." << std::endl;
            Packet packet(buffer, n);
            packets.push_back(packet);
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
