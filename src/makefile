all : binder librpc.a 

client : client1

server : server

CPPFLAGS= -std=c++0x -pthread
COMMON_SRCS= Message.cpp Messenger.cpp IncomingSocketManager.cpp OutgoingSocketManager.cpp Database.cpp

BINDER_SRCS= BinderMain.cpp Binder.cpp

RPC_SRCS= rpc.cpp RPCServer.cpp

BINDER_OBJS=$(subst .cpp,.o,$(BINDER_SRCS) $(COMMON_SRCS))
RPC_OBJS=$(subst .cpp,.o,$(RPC_SRCS) $(COMMON_SRCS))
ALL_OBJS=$(subst .cpp,.o,$(RPC_SRCS) $(COMMON_SRCS) $(BINDER_OBJS))

binder: $(BINDER_OBJS)
	g++ -std=c++0x -o binder $(BINDER_OBJS) -pthread

librpc.a: $(RPC_OBJS)
	ar rcs librpc.a $(RPC_OBJS)

#TODO: Remove these before final submission
client1: client1.c librpc.a
	g++ -std=c++0x -w client1.c -o client1 librpc.a -pthread

server: server.c server_function_skels.c server_functions.c librpc.a
	g++ -std=c++0x -w server.c server_function_skels.c server_functions.c -o server librpc.a -pthread

depend: .depend

.depend: $(BINDER_SRCS) $(COMMON_SRCS) $(RPC_SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(ALL_OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend