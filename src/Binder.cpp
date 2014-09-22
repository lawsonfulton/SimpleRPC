#include "Binder.h"
#include "Message.h"
#include "Messenger.h"
#include "IncomingSocketManager.h"
#include "MyExceptions.h"

#include <iostream>

int Binder::registerServerFunction(FunctionSignature sig, ServerInfo *info)
{ 
	std::lock_guard<std::mutex> guard(dbMutex);
	database[sig].insert(info);

	std::cout << "Registered function " << sig.name << " from " << info->address << ":" << info->port << std::endl;
	return 0;
}

void Binder::removeServer(ServerInfo *info)
{
	std::lock_guard<std::mutex> guard(dbMutex);

	for(auto iter=database.begin(); iter!=database.end();)
	{
		int i = iter->second.erase(info);
	if(i > 0)
			std::cout << "Erased " << iter->first.name << " for " << info->address <<":" << info->port << std::endl;

		if(iter->second.empty())
		{
			std::cout << "Erased " << iter->first.name << " completely from the database" << std::endl;
			database.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

void Binder::terminateServers()
{
	//Send to all sockets, catch exception if socket is already down
	//because im not going to remove it from th array
	Message term(TERMINATE);
	for(int i = 0; i < serverSockets.size(); i++)
	{
		try
		{
			Messenger::sendMessage(serverSockets[i], &term);
		}
		catch(MY_EXCEPTION e)
		{
			continue; //already down.
		}
	}

	std::cout << "Received TERMINATE message. Quitting." << std::endl;
	exit(0);
}


void Binder::startServerThread(Message *msg, int socket)
{
	std::thread t(&Binder::handleServerConnection, this, msg, socket);
	serverThreads.push_back(std::move(t));
	serverSockets.push_back(socket);
}

void Binder::handleServerConnection(Message *_msg, int socket)
{
	Message *message = _msg;
	bool finished = false;

	//Info of the connected server
	ServerInfo *serverInfo = new ServerInfo(message->serverIdentifier, message->port);

	//std::cout << "Started server thread." << std::endl;

	do
	{
		if(message->type == REGISTER)
		{
			FunctionSignature sig(message->name, message->argTypes);
			int result = registerServerFunction(sig, serverInfo);

			//std::cout << "Sending response." << std::endl;
			Message response(REGISTER_SUCCESS, result);
			try
			{
				Messenger::sendMessage(socket, &response);
			} catch(MY_EXCEPTION e)
			{
				printException(e, "Binder::handleServerConnection");
			}
		}
		else
		{
			throw UNEXPECTED_MESSAGE_ERROR;
		}

		//std::cout << "Waiting for next message." << std::endl;

		//Keep waiting to hear from the server
		//delete message; //TODO why isnt this working??
		if(!finished)
		{
			try
			{
				message = Messenger::receiveMessage(socket); 
				//std::cout << "Received a message" << std::endl;
			}
			catch(MY_EXCEPTION e)
			{
				if(e == DISCONNECTION_ERROR)
				{
					std::cout << serverInfo->address <<":"<<serverInfo->port<< " disconnected." << std::endl;
					removeServer(serverInfo);
					finished = true;
				} else
				{
					printException(e, "Binder::handleServerConnection");
					finished = true;
				}
			}
		}
	} while(!finished);
}

ServerInfo* Binder::getMostStaleServer(std::set<ServerInfo*> &serverSet)
{
	ServerInfo *server = *serverSet.begin();
	auto lowestTime = server->lastExecuted;

	for(auto it = serverSet.begin(); it != serverSet.end(); it++)
	{
		ServerInfo *s = *it;
		if (s->lastExecuted < lowestTime)
		{
			server = s;
			lowestTime = s->lastExecuted;
		}
	}

	server->lastExecuted = std::chrono::high_resolution_clock::now();
	return server;
}

void Binder::sendLookupResponse(Message *msg, int socket)
{
	std::lock_guard<std::mutex> guard(dbMutex);

	FunctionSignature sig(msg->name, msg->argTypes);

	auto it = database.find(sig);
	if(it == database.end())
	{
		std::cout << "Couldn't find function " << sig.name <<" with those args." << std::endl;
		Message msg(LOC_FAILURE, BINDER_FUNCTION_NOT_FOUND);
		Messenger::sendMessage(socket, &msg);
	}
	else
	{
		std::cout << "Performed lookup for function " << sig.name << std::endl;
		//TODO Round Robin goes here
		ServerInfo *server = getMostStaleServer(it->second);

		Message msg(LOC_SUCCESS, server->address, server->port);
		Messenger::sendMessage(socket, &msg);
	}
}


int Binder::run()
{
	try
	{
	listeningSocket = IncomingSocketManager::createWelcomeSocket();
	IncomingSocketManager::printSocketInfo(listeningSocket);

	//std::cout << "Listening for connections." << std::endl;

	//Should start THIS into new thread, have another loop to keep accepting
	while(true)
	{
		int incomingSocket = IncomingSocketManager::acceptConnection(listeningSocket);
		if (incomingSocket == -1)
		{
			std::cout << "Error: Couldn't accepting a connection. Continuing.." << std::endl;
			continue;
		}

		//std::cout << "Accepted new connection." << std::endl;

		Message *msg = Messenger::receiveMessage(incomingSocket);
		
		//std::cout << "Received a message." << std::endl;

		switch(msg->type)
		{
			case REGISTER:
				//std::cout << "Starting a server thread." << std::endl;
				startServerThread(msg, incomingSocket);
				break;
			case LOC_REQUEST: 
				sendLookupResponse(msg, incomingSocket);
				break;
			case TERMINATE:
				//std::cout << "Received termination request." << std::endl;
				terminateServers();
				break;
			default:
				std::cout << "This shouldn't happen. Got connection with invalid command" << std::endl;
				exit(1);
		}

		delete msg;
	}
	}
	catch(MY_EXCEPTION e)
	{
		return printException(e);
	}


	return 0;
}




