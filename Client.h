#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <fstream>
#include <iostream>

#include "Globals.h"
#include "StringUtilities.h"
#include "Packet.h"
#include "ClientWindow.h"

class Client {
public:
    Client(uint16_t port, const std::string & address) {
        this->port = port;
        this->address = address;

        filepath.clear();
    }

    void Run(void);
    char* substr(char* arr, int begin, int len);

private:
    void CreateServerSocket(void);
    void RequestFileFromServer(void);
    std::vector<Packet> GetPacketsFromServer(void);
    void SendAckToServer(uint32_t id);

    int sock;
    struct sockaddr_in serverAddress;
    uint16_t port;
    std::string address;

    std::string filepath;
    std::string destination;
};

#endif//CLIENT_H
