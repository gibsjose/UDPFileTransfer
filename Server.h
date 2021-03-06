#ifndef SERVER_H
#define SERVER_H

#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include <iostream>

#include "Globals.h"
#include "StringUtilities.h"
#include "ServerWindow.h"
#include "Packet.h"

class Server {
public:
    Server(uint16_t port) {
        this->port = port;

        filepath.clear();
    }

    void Run(void);

private:
    void CreateClientSocket(void);
    std::string ReceiveRequestFromClient(void);
    void SendFileToClient(std::string aFilePath);

    int sock;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    uint16_t port;

    std::string filepath;
};

#endif//SERVER_H
