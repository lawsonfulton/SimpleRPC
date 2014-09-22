#ifndef MY_EXCEPTIONS_H
#define MY_EXCEPTIONS_H

#include <iostream>

enum MY_EXCEPTION
{
	CONNECTION_ERROR = -1,
	ENV_VARS_NOT_SET = -2,
	RECEIVE_HEADER_ERROR = -3,
	RECEIVE_BODY_ERROR = -4,
	SEND_ERROR = -5,
	UNKNOWN_HOST_ERROR = -6,
	SOCKET_ERROR = -7,
	DISCONNECTION_ERROR = -8,
	UNEXPECTED_MESSAGE_ERROR = -9,
	SERVER_FUNCTION_NOT_FOUND = -10,
	BINDER_FUNCTION_NOT_FOUND = -11,
	RUNTIME_LOGIC_ERROR = -99999
};

inline int printException(int e)
{
	//std::cout << "Error: " << e << std::endl;
	return e;
}

inline int printException(int e, std::string msg)
{
	//std::cout << "[" << msg << "] " << "Error: " << e << std::endl;
	return e;
}

#endif