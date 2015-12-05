# This is a sample Makefile which compiles source files named:
# - tcpechotimeserv.c
# - tcpechotimecliv.c
# - time_cli.c
# - echo_cli.c
# and creating executables: "server", "client", "time_cli"
# and "echo_cli", respectively.
#
# It uses various standard libraries, and the copy of Stevens'
# library "libunp.a" in ~cse533/Stevens/unpv13e_solaris2.10 .
#
# It also picks up the thread-safe version of "readline.c"
# from Stevens' directory "threads" and uses it when building
# the executable "server".
#
# It is set up, for illustrative purposes, to enable you to use
# the Stevens code in the ~cse533/Stevens/unpv13e_solaris2.10/lib
# subdirectory (where, for example, the file "unp.h" is located)
# without your needing to maintain your own, local copies of that
# code, and without your needing to include such code in the
# submissions of your assignments.


CC = gcc

LIBS = 	-lresolv -lsocket -lnsl -lpthread\
	/home/courses/cse533/Stevens/unpv13e_solaris2.10/libunp.a\
	
	
FLAGS = -g -O2

CFLAGS = ${FLAGS} -I/home/courses/cse533/Stevens/unpv13e_solaris2.10/lib

all: client server echo_cli time_cli

time_cli: time_cli.o
	${CC} ${FLAGS} -o time_cli time_cli.o ${LIBS}
time_cli.o: time_cli.c
	${CC} ${CFLAGS} -c time_cli.c

echo_cli.o: echo_cli.c
	${CC} ${CFLAGS} -c echo_cli.c
echo_cli: echo_cli.o
	${CC} ${FLAGS} -o echo_cli echo_cli.o ${LIBS}

echo_srv.o: echo_srv.c
	${CC} ${CFLAGS} -c echo_srv.c
	

day_srv.o: day_srv.c
	${CC} ${CFLAGS} -c day_srv.c
	
tcpechotimesrv.o: tcpechotimesrv.c
	${CC} ${CFLAGS} -c tcpechotimesrv.c
server: tcpechotimesrv.o echo_srv.o day_srv.o
	${CC} ${FLAGS} -o server tcpechotimesrv.o echo_srv.o day_srv.o ${LIBS}


tcpechotimecli.o: tcpechotimecli.c
	${CC} ${CFLAGS} -c tcpechotimecli.c

client: tcpechotimecli.o
	${CC} ${FLAGS} -o client tcpechotimecli.o ${LIBS}


clean:
	rm server tcpechotimesrv.o client tcpechotimecli.o readline.o echo_cli echo_cli.o echo_srv.o day_srv.o time_cli time_cli.o

