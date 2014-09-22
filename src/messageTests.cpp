#include "Message.h"
#include <assert.h>
#include <iostream>

void testRegister();
void testRegisterSuccess();
void testRegisterFailure();
void testLocReq();
void testLocSuccess();
void testLocFailure();
void testTerminate();

int argTypes[5] = {1,2,3,4,0};
int len = 5;

int type = REGISTER;
const char *serverIdentifier = "test";
const char *name = "Lawson";
int port = 1234;
int reasonCode = 1234;

int main()
{
	testRegister();
	testRegisterSuccess();
	testRegisterFailure();
	testLocReq();
	testLocSuccess();
	testLocFailure();
	testTerminate();

	//testExecute();
	//testExecuteSuccess();
	//testExecuteFailure();
	
}

void printArray(int *array, int len)
{
	for(int i = 0; i < len; i++)
	{
		std::cout << array[i] << ", ";
	}
	std::cout << std::endl;
}

int compareArrays(int *a1, int *a2, int len)
{
	for(int i = 0; i < len; i++)
	{
		if(a1[i] != a2[i])
			return -1;
	}
	return 0;
}

void testRegister()
{
	int type = REGISTER;
	
	Message msg(type, serverIdentifier, port, name, argTypes);

	char *msgData;
	//int length = msg.serialize(msgData);
	msg.serialize(msgData);
	
	Message *newMsg = Message::createFromBytes(type, msgData + 8, msg.length);

	assert(type == newMsg->type);
	assert(strcmp(serverIdentifier, newMsg->serverIdentifier) == 0);
	assert(port == newMsg->port);
	assert(strcmp(name, newMsg->name) == 0);
	assert(compareArrays(argTypes, newMsg->argTypes, len) == 0);

	std::cout << "REGISTER: PASS" << std::endl;

	delete newMsg;
	free(msgData);
}	

void testRegisterSuccess()
{
	int type = REGISTER_SUCCESS;
	
	Message msg(type, reasonCode);

	char *msgData;
	msg.serialize(msgData);
	
	Message *newMsg = Message::createFromBytes(type, msgData + 8, msg.length);

	assert(type == newMsg->type);
	assert(reasonCode == newMsg->reasonCode);

	std::cout << "REGISTER_SUCCESS: PASS" << std::endl;

	delete newMsg;
	free(msgData);
}

void testRegisterFailure()
{
	int type = REGISTER_FAILURE;
	
	Message msg(type, reasonCode);

	char *msgData;
	msg.serialize(msgData);
	
	Message *newMsg = Message::createFromBytes(type, msgData + 8, msg.length);

	assert(type == newMsg->type);
	assert(reasonCode == newMsg->reasonCode);

	std::cout << "REGISTER_FAILURE: PASS" << std::endl;

	delete newMsg;
	free(msgData);
}

void testLocReq()
{
	int type = LOC_REQUEST;
	
	Message msg(type, name, argTypes);

	char *msgData;
	msg.serialize(msgData);
	
	Message *newMsg = Message::createFromBytes(type, msgData + 8, msg.length);

	assert(type == newMsg->type);
	assert(strcmp(name, newMsg->name) == 0);
	assert(compareArrays(argTypes, newMsg->argTypes, len) == 0);

	std::cout << "LOC_REQUEST: PASS" << std::endl;

	delete newMsg;
	free(msgData);
}

void testLocSuccess()
{
	int type = LOC_SUCCESS;
	
	Message msg(type, serverIdentifier, port);

	char *msgData;
	msg.serialize(msgData);
	
	Message *newMsg = Message::createFromBytes(type, msgData + 8, msg.length);

	assert(type == newMsg->type);
	assert(strcmp(serverIdentifier, newMsg->serverIdentifier) == 0);
	assert(port == newMsg->port);

	std::cout << "LOC_SUCCESS: PASS" << std::endl;

	delete newMsg;
	free(msgData);
}

void testLocFailure()
{
	int type = LOC_FAILURE;
	
	Message msg(type, reasonCode);

	char *msgData;
	msg.serialize(msgData);
	
	Message *newMsg = Message::createFromBytes(type, msgData + 8, msg.length);

	assert(type == newMsg->type);
	assert(reasonCode == newMsg->reasonCode);

	std::cout << "LOC_FAILURE: PASS" << std::endl;

	delete newMsg;
	free(msgData);
}

void testTerminate()
{
	int type = TERMINATE;
	
	Message msg(type);

	char *msgData;
	msg.serialize(msgData);
	
	Message *newMsg = Message::createFromBytes(type, msgData + 8, msg.length);

	assert(type == newMsg->type);

	std::cout << "TERMINATE: PASS" << std::endl;

	delete newMsg;
	free(msgData);

}
