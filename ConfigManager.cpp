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
            runType = Server;
            i++;
        } else if(!strcmp("-c", argFlag)) {
            runType = Client;
            i++;
        } else if(!strcmp("--help", argFlag) {
            std::cout << "@usage: UDPFileTransfer [-s, -c] [-p PORT] [-ip ADDRESS]" << std::endl;
            std::cout << "\t Defaults to 'Server (-s)', 'Port 8888', 'Address 127.0.0.1'" << std::endl;
        } else {
            char * argValue = argv[i + 1];

            if(!strcmp("-p", argFlag)) {
                port = atoi(argValue);
            } else if(!strcmp("-ip", argFlag)) {
                address = std::string(argValue);
            }

            i+= 2;
        }
    }
}

void ConfigManager::PrintArgs(void) {
    std::cout << "Configuration" << std::endl;
    std::cout << "----------------------------------" << std::endl;
    std::cout << "Run Type: " << (runType == Server ? "Server" : "Client") << std::endl;
    std::cout << "Address: " << address << std::endl;
    std::cout << "Port: " << port << std::endl;
}
