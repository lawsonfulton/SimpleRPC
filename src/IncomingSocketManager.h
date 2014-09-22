#ifndef INCOMINGSOCKETMANAGER_H
#define INCOMINGSOCKETMANAGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BACKLOG 10

class IncomingSocketManager
{
public:
	static int createWelcomeSocket();
	static char* getAddress();
	static int getPort(int listeningSocket);
	static void printSocketInfo(int listeningSocket);
	static int acceptConnection(int listeningSocket);
};

#endif