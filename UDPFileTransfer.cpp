#include "UDPFileTransfer.h"

int main(int argc, char * argv[]) {
    ConfigManager configManager;
    configManager.ParseArgs(argc, argv);
    configManager.PrintArgs();

    if(configManager.GetRunType() == RunServer) {
        Server server(configManager.GetPort());
        server.Run();
    } else {
        Client client(configManager.GetPort(), configManager.GetAddress());
        client.Run();
    }
}
