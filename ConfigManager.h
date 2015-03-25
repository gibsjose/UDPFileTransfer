#pragma once
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

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager();
    void ParseArgs(int argc, char * argv[]);
    unsigned GetPort(void) const;
    std::string & GetAddress(void) const;

private:
    std::string address;
    unsigned port;
};
