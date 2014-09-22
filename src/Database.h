#ifndef DATABASE_H
#define DATABASE_H

#include <string.h>
#include <chrono>


class FunctionSignature
{
private:
	int argTypesLength;
	void calcLength();
	int normalizeArrayLength(int a) const;

public:
	const char *name;
	int *argTypes;

	FunctionSignature(const char *name, int *argTypes);
	bool operator<( const FunctionSignature& other) const;
};

class ServerInfo
{
public:
	const char* address;
	int port;

	std::chrono::time_point<std::chrono::high_resolution_clock> lastExecuted;
	
	ServerInfo(const char* address, int port);
	ServerInfo();

	bool operator==( const ServerInfo& other) const;
	bool operator<( const ServerInfo& other) const;
};

#endif