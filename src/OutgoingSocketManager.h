#ifndef OUTGOINGSOCKETMANAGER_H
#define OUTGOINGSOCKETMANAGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

class OutgoingSocketManager
{
public:
	static int connectTo(const char* server_address, const char* server_port);
	static int connectTo(const char* server_address, int server_port);

};



#endif