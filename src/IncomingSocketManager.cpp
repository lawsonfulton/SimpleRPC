#include "IncomingSocketManager.h"
#include "MyExceptions.h"

#include <unistd.h>
#include <string.h>


#define BACKLOG 10

char* IncomingSocketManager::getAddress()
{
   char *hostname = (char*)malloc(1024);
   hostname[1023] = '\0';
   gethostname(hostname, 1023);
   return hostname;
}

int IncomingSocketManager::getPort(int listeningSocket)
{
   sockaddr_in my_addr;
   socklen_t channellen = sizeof(my_addr);
   getsockname(listeningSocket, (sockaddr*) &my_addr, &channellen);
   unsigned short portno = ntohs(my_addr.sin_port);
   return portno;
}

void IncomingSocketManager::printSocketInfo(int listeningSocket)
{
   std::cout << "BINDER_ADDRESS " << getAddress() << std::endl;
   std::cout << "BINDER_PORT " << getPort(listeningSocket) << std::endl;
}

int IncomingSocketManager::acceptConnection(int listeningSocket)
{
   sockaddr_in incoming_addr;
   socklen_t addr_size = sizeof(incoming_addr);
   int incoming_socketfd = accept(listeningSocket, (struct sockaddr *)&incoming_addr, &addr_size);
   if(incoming_socketfd == -1)
   {
      throw CONNECTION_ERROR;
   }

   return incoming_socketfd;  
}

int IncomingSocketManager::createWelcomeSocket()
{
   struct sockaddr_in my_addr;
   int listen_socketfd;

   //Create a socket to listen for incoming connections
   listen_socketfd = socket(AF_INET, SOCK_STREAM, 0);
   if(listen_socketfd == -1)
      throw SOCKET_ERROR;

   //Bind the listening socket to a free port
   memset(&my_addr, 0, sizeof(my_addr));
   my_addr.sin_family = AF_INET;
   my_addr.sin_addr.s_addr = INADDR_ANY;
   if(bind(listen_socketfd, (sockaddr *) &my_addr, sizeof(my_addr)))
      throw CONNECTION_ERROR;

   //Listen for incoming connections
   if(listen(listen_socketfd, BACKLOG) == -1)
      throw CONNECTION_ERROR;

   return listen_socketfd;
}

