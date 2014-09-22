#include "Messenger.h"
#include "MyExceptions.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int Messenger::sendAll(int sockfd, char *buf, int len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = len; // how many we have left to send
    int n;

    while(total < len) {
        n = send(sockfd, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

//Blocks until exactly len bytes have been received
int Messenger::receiveExactBytes(int socketfd, char* buf, int len)
{
	int total = 0;
    while(total < len)
    {
        int bytes_received = 0;
        bytes_received = recv(socketfd, buf + total, len - total, 0);
        
        if (bytes_received <= 0)
            return bytes_received;

        total += bytes_received;
    }

    return total;
}

//Reads the 4 byte header of a message
//Returns the corresponding length of the remaining message
void Messenger::receiveHeader(int socketfd, int* msgLength, int* msgType)
{
	char header[8];
    int bytes = receiveExactBytes(socketfd, header, 8);
    if(bytes == 0)
    	throw DISCONNECTION_ERROR;
    else if(bytes < 0)
        throw RECEIVE_HEADER_ERROR;

    memcpy(msgLength, header, 4);
    memcpy(msgType, header + 4, 4);
}

void Messenger::sendMessage(int socketfd, Message *message)
{
    char *msgData;
    int len = message->serialize(msgData);

    if(sendAll(socketfd, msgData, len) == -1)
        throw SEND_ERROR;

    free(msgData);
}

Message* Messenger::receiveMessage(int socketfd)
{
	int length;
	int type;
	receiveHeader(socketfd, &length, &type);

	char* msgData = (char*)malloc(length);
    int receivedBytes = receiveExactBytes(socketfd, msgData, length);
	if(receivedBytes == 0 && length != 0)
		throw DISCONNECTION_ERROR;
    else if(receivedBytes < 0)
        throw RECEIVE_BODY_ERROR;

	Message* message = Message::createFromBytes(type, msgData, length);

    free(msgData);
	return message;
}


