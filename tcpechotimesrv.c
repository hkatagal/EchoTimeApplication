/*
* Author: Harishkumar Katagal(109915793)
* FileName: tcpechotimesrv.c
* This program is an implementation of multi-threading and server services. Two services are
* supported namely echo and time. The server listens on two ports, 15794 for echo server and 
* 15793 for the time server. When a client connects to the server it creates and thread to 
* handle the service. The service depends on the port connected. 
*/

#include	"unp.h"
#include	<time.h>
#include	<fcntl.h>

/*The functions that the thread uses for echo and time servers*/
static void  *echo(void *);
static void *daytime(void *);


int
main(int argc, char **argv)
{
	/*Variable Declarations*/
	int					listenecho, listendaytime;
	int  i, maxi, maxfd,client[FD_SETSIZE];
	int nready;
	int 	*connfd;
	struct sockaddr_in	servaddr_echo,cliaddr;
	struct sockaddr_in	servaddr_daytime;
	
	int so_echo = 1, so_time=1;
	char				buff[MAXLINE];
	ssize_t		n;
	char		buf[MAXLINE];
	pthread_t tid_echo, tid_time;	
	fd_set rset, allset;
	socklen_t clilen;
	
	int echoFlag,timeFlag;
	
	/* Creating two sockets for the two services. */
	listenecho = Socket(AF_INET, SOCK_STREAM, 0);
	listendaytime = Socket(AF_INET,SOCK_STREAM,0);
	
	/* Setting the sockoptions so that the sockets can be resused. */
	if(setsockopt(listenecho, SOL_SOCKET,SO_REUSEADDR, &so_echo,sizeof(so_echo))<0)
		err_sys("socket cannot be reused");
	
	if(setsockopt(listendaytime, SOL_SOCKET,SO_REUSEADDR, &so_time,sizeof(so_time))<0)
		err_sys("socket cannot be reused");
	
	/* Set all the bytes to zero for the addresses. */
	bzero(&servaddr_echo, sizeof(servaddr_echo));
	bzero(&servaddr_daytime, sizeof(servaddr_daytime));
	
	/* Specify the family, address and port number. */
	servaddr_echo.sin_family      = AF_INET;
	servaddr_echo.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr_echo.sin_port        = htons(15794);	/* echo server */

	servaddr_daytime.sin_family      = AF_INET;
	servaddr_daytime.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr_daytime.sin_port        = htons(15793);	/* daytime server */

	
	/* Bind the sockets to the address. */
	Bind(listenecho, (SA *) &servaddr_echo, sizeof(servaddr_echo));
	
	Bind(listendaytime, (SA *) &servaddr_daytime, sizeof(servaddr_daytime));
	
	/* Set the sockets to non-blocking using fcntl. */
	if(echoFlag = fcntl(listenecho,F_GETFL,0) == -1){
		perror("fcntl F_GETFL");
		exit(1);
	}
	if(fcntl(listenecho,F_SETFL,echoFlag|O_NONBLOCK) ==-1){
		perror("fcntl F_SETFL, O_NONBLOCK");
		exit(1);
	}
	if(timeFlag = fcntl(listendaytime,F_GETFL,0) == -1){
		perror("fcntl F_GETFL");
		exit(1);
	}
	if(fcntl(listendaytime,F_SETFL,timeFlag|O_NONBLOCK) ==-1){
		perror("fcntl F_SETFL, O_NONBLOCK");
		exit(1);
	}
	
	/* Make the sockets listening. */
	Listen(listenecho, LISTENQ);
	Listen(listendaytime,LISTENQ);
	
	/* To specify in the select. */
	maxfd = max(listenecho,listendaytime)+1;
	/* Initialize the set: all bits off.*/	
	FD_ZERO(&allset);
	/* Turn on bits for listenecho and listendaytime. */
	FD_SET(listenecho,&allset);
	FD_SET(listendaytime,&allset);
	
	/* Ignore the SIGPIPE Signal. */
	Signal(SIGPIPE,SIG_IGN);
	for ( ; ; ) {
		rset = allset;
		/* Set select on the two sockets. */
		nready = Select(maxfd+1,&rset,NULL,NULL,NULL);
		
		/* Handles the daytime part. */
		if(FD_ISSET(listendaytime,&rset)){
			clilen = sizeof(cliaddr);
			connfd = Malloc(sizeof(int));
			/* Accept the Connection. */
			*connfd = Accept(listendaytime, (SA *) &cliaddr, &clilen);
			/* Make the Accept blocking. */
			if(fcntl(*connfd,F_SETFL,timeFlag & (~O_NONBLOCK))<0)
				err_sys("F_SETFL error");
			/* Create the Thread. */
			Pthread_create(&tid_time,NULL,&daytime,connfd);
			//FD_CLR(listendaytime,&rset);
		}
		
		/* Handles the echo part. */
		if(FD_ISSET(listenecho,&rset)){
			clilen = sizeof(cliaddr);
			connfd = Malloc(sizeof(int));
			/* Accept the Connection. */
			*connfd = Accept(listenecho, (SA *) &cliaddr, &clilen);
			/* Make the Accept blocking. */
			if(fcntl(*connfd,F_SETFL,echoFlag & (~O_NONBLOCK))<0)
				err_sys("F_SETFL error");
			/* Create the Thread. */		
			Pthread_create(&tid_echo,NULL,&echo,connfd);
			//FD_CLR(listenecho,&rset);
		}
	}
}

/* Handles the thread function by detaching the thread and calling another function on socket*/
static void * echo(void *arg){
	int connfd;
	connfd = *((int *)arg);
	/* Detach the thread from self. */
	Pthread_detach(pthread_self());
	/* Call the echo server function with connection socket. */ 
	echo_srv(connfd);
	Close(connfd);
	return NULL;
}


/* Handles the thread function by detaching the thread and calling another function on socket*/
static void* daytime(void * arg){
	int connfd;
	connfd = *((int*)arg);
	/* Detach the thread from self. */
	Pthread_detach(pthread_self());
	/* Call the echo server function with connection socket. */ 
	day_srv(connfd);
	Close(connfd);
	return NULL;
}



