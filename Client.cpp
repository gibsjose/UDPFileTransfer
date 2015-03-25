#include "Client.h"


int main(int argc, char **argv){
  int sockfd = socket(AF_INET,SOCK_DGRAM,0);

  if(sockfd < 0) {
    std::cout << "error creating socket" << std::endl;
  }

  struct sockaddr_in serveraddr;
  serveraddr.sin_family = AF_INET;
  //Add in port
  serveraddr.sin_port = htons();
  //Add in IPv4 address
  serveraddr.sin_addr.s_addr = inet_addr("");


}
