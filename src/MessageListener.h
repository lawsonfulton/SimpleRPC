#ifndef MESSAGELISTENER_H
#define MESSAGELISTENER_H

#include "Message.h"

//Uses a message factory to create a message by reading a socket
class MessageListener
{
public:
	static Message* listen(int socketfd);

private:
	static int receiveExactBytes(int socketfd, char* buf, int len);
	static void receiveHeader(int socketfd, int* msgLength, int* msgType);
};
#endif