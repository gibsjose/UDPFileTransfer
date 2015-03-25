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

// This is the default port to listen on for incoming DNS requests.
#define DEFAULT_PORT 8888

class ConfigManager
{
public:
    ConfigManager();
    ~ConfigManager();
    void parseArgs(int argc, char * argv[]);
    int getListeningPort() const;

private:
    std::string mResolverIP;
    unsigned mListeningPort;
};
