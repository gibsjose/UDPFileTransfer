int main(int argc, char **argv){
  int sockfd = socket(AF_INET,SOCK_DGRAM,0);

  if(sockfd < 0) {
    std::cout << "error creating socket" << std::endl;
  }

  struct sockaddr_in serveraddr, clientaddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(9876);
  serveraddr.sin_addr.s_addr=INADDR_ANY;

  bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));




}
