#include <iostream>

#include "Database.h"

FunctionSignature::FunctionSignature(const char *name, int *argTypes) : name(name), argTypes(argTypes)
{
	calcLength();	
}

void FunctionSignature::calcLength()
{
	int arg, i = 0;
	do
	{
		arg = argTypes[i];
		i++;
	} while(arg != 0);

	argTypesLength = i;
}

//Basically make array length 0 or 1
int FunctionSignature::normalizeArrayLength(int a) const
{
	int lowerTwo = a & 0x0000ffff; //Get array length
	int result = a;

	//if array length > 0, change to 1;
	if(lowerTwo > 0)
		result = (a & 0xffff0000) | 1;

	return result;
}

bool FunctionSignature::operator<( const FunctionSignature& other) const
{
	//Compare name first
	int streq = strcmp(name, other.name);
	if(streq != 0)
		return streq < 0;

	//compare argTypes array lengths
	if(argTypesLength < other.argTypesLength)
		return true;
	if(argTypesLength > other.argTypesLength)
		return false;

	//compare argtypes if equal lengths
	for (int i = 0; i < argTypesLength - 1; ++i) //Dnot bother with 0 at end
	{
		int a1 = argTypes[i];
		int a2 = other.argTypes[i];

		a1 = normalizeArrayLength(a1);
		a2 = normalizeArrayLength(a2);
		
		//Check input flags
		int io1 = (a1 >> 30) & 3;
		int io2 = (a2 >> 30) & 3;

		if(io1 != io2)
			return io1 < io2;

		//Check type flag
		int t1 = (a1 & 0x00ff0000) >> 16;
		int t2 = (a2 & 0x00ff0000) >> 16;

		if(t1 != t2)
			return t1 < t2;

		//Check if is array
		int l1 = a1 & 1;
		int l2 = a2 & 1;

		if(l1 != l2)
			return l1 < l2;
	}

	return false;
}

ServerInfo::ServerInfo(const char *address, int port) : address(address), port(port), 
	lastExecuted(std::chrono::time_point<std::chrono::high_resolution_clock>()){}

ServerInfo::ServerInfo() : address(""), port(-1), lastExecuted(std::chrono::time_point<std::chrono::high_resolution_clock>()) {}

bool ServerInfo::operator==( const ServerInfo& other) const
{
	return strcmp(address, other.address) == 0 && port == other.port;
}

bool ServerInfo::operator<( const ServerInfo& other) const
{
	int cmp = strcmp(address, other.address);
	if(cmp != 0)
		return cmp < 0;
	else
		return port < other.port;
}