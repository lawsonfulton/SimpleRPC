#include "OutgoingSocketManager.h"
#include "MyExceptions.h"

#include <stdlib.h>
#include <sstream>
#include <unistd.h>
#include <string.h>

int OutgoingSocketManager::connectTo(const char* address, const char* port)
{
    //Set up socket information
    struct addrinfo hints, *servinfo;
    int sockfd;

    //Set up socket information
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(address, port, &hints, &servinfo) != 0)
        throw UNKNOWN_HOST_ERROR;

    //Create the socket for connecting to the server
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(sockfd == -1)
    throw SOCKET_ERROR;

    //Attempt to establish a connection with the server
    if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        throw CONNECTION_ERROR;
    }

    return sockfd;
}

int OutgoingSocketManager::connectTo(const char* address, int port)
{
    std::stringstream ss;
    ss << port;
    return connectTo(address, ss.str().c_str());
}











