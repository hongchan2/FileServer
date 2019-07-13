all : a.out server.out

a.out : Lpc.h LpcProxy.h testcase.c LpcProxy.c
	gcc testcase.c LpcProxy.c -o a.out
server.out : Lpc.h LpcStub.h server.c LpcStub.c
	gcc server.c LpcStub.c -o server.out -lpthread
clean :
	rm -rf a.out server.out */
