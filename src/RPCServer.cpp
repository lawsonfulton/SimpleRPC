#include "RPCServer.h"
#include "MyExceptions.h"
#include "Database.h"
#include "IncomingSocketManager.h"

#include <vector>
#include <map>
#include <utility>
#include <stdlib.h>
#include <unistd.h>
#include <thread>

/* 
 * First, it creates a connection socket to be used for accepting connections from
 * clients. Secondly, it opens a connection to the binder, this connection is also
 * used by the server for sending register requests to the binder and is left open
 * as long as the server is up so that the binder knows the server is available.
 * 
 * The return value is 0 for success, negative if any part of the initialization
 * sequence was unsuccessful
 */
int RPCServer::rpcInit() 
{
	char* addr = getenv("BINDER_ADDRESS");
	char* port = getenv("BINDER_PORT");

	try
	{
		if(addr == NULL || port == NULL)
			throw ENV_VARS_NOT_SET;

		binderSocket = OutgoingSocketManager::connectTo(addr, port);
	}
	catch(MY_EXCEPTION e)
	{
		return printException(e);
	}

	//Get Server Port info
	listening_server_socket = IncomingSocketManager::createWelcomeSocket();
	char* serverAddress_ch = IncomingSocketManager::getAddress();
	strcpy(serverAddress, serverAddress_ch);

	serverPort = IncomingSocketManager::getPort(listening_server_socket);
	
	std::cout << "Server connected to Binder." << std::endl;

	return 0;
}

/*
 * This function does two key things. It calls the binder, informing it that a
 * server procedure with the indicated name and list of argument types is
 * available at this server. The result returned is 0 for a successful
 * registration, positive for a warning (e.g., this is the same as some previously
 * registered procedure), or negative for failure (e.g., could not locate binder).
 * 
 * The function also makes an entry in a local database, associating the server
 * skeleton with the name and list of argument types. The first two parameters are
 * the same as those for the rpcCall function. The third parameter is the address
 * of the server skeleton, which corresponds to the server procedure that is being
 * registered.
 */
int RPCServer::rpcRegister(char* name, int* argTypes, skeleton f)
{
	int result = 0;

	//add it to the local db.
	FunctionSignature sig(name, argTypes);
	fcnDatabase[sig] = f;

	//std::cout << "Now have " << fcnDatabase.size() << " functions registered locally." << std::endl;
	try
	{
		Message msg(REGISTER, serverAddress, serverPort, name, argTypes);
		Messenger::sendMessage(binderSocket, &msg);

		//Check response
		Message* response = Messenger::receiveMessage(binderSocket);
		if(response->type == REGISTER_FAILURE)
		{
			std::cout << "Register failed: " << response->reasonCode << std::endl;
		}
		else if(response->type == REGISTER_SUCCESS)
		{
			std::cout << "Server registered "<< name << "@" << serverAddress << ":" << serverPort << " with binder." << std::endl;
		}
		else
		{
			throw UNEXPECTED_MESSAGE_ERROR;
		}

		result = response->reasonCode;
		delete response;
	}
	catch(MY_EXCEPTION e)
	{
		return printException(e);
	}

  	return 0;
}

void RPCServer::binderMonitorThread(int binderSocket)
{
	try
	{
		Message *msg = Messenger::receiveMessage(binderSocket);
		if(msg->type == TERMINATE)
		{
			std::cout << "Received TERMINATE message. Quitting." << std::endl;
			exit(0);
		}
	}
	catch(MY_EXCEPTION e)
	{
	}
	std::cout << "Binder disconnected. Quitting." << std::endl;
	exit(0);
}

/*
 * The skeleton function returns an integer to indicate if the server function
 * call executes correctly or not. In the normal case, it will return zero. In
 * case of an error it will return a negative value meaning that the server
 * function execution failed (for example, wrong arguments). In this case, the
 * RPC library at the server side should return an RPC failure message to the
 * client.
 * The server finally calls rpcExecute, which will wait for and receive requests,
 * forward them to skeletons, and send back the results.
 * It hands over control to the skeleton, which is expected to unmarshall the
 * message, call the appro- priate procedures as requested by the clients, and
 * marshall the returns. Then rpcExecute sends the result back to the client. It
 * returns 0 for normally requested termination (the binder has requested
 * termination of the server) and negative otherwise (e.g. if there are no
 * registered procedures to serve).
 * rpcExecute should be able to handle multiple requests from clients without
 * blocking, so that a slow server function will not choke the whole server.
 */
//Note that IncomingManager.h functions need to be public (in order to invoke from it as its a static class)
int RPCServer::rpcExecute( void )
{
	std::thread t2(&RPCServer::binderMonitorThread, this, binderSocket);
	threads.push_back(std::move(t2));

	while(1)
	{
		//std::cout << "Waiting for connection from client." << std::endl;
		int clientSocket = IncomingSocketManager::acceptConnection(listening_server_socket);
		//std::cout << "Received a connection. Starting thread." << std::endl;
		std::thread t(&RPCServer::handleConnectionThread, this, clientSocket);
		threads.push_back(std::move(t));
	}
	return 0;
}


void RPCServer::handleConnectionThread(int clientSocket)
{
	//std::cout << "Thread started." << std::endl;
	const char* name;
	int* arg_types;
	Message* msg;
	try
	{
		//std::cout << "Receiving message." << std::endl;
		msg = Messenger::receiveMessage(clientSocket);

		name = msg->name;
		arg_types = msg->argTypes;

	}
	catch(MY_EXCEPTION e)
	{
		printException(e);
		return;
	}

	Message msg_to_client;

	FunctionSignature sig(name, arg_types);
	std::map<FunctionSignature, skeleton>::iterator it = fcnDatabase.find(sig);
	if(it == fcnDatabase.end())
	{
		std::cout << "Couldn't find function " << name << std::endl;
		msg_to_client = Message(EXECUTE_FAILURE, SERVER_FUNCTION_NOT_FOUND);
	}
	else
	{
		skeleton f = it->second;
		std::cout << "Executing function " << name << std::endl;
		int res = f(arg_types, msg->args);
		if(res == 0)
		{
			msg_to_client = Message(EXECUTE_SUCCESS, name, arg_types, msg->args);
		} else if(res < 0)
		{
			msg_to_client = Message(EXECUTE_FAILURE, res);
		}
	}

	std::cout << "Sending response for " << name << std::endl;
	Messenger::sendMessage(clientSocket, &msg_to_client);

	//std::cout << "Ending thread. Parent should be waiting." << std::endl;
}


RPCServer::RPCServer()
{
	serverAddress[1023] = '\0';
}

RPCServer::~RPCServer()
{

}