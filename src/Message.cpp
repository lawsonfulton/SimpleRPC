#include "Message.h"
#include "rpc.h"
#include "MyExceptions.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <iostream>

Message::Message()
{
	initVars();
}

Message::~Message()
{
	//TODO free memory?
}

void Message::initVars()
{
	argTypes = NULL;
	args = NULL;
	argsLength = 0;
	port = 0;
	serverIdentifier = NULL;
	name = NULL;
	reasonCode = 0;
	length = 0;
	type = -1;
}

Message::Message(int type, const char* serverIdentifier, int port, const char* name, int* argTypes) : 
	type(type),
	serverIdentifier(serverIdentifier),
	port(port),
	name(name),
	argTypes(argTypes),
	args(NULL),
	argsLength(0),
	reasonCode(0),
	length(0)
	{calcArgsLength();}

Message::Message(int type, const char* name, int* argTypes) :
	type(type),
	name(name),
	argTypes(argTypes),
	serverIdentifier(NULL),
	port(0),
	args(NULL),
	argsLength(0),
	reasonCode(0),
	length(0)
	{calcArgsLength();}

Message::Message(int type, const char* serverIdentifier, int port) :
	type(type),
	serverIdentifier(serverIdentifier),
	port(port),
	name(NULL),
	argTypes(NULL),
	args(NULL),
	argsLength(0),
	reasonCode(0),
	length(0){}

Message::Message(int type, int reasonCode) :
	type(type),
	reasonCode(reasonCode),
	serverIdentifier(NULL),
	port(0),
	name(NULL),
	argTypes(NULL),
	args(NULL),
	argsLength(0),
	length(0){}

Message::Message(int type, const char *name, int* argTypes, void** args) :
	type(type),
	argTypes(argTypes),
	args(args),
	reasonCode(0),
	serverIdentifier(NULL),
	port(0),
	name(name),
	argsLength(0),
	length(0)
	{calcArgsLength();}

Message::Message(int type) :
	type(type),
	argTypes(NULL),
	args(NULL),
	reasonCode(0),
	serverIdentifier(NULL),
	port(0),
	name(NULL),
	argsLength(0),
	length(0) {}


//Copies header data into preallocated array
void Message::serializeHeader(char data[8])
{
    memcpy(data, &length, 4);
    memcpy(data + 4, &type, 4);
}

int Message::serializeInt(char *destData, int index, int integer)
{
	memcpy(destData + index, &integer, 4);
	return index + 4;
}

int Message::serializeString(char *destData, int index, const char *str)
{
    strcpy(destData + index, str);

    return index + strlen(str) + 1;
}

//Should explicitly set argsLength in case it isn't set
int Message::serializeArgTypes(char *destData, int index)
{
	memcpy(destData + index, argTypes, (argsLength + 1) * 4);

	return index + (argsLength + 1) * 4;
}

//only serializes input or input + output args
int Message::serializeInputArgs(char *destData, int index)
{
	int i = 0;
	while(argTypes[i] != 0)
	{
		int argType = argTypes[i];

		if(isInput(argType))
		{
			index += copyVariable(destData + index, args[i], argType);
		}

		i++;
	}
	return index;
}

//only serializes output or input + output args
int Message::serializeOutputArgs(char *destData, int index)
{
	int i = 0;
	while(argTypes[i] != 0)
	{
		int argType = argTypes[i];

		if(isOutput(argType))
			index += copyVariable(destData + index, args[i], argType);

		i++;
	}
	return index;
}

int Message::serialize(char*& completeData)
{
	int index = 8; //start serializing after the header
	switch(type)
	{
		case REGISTER:
			length += strlen(serverIdentifier) + 1;
			length += 4; //port
			length += strlen(name) + 1;
			length += (argsLength + 1)*4; //argsTypes 1 longer than args

			completeData = (char*) malloc(8 + length);

			index = serializeString(completeData, index, serverIdentifier);
			index = serializeInt(completeData, index, port);
			index = serializeString(completeData, index, name);
			index = serializeArgTypes(completeData, index);
			break;

		case LOC_REQUEST:
			length += strlen(name) + 1;
			length += (argsLength + 1) * 4;

			completeData = (char*) malloc(8 + length);

			index = serializeString(completeData, index, name);
			index = serializeArgTypes(completeData, index);
			break;

		case LOC_SUCCESS:
			length += strlen(serverIdentifier) + 1;
			length += 4; //port

			completeData = (char*) malloc(8 + length);

			index = serializeString(completeData, index, serverIdentifier);
			index = serializeInt(completeData, index, port);
			break;

		case EXECUTE:
			length += strlen(name) + 1;
			length += (argsLength + 1) * 4;
			length += sizeOfInputArgs();
			completeData = (char*) malloc(8 + length);

			index = serializeString(completeData, index, name);
			index = serializeArgTypes(completeData, index);
			index = serializeInputArgs(completeData, index);
			break;

		case EXECUTE_SUCCESS:
			length += strlen(name) + 1;
			length += (argsLength + 1) * 4;
			length += sizeOfOutputArgs();

			completeData = (char*) malloc(8 + length);

			index = serializeString(completeData, index, name);
			index = serializeArgTypes(completeData, index);
			index = serializeOutputArgs(completeData, index);

			break;

		case REGISTER_SUCCESS: 
		case REGISTER_FAILURE:
		case LOC_FAILURE:
		case EXECUTE_FAILURE:
			length += 4; //reason code
			completeData = (char*) malloc(8 + length);

			index = serializeInt(completeData, index, reasonCode);
			break;

		case TERMINATE:
			completeData = (char*) malloc(8);
			break;

		default:
			std::cout << "Error: Invalid message type: " << type << std::endl;
			exit(1);
	}

	serializeHeader(completeData); //Add the header to the first 8 bytes

	return length + 8;
}

int Message::readServerIdentifier(char *data, int index)
{
	char* tempStr = (char*)malloc(length); //The identifier can be no longer than the length of the message
	strncpy(tempStr, data + index, length - index);
	serverIdentifier = tempStr;
	return index + strlen(serverIdentifier) + 1;
}


int Message::readPort(char *data, int index)
{
    memcpy(&port, data + index, 4);
    return index + 4;
}

int Message::readName(char *data, int index)
{
	char* tempStr = (char*)malloc(length); //The name can be no longer than the length of the message
	strncpy(tempStr, data + index, length - index);
	name = tempStr;
	return index + strlen(name) + 1;
}

int Message::readArgTypes(char *data, int index)
{
	argsLength = 0;
	int val = 0;
	do
	{
    	memcpy(&val, data + index + argsLength * 4,  4);    	
		argsLength++;
	} while(val != 0);

	argTypes = (int*)malloc(argsLength * 4);
	memcpy(argTypes, data + index, argsLength * 4);
	
	index += argsLength * 4;
	argsLength--; //argTypes one longer than args

	return index;
}

int Message::readReasonCode(char *data, int index)
{
	memcpy(&reasonCode, data + index, 4);
    return index + 4;
}

void Message::calcArgsLength()
{
	int arg, i = 0;
	do
	{
		arg = argTypes[i];
		i++;
	} while(arg != 0);

	argsLength = i - 1; //length of args is 1 less
}

int Message::readArgsServerSide(char *msgData, int index)
{
	//Need to malloc args array
	args = (void**)malloc(sizeof(void*) * argsLength);

	int i = 0;
	while(argTypes[i] != 0)
	{
		int argType = argTypes[i];
		if(isInput(argType))
		{
			args[i] = malloc(sizeOfArg(argType));
			index += copyVariable(args[i], msgData + index, argType);

		}
		else if(isOutput(argType) && !isInput(argType))
		{
			//just allocate space
			args[i] = malloc(sizeOfArg(argType));
			//don't increment index TODO is this right?
		}

		i++;
	}

	return index;
}

//only read in output args
int Message::readArgsClientSide(char *msgData, int index)
{
	//Need to malloc args array
	args = (void**)malloc(sizeof(void*) * argsLength);

	int i = 0;
	while(argTypes[i] != 0)
	{
		int argType = argTypes[i];
		if(isOutput(argType))
		{
			args[i] = malloc(sizeOfArg(argType));
			index += copyVariable(args[i], msgData + index, argType);
		}

		i++;
	}

	return index;
}


Message* Message::createFromBytes(int type, char* msgData, int msgLength)
{
	Message* msg = new Message();
	msg->type = type;
	msg->length = msgLength;

	int index = 0;
	switch(type)
	{
		case REGISTER:
			index = msg->readServerIdentifier(msgData, index);
			index = msg->readPort(msgData, index);
			index = msg->readName(msgData, index);
			index = msg->readArgTypes(msgData, index);
			break;

		case LOC_REQUEST:
			index = msg->readName(msgData, index);
			index = msg->readArgTypes(msgData, index);
			break;

		case LOC_SUCCESS:
			index = msg->readServerIdentifier(msgData, index);
			index = msg->readPort(msgData, index);
			break;

		case EXECUTE:
			index = msg->readName(msgData, index);
			index = msg->readArgTypes(msgData, index);
			index = msg->readArgsServerSide(msgData, index);
			break;

		case EXECUTE_SUCCESS:
			index = msg->readName(msgData, index);
			index = msg->readArgTypes(msgData, index);
			index = msg->readArgsClientSide(msgData, index);
			break;

		case REGISTER_SUCCESS: 
		case REGISTER_FAILURE:
		case LOC_FAILURE:
		case EXECUTE_FAILURE:
			index = msg->readReasonCode(msgData, index);
			break;

		case TERMINATE:
			break;

		default:
			std::cout << "Received unknown message" << std::endl;
			exit(1);
	}

	assert (index == msgLength);
	return msg;
}

int Message::getArgDataType(int argType)
{
	return (argType & 0x00ff0000) >> 16;
}

int Message::getArgIOType(int argType)
{
	int io = (argType >> 30) & 3;
	switch(io)
	{
		case 1:
			return ARG_OUTPUT;
			break;
		case 2:
			return ARG_INPUT;
			break;
		case 3:
			return ARG_INPUT + ARG_OUTPUT;
			break;
		default:
			return -1;
	}
}

int Message::getArgArrayLength(int argType)
{
	return argType & 0x0000ffff;
}

bool Message::isOutput(int argType)
{
	int io = getArgIOType(argType);
	return io == ARG_OUTPUT || io == ARG_OUTPUT + ARG_INPUT;
}

bool Message::isInput(int argType)
{
	int io = getArgIOType(argType);
	return io == ARG_INPUT || io == ARG_OUTPUT + ARG_INPUT;
}

//returns size of data copied
int Message::copyVariable(void *dest, void *source, int argType)
{
	int len = getArgArrayLength(argType);
	if(len == 0) len++;

	int size = 0;

	for(int i = 0; i < len; i++)
	{
		switch(getArgDataType(argType))
		{
			case ARG_CHAR:
				*(((char*)dest) + i) = *( ((char*)source) + i);
				size += sizeof(char);
				break; 
			case ARG_SHORT:
				*(((short*)dest) + i) = *( ((short*)source) + i);
				size += sizeof(short);
				break;
			case ARG_INT:
				*(((int*)dest) + i) = *( ((int*)source) + i);
				size += sizeof(int);
				break;  
			case ARG_LONG:
				*(((long*)dest) + i) = *( ((long*)source) + i);
				size += sizeof(long);
				break; 
			case ARG_DOUBLE:
				*(((double*)dest) + i) = *( ((double*)source) + i);
				size += sizeof(double);
				break;
			case ARG_FLOAT:
				*(((float*)dest) + i) = *( ((float*)source) + i);
				size += sizeof(float);
				break;
			default:
				throw RUNTIME_LOGIC_ERROR;
		}
	}

	return size;
}

int Message::sizeOfArg(int argType)
{
	int len = getArgArrayLength(argType);
	if(len == 0) len++;

	switch(getArgDataType(argType))
	{
		case ARG_CHAR:
			return sizeof(char) * len;
			break; 
		case ARG_SHORT:
			return sizeof(short) * len;
			break;
		case ARG_INT:
			return sizeof(int) * len;
			break;  
		case ARG_LONG:
			return sizeof(long) * len;
			break; 
		case ARG_DOUBLE:
			return sizeof(double) * len;
			break;
		case ARG_FLOAT:
			return sizeof(float) * len;
			break;
		default:
			throw RUNTIME_LOGIC_ERROR;
	}

	return 0;
}

int Message::sizeOfInputArgs()
{
	int size = 0;
	int i = 0;
	while(argTypes[i] != 0)
	{
		if(isInput(argTypes[i]))
			size += sizeOfArg(argTypes[i]);
		i++;
	}

	return size;
}

int Message::sizeOfOutputArgs()
{
	int size = 0;
	int i = 0;
	while(argTypes[i] != 0)
	{
		if(isOutput(argTypes[i]))
			size += sizeOfArg(argTypes[i]);
		i++;
	}

	return size;
}
