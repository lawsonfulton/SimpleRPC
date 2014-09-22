#ifndef MESSAGE_H
#define MESSAGE_H

enum MessageType
{
	REGISTER,
	REGISTER_SUCCESS,
	REGISTER_FAILURE,
	LOC_REQUEST,
	LOC_SUCCESS,
	LOC_FAILURE,
	EXECUTE,
	EXECUTE_SUCCESS,
	EXECUTE_FAILURE,
	TERMINATE
};

class Message
{
private:
	void initVars();

	void serializeHeader(char data[8]);

	int serializeInt(char *destData, int index, int integer);
	int serializeString(char *destData, int index, const char *str);
	int serializeArgTypes(char *destData, int index);
	int serializeInputArgs(char *destData, int index);
	int serializeOutputArgs(char *destData, int index);

	int readServerIdentifier(char* msgData, int index);
	int readPort(char *msgData, int index);
	int readName(char *msgData, int index);
	int readArgTypes(char *msgData, int index);
	int readArgsServerSide(char *msgData, int index);
	int readArgsClientSide(char *msgData, int index);
	int readReasonCode(char *msgData, int index);

	void calcArgsLength();

	static int sizeOfArg(int argType);
	int sizeOfInputArgs();
	int sizeOfOutputArgs();

public:
	int type;
	int length;

	int *argTypes;
	void **args;
	int argsLength;
	int port;
	const char *serverIdentifier;
	const char *name;
	int reasonCode;

	static Message* createFromBytes(int type, char* msgData, int msgLength);

	//Any arrays given to Message have their lifetime managed by message
	Message(int type, const char *serverIdentifier, int port, const char *name, int *argTypes);
	Message(int type, const char *name, int *argTypes);
	Message(int type, const char *serverIdentifier, int port);
	Message(int type, int reasonCode);
	Message(int type, const char *name, int *argTypes, void **args);
	Message(int type);
	Message();
	~Message(); 
	int serialize(char*& completeData);

	static int getArgDataType(int argType);
	static int getArgIOType(int argType);
	static int getArgArrayLength(int argType);
	static bool isOutput(int argType);
	static bool isInput(int argType);
	static int copyVariable(void *dest, void *source, int argType);
};


#endif