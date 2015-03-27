#include "ConfigManager.h"

void ConfigManager::ParseArgs(int argc, char ** argv) {

    //Display @usage if incorrect usage
    // if((argc - 1) < 1) {
    //     std::cerr << "@usage: UDPFileTransfer [-s, -c] [-p PORT] [-ip ADDRESS]" << std::endl;
    //     exit(-1);
    // }

    //Parse Arguments
    for(int i = 1; argv[i] != NULL; ) {
        char * argFlag = argv[i];

        if(!strcmp("-s", argFlag)) {
            runType = RunServer;
            i++;
        } else if(!strcmp("-c", argFlag)) {
            runType = RunClient;
            i++;
        } else if(!strcmp("--help", argFlag)) {
            std::cout << "@usage: UDPFileTransfer [-s, -c] [-p PORT] [-ip ADDRESS]" << std::endl;
            std::cout << "\t Defaults to 'Server (-s)', 'Port 8888', 'Address 127.0.0.1'" << std::endl;
        } else {
            char * argValue = argv[i + 1];

            if(!strcmp("-p", argFlag)) {
                if(argValue == NULL) {
                    port = (uint16_t)8888;
                } else {
                    port = atoi(argValue);
                }
            } else if(!strcmp("-ip", argFlag)) {
                if(argValue == NULL) {
                    address = "127.0.0.1";
                } else {
                    address = std::string(argValue);
                }
            }

            i+= 2;
        }
    }
}

void ConfigManager::PrintArgs(void) {
    std::cout << std::endl;
    std::cout << "Configuration" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << "Run Type: " << (runType == RunServer ? "Server" : "Client") << std::endl;
    std::cout << "Address: " << address << std::endl;
    std::cout << "Port: " << port << std::endl;
}
