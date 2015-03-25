#include "UDPFileTransfer.h"

int main(int argc, char * argv[]) {
    ConfigManager configManager;
    configManager.ParseArgs(argc, argv);
    configManager.PrintArgs();
}
