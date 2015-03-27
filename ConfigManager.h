#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <vector>

#include "Exception.h"
#include "StringUtilities.h"

// This is the default port for UDP File Transfer
#define DEFAULT_PORT 8888

typedef enum RunType {
    RunServer,
    RunClient
} RunType;

class ConfigManager {
public:
    ConfigManager() : runType(RunClient), address("127.0.0.1"), port((uint16_t)8888) {}
    RunType GetRunType(void) const { return runType; }
    uint16_t GetPort(void) const { return port; }
    const std::string & GetAddress(void) const { return address; }

    void ParseArgs(int, char **);
    void PrintArgs(void);

private:
    RunType runType;
    std::string address;
    uint16_t port;
};

#endif//CONFIGMANAGER_H
