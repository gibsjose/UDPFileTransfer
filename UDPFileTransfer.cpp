#include "UDPFileTransfer.h"

int main(int argc, char * argv[]) {
    ConfigManager configManager;
    configManager.ParseArgs(argc, argv);
    configManager.PrintArgs();

    //Run either the Server or Client process
    if(configManager.GetRunType() == RunServer) {
        Server server(configManager.GetPort());
        server.Run();
    } else {
        Client client(configManager.GetPort(), configManager.GetAddress());
        client.Run();
    }
}
