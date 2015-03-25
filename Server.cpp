int main(int argc, char **argv){
  int sockfd = socket(AF_INET,SOCK_DGRAM,0);

  if(sockfd < 0) {
    std::cout << "error creating socket" << std::endl;
  }

  struct sockaddr_in serveraddr, clientaddr;
  serveraddr.sin_family = AF_INET;
  
  //need to add in port that is specified
  serveraddr.sin_port = htons();
  serveraddr.sin_addr.s_addr=INADDR_ANY;

  bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));




}
