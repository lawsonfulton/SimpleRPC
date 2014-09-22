#ifndef BINDER_H
#define BINDER_H

#include "Message.h"
#include "Database.h"

#include <vector>
#include <map>
#include <set>
#include <thread>
#include <mutex>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

class Binder
{
private:
	std::mutex dbMutex;
	std::map<FunctionSignature, std::set<ServerInfo*>> database;

	int listeningSocket;

	std::vector<int> serverSockets;
	std::vector<std::thread> serverThreads;
	
	int registerServerFunction(FunctionSignature sig, ServerInfo *info);
	void removeServer(ServerInfo *info);
	void terminateServers();

	void startServerThread(Message *msg, int socket);
	void handleServerConnection(Message *_msg, int socket);
	ServerInfo* getMostStaleServer(std::set<ServerInfo*> &serverSet);
	void sendLookupResponse(Message *msg, int socket);

public:
	int run();


};
#endif