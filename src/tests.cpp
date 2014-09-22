#include <iostream>
#include <map>
#include "rpc.h"
#include "Database.h"
#include "Message.h"

int main(int argc, char const *argv[])
{
	std::map<FunctionSignature, ServerInfo> db;

	int count0 = 3;
	int count1 = 5;
	int count2 = 3;
	int count3 = 1;
	int count4 = 1;
	int count5 = 3;
	int argTypes0[count0 + 1];
	int argTypes1[count1 + 1];
	int argTypes2[count2 + 1];
	int argTypes3[count3 + 1];
	int argTypes4[count4 + 1];
	int argTypes5[count5 + 1];

	argTypes0[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
	argTypes0[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes0[2] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes0[3] = 0;

	argTypes1[0] = (1 << ARG_OUTPUT) | (ARG_LONG << 16);
	argTypes1[1] = (1 << ARG_INPUT) | (ARG_CHAR << 16);
	argTypes1[2] = (1 << ARG_INPUT) | (ARG_SHORT << 16);
	argTypes1[3] = (1 << ARG_INPUT) | (ARG_INT << 16);
	argTypes1[4] = (1 << ARG_INPUT) | (ARG_LONG << 16);
	argTypes1[5] = 0;

	/* 
	* the length in argTypes2[0] doesn't have to be 100,
	* the server doesn't know the actual length of this argument
	*/
	argTypes2[0] = (1 << ARG_OUTPUT) | (ARG_CHAR << 16) | 100;
	argTypes2[1] = (1 << ARG_INPUT) | (ARG_FLOAT << 16);
	argTypes2[2] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
	argTypes2[3] = 0;

	argTypes5[0] = (1 << ARG_OUTPUT) | (ARG_CHAR << 16);//` | 100;
	argTypes5[1] = (1 << ARG_INPUT) | (ARG_FLOAT << 16);
	argTypes5[2] = (1 << ARG_INPUT) | (ARG_DOUBLE << 16);
	argTypes5[3] = 0;

	/*
	* f3 takes an array of long. 
	*/
	argTypes3[0] =  (1 << ARG_INPUT) | (ARG_LONG << 16) | 11;
	argTypes3[1] = 0;

	std::cout << Message::getArgDataType(argTypes3[0]) << std::endl;
	std::cout << Message::getArgIOType(argTypes3[0]) << std::endl;
	std::cout << Message::getArgArrayLength(argTypes3[0]) << std::endl;

	/* same here, 28 is the exact length of the parameter */
	argTypes4[0] = (1 << ARG_INPUT) | (ARG_CHAR << 16) | 28;
	argTypes4[1] = 0;

	FunctionSignature f0("f0", argTypes0);
	FunctionSignature f1("f1", argTypes1);
	FunctionSignature f2("f2", argTypes2);
	FunctionSignature f3("f3", argTypes3);
	FunctionSignature f4("f4", argTypes4);
	FunctionSignature f5("f2", argTypes5);

	ServerInfo info("some-server",12345);

	 db[f0] = ServerInfo("s",0);
	 db[f1] = ServerInfo("s",1);
	 db[f2] = ServerInfo("s",2);
	 db[f3] = ServerInfo("s",3);
	 db[f4] = ServerInfo("s",4);
	 db[f5] = ServerInfo("s",5); // Need to fix this, should be diff.

	//std::cout << (f2 < f5) << std::endl;
	//std::cout << (f5 < f2) << std::endl;

	//std::cout << "size: " <<db.size() << std::endl;
	//std::cout << db[FunctionSignature("f2",argTypes2)].port << std::endl;

	

	return 0;
}