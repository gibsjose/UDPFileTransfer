#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <iostream>

#include "StringUtilities.h"

class Client {
public:
    Client(uint16_t port, const std::string & address) {
        this->port = port;
        this->address = address;

        filepath.clear();
    }

    void Run(void);

private:
    void CreateServerSocket(void);
    void RequestFileFromServer(void);
    void ReceiveFileFromServer(void);

    int sock;
    struct sockaddr_in serverAddress;
    uint16_t port;
    std::string address;

    std::string filepath;
};

#endif//CLIENT_H
