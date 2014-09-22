#include "MessageListener.h"
#include "helperFcns.h"


//Blocks until exactly len bytes have been received
int MessageListener::receiveExactBytes(int socketfd, char* buf, int len)
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
void MessageListener::receiveHeader(int socketfd, int* msgLength, int* msgType)
{
	char header[8];
    if(receiveExactBytes(socketfd, header, 8) != 8)
    	handle_error("recv"); //TODO

    memcpy(header, msgLength, 4);
    memcpy(header + 4, msgType, 4);
}


Message* MessageListener::listen(int socketfd)
{
	int length;
	int type;
	receiveHeader(socketfd, &length, &type);

	char* msgData = (char*)malloc(length);
	if(receiveExactBytes(socketfd, msgData, length) != length)
		handle_error("recv");

	Message* message = new Message(type, msgData, length);

	return message;
}