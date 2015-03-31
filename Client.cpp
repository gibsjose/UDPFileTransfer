#include "Client.h"

void Client::Run(void) {
    CreateServerSocket();
    RequestFileFromServer();

    FILE * file = fopen(destination.c_str(), "ab");
    if(file == NULL) {
        std::cerr << "Error writing to destination file: " << strerror(errno) << std::endl;
        return;
    }
    Packet empty = Packet();
    ClientWindow window = ClientWindow(5);

    while(true) {
      std::vector<Packet> packets = ReceiveFileFromServer();

      Packet p;
      //TODO verify checksum
      while(!packets.empty()) {
        p = packets.back();
        packets.pop_back();

        SendAckToServer(p.GetID());
        if(!p.isEmpty()) {
          window.Push(p);
        }
        Packet pakPop;
        while(!(pakPop = window.Pop()).isEmpty()) {
          int bytesWritten = fwrite(pakPop.GetMData(), 1, strlen(pakPop.GetMData()), file);
          if(bytesWritten != pakPop.GetSize()) {
              std::cerr << "Error writing to destination file: " << strerror(errno) << std::endl;
          }
          std::cout << "Successfully wrote file to \"" << destination << "\"" << std::endl;
        }
      }
    }
}

void Client::CreateServerSocket(void) {
    //Create the socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    //Create timeval struct
    struct timeval to;
    to.tv_sec = 10;
    to.tv_usec = 0;

    //Make socket only wait for 10 seconds w/ setsockopt
    //  socket descriptor
    //  socket level (internet sockets, local sockets, etc.)
    //  option we want (SO_RCVTIMEO = Receive timeout)
    //  timeout structure
    //  size of structure
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));

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
    //Clear filepath
    filepath.clear();

    char buffer[1024];
    Packet ack = Packet();
    ack.setAckPacket();
    ack.setId(id);

    //Send the filepath request to the server
    int n = sendto(sock, ack.GetRawData(), strlen(ack.GetRawData()) + 1, 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr));

    std::cout << "Sent ack to server" << std::endl;
}

std::vector<Packet> Client::ReceiveFileFromServer(void) {
    int bufferSpot = 1024;
    //5MB File buffer
    char buffer[1024 * 5];

    unsigned int len = sizeof(struct sockaddr);
    int n = recvfrom(sock, buffer, (1024 * 1024), 0, (struct sockaddr *)&serverAddress, &len);

    if(n < 0) {
        std::cerr << "Error receiving file from server: " << strerror(errno) << std::endl;
    }
    std::vector<Packet> packets;
    if(n == 1) {
        if((unsigned char)buffer[0] == 0xEE) {
            std::cerr << "File \"" << filepath << "\" does not exist" << std::endl;
            return packets;
        }
    }

    Packet p;
    //Need to fix so it add p1, p2, p3, p4, p5
    if(strlen(buffer) >= bufferSpot) {
      p = Packet(substr(buffer, bufferSpot - 1024, 1024), 1024);
      bufferSpot += 1024;
      packets.push_back(p);
    }

    std::cout << "Received " << n << " bytes from server" << std::endl;
    // std::cout << "File contents:" << std::endl;
    // std::cout << buffer << std::endl;

    return packets;
}

char* Client::substr(char* arr, int begin, int len)
{
    char* res = new char[len];
    for (int i = 0; i < len; i++)
        res[i] = *(arr + begin + i);
    res[len] = 0;
    return res;
}
