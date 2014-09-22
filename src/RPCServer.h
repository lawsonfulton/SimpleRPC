#ifndef RPCServer_H
#define RPCServer_H

#include "OutgoingSocketManager.h"
#include "Message.h"
#include "Messenger.h"
#include "rpc.h"
#include "Database.h"

#include <vector>
#include <map>
#include <utility>
#include <stdlib.h>
#include <thread>

class RPCServer
{
private:
	std::map<FunctionSignature, skeleton> fcnDatabase;
	std::vector<std::thread> threads;
	
	int listening_server_socket;
	int binderSocket;
	char serverAddress [1024];
	int serverPort;

	void handleConnectionThread(int sock_fd);
	void binderMonitorThread(int binderSocket);

public:
	int rpcInit();
	int rpcRegister(char* name, int* argTypes, skeleton f);
	int rpcExecute(void);

	RPCServer();
   ~RPCServer();
};

#endif