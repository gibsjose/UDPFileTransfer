#include "ConfigManager.h"

ConfigManager::ConfigManager()
{
    mListeningPort = -1;
}

ConfigManager::~ConfigManager()
{

}

void ConfigManager::parseArgs(int aArgc, char * aArgv[])
{
    // Attempt to find the port and IP of the DNS resolver on the command line.
    // if(aArgc < 3)
    // {
    //     std::cerr << "Not enough arguments. Ex: DNSResolver -p 8888\n";
    //     exit(-1);
    // }
    for(int i = 1; i + 1 < aArgc; i = i + 2)
    {
      char * lArgFlag = aArgv[i];
      char * lArgValue = aArgv[i + 1];

      if(!strcmp("-p", lArgFlag))
      {
        mListeningPort = atoi(lArgValue);
      }
    }

    // If no port was set for which to listen on for incoming DNS requests, set
    // it to the default port.
    if(mListeningPort == -1)
    {
        mListeningPort = DEFAULT_PORT;
    }
}

int ConfigManager::getListeningPort() const
{
    return mListeningPort;
}
