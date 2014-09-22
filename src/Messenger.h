#ifndef MESSENGER_H
#define MESSENGER_H

#include "Message.h"

class Messenger
{
private:
	static int sendAll(int sockfd, char *buf, int len);
	static int receiveExactBytes(int socketfd, char* buf, int len);
	static void receiveHeader(int socketfd, int* msgLength, int* msgType);

public:
	static void sendMessage(int socketfd, Message* message);
	static Message* receiveMessage(int socketfd);

};

#endif