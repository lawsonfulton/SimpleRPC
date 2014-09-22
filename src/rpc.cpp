#include "rpc.h"
#include "RPCServer.h"
#include "MyExceptions.h"

RPCServer *server = NULL;

int rpcInit()
{
	try
	{
   		server = new RPCServer();
   		return server->rpcInit();
   	}
   	catch(MY_EXCEPTION e)
	{
		return e;
	}
}

int rpcRegister(char* name, int* argTypes, skeleton f)
{
	try
	{
		return server->rpcRegister(name, argTypes, f);
   	}
   	catch(MY_EXCEPTION e)
	{
		return e;
	}
}

int rpcExecute()
{
   	try
	{
		int result = server->rpcExecute();
		delete server;

		return result;
   	}
   	catch(MY_EXCEPTION e)
	{
		return e;
	}
}

void lookupBinder(char *&address, char *&port)
{
	address = getenv("BINDER_ADDRESS");
	port = getenv("BINDER_PORT");	

	if(address == NULL || port == NULL)
		throw ENV_VARS_NOT_SET;
}

ServerInfo lookupServerInfo(char *name, int *argTypes)
{
	int binderSocket;
	char *binderAddress = NULL, *binderPort = NULL;
	lookupBinder(binderAddress, binderPort);
	
	////std::cout << "Connecting to the binder at " << binderAddress << ":" << binderPort << std::endl;
	//Connect to the binder
	binderSocket = OutgoingSocketManager::connectTo(binderAddress, binderPort);

	//std::cout << "Requesting location of function " << name << std::endl;
	//Get function location
	Message request(LOC_REQUEST, name, argTypes);
	Messenger::sendMessage(binderSocket, &request);
	Message *response = Messenger::receiveMessage(binderSocket);

	ServerInfo info;
	if(response->type == LOC_SUCCESS)
	{
		info = ServerInfo(response->serverIdentifier, response->port);
		//std::cout << "Found function " << name << " at " << info.address <<":"<<info.port<< std::endl;
	}
	else if(response->type == LOC_FAILURE)
	{
		//std::cout << "Binder could not locate " << name << std::endl;
		throw (MY_EXCEPTION)response->reasonCode;
	}
	else
	{
		throw UNEXPECTED_MESSAGE_ERROR;
	}

	return info;
}

//Copy values from response args to the locations specified by local args
void copyOutputArgs(int *argTypes, void **localArgs, void **respArgs)
{
	int i = 0;
	while(argTypes[i] != 0)
	{
		int argType = argTypes[i];
		if(Message::isOutput(argType))
		{
			Message::copyVariable(localArgs[i], respArgs[i], argType);
		}

		i++;
	}
}

int executeOnServer(ServerInfo info, char *name, int *argTypes, void **args)
{
	//std::cout << "Connecting to server." << std::endl;
	int serverSocket = OutgoingSocketManager::connectTo(info.address, info.port);

	//std::cout << "Connected to server. Sending Message." << std::endl;
	Message request(EXECUTE, name, argTypes, args);
	Messenger::sendMessage(serverSocket, &request);
	//std::cout << "Waiting for response." << std::endl;
	Message *response = Messenger::receiveMessage(serverSocket);

	if(response->type == EXECUTE_SUCCESS)
	{
		copyOutputArgs(argTypes, args, response->args);
		return 0;
	}
	else if(response->type == EXECUTE_FAILURE)
	{
		return response->reasonCode;
	}
	else
	{
		throw UNEXPECTED_MESSAGE_ERROR;
	}
}

int rpcCall(char* name, int* argTypes, void** args)
{
	try
	{
		//std::cout << "Looking up binder location." << std::endl;
		ServerInfo info = lookupServerInfo(name, argTypes);
		
		//std::cout << "Sending rpc request to server at " << info.address <<":"<< info.port << std::endl;
		return executeOnServer(info, name, argTypes, args);
	}
	catch(MY_EXCEPTION e)
	{
		return printException(e);
	}

	return 0;
}

int rpcCacheCall(char* name, int* argTypes, void** args)
{
   return 0;
}

int rpcTerminate()
{
	int binderSocket;

	try
	{
		int binderSocket;
		char *binderAddress = NULL, *binderPort = NULL;
		lookupBinder(binderAddress, binderPort);

		//Connect to the binder
		binderSocket = OutgoingSocketManager::connectTo(binderAddress, binderPort);

		//Get function location
		Message request(TERMINATE);
		Messenger::sendMessage(binderSocket, &request);

		//TODO do I need this to ensure binder shuts down?
		try
		{
			Messenger::receiveMessage(binderSocket);
		} 
		catch(MY_EXCEPTION e)
		{
			if(e == DISCONNECTION_ERROR)
				return 0;
			else
				throw e;
		}
	}
	catch(MY_EXCEPTION e)
	{
		return printException(e);
	}

   return 0;
}



