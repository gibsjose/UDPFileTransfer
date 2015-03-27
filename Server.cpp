#include "Server.h"

void Server::Run(void) {
    CreateClientSocket();

    while(true) {
        ReceiveRequestFromClient();
        SendFileToClient();
    }
}

void Server::CreateClientSocket(void) {
    //Create the socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

    //Create timeval struct
    struct timeval to;
    to.tv_sec = 10;
    to.tv_usec = 0;

    //Make socket only wait for 5 seconds w/ setsockopt
    //  socket descriptor
    //  socket level (internet sockets, local sockets, etc.)
    //  option we want (SO_RCVTIMEO = Receive timeout)
    //  timeout structure
    //  size of structure
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to));

    //Create server/client address structures:
    //Server needs to know both the address of itself and the client
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    //Socket error
    if(sock < 0) {
        std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
    }

    //Bind, on the server side, tells all incoming data on the server address to go to the specified socket
    bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
}

void Server::ReceiveRequestFromClient(void) {
    char buffer[1024];

    unsigned int len = sizeof(struct sockaddr);

    //Clear filepath
    filepath.clear();

    //Receive request from client
    int n = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&clientAddress, &len);

    if(n < 0) {
        //Ignore 'temporarily unreachable...'
        if(errno != 35) {
            std::cerr << "Error receiving request from client: " << strerror(errno) << std::endl;
        }

        return;
    }

    filepath = std::string(buffer);

    std::cout << "Received " << n << " bytes from client" << std::endl;
    std::cout << "Client requested: " << filepath << std::endl;
}

void Server::SendFileToClient(void) {
    //1MB File buffer
    char buffer[1024 * 1024];
    int n = 0;
    unsigned int totalBytesRead;

    if(filepath.empty()) {
        return;
    }

    //Attempt to open the file pointed to by the file path.
    FILE * file = fopen(filepath.c_str(), "rb");

    if(file == NULL) {
        std::cerr << "Error: File could not be served: " << strerror(errno) << std::endl;

        char err = 0xEE;

        //Send message to client indicating file does not exist
        n = sendto(sock, &err, 1, 0, (struct sockaddr *)&clientAddress, sizeof(struct sockaddr));

        std::cout << "File does not exist; notified client" << std::endl;
        return;
    }

    //Read the data from the file into the buffer.
    while(!feof(file)) {
        int bytesRead = fread(buffer, 1, (1024 * 1024), file);
        totalBytesRead += bytesRead;

        if(bytesRead == 0) {
            if(ferror(file)) {
                std::cerr << "Error reading the file: " << strerror(errno) << std::endl;
                return;
            } else {
                //Done reading the file
                break;
            }
        }
    }

    //Close the file
    fclose(file);

    std::cout << "Read " << totalBytesRead << " bytes from file" << std::endl;

    //Send file
    n = sendto(sock, buffer, totalBytesRead, 0, (struct sockaddr *)&clientAddress, sizeof(struct sockaddr));

    if(n < 0) {
        std::cerr << "Error sending file to client: " << strerror(errno) << std::endl;
        return;
    }

    std::cout << "Sent " << n << " bytes to client" << std::endl;
}
