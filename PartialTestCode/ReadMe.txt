rpc.h: include file for your code
Defines all rpc related information. Declares rpc library functions like
rpcInit, rpcRegister etc.

client1.c
A sample test client for testing functions implemented by the test server.

server.c :
A sample server that implements a few functions and registers them with the
binder.

server_functions.h and server_functions.c :
Declares and defines the functions implemented by the sample server.

server_function_skels.h and server_function_skels.c :
Declaration & definition of skeleton functions for each of the sample
server's functions. Responsible for marshaling/unmarshaling and calling
the appropriate server function.
