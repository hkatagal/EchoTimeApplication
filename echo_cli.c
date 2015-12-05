/*
* Author: Harishkumar Katagal(109915793)
* FileName: echo_cli.c
* This program is an implementation of echo client. It takes the IP address of the server as 
* argument. It then connects to the server, reads the data from terminal and sends it to server
* and then reads the echoed data from server.
*/

#include	"unp.h"

/* SIGINT handler. */
void handlerint(int signo);

int
main(int argc, char **argv)
{
	int					sockfd;
	struct sockaddr_in	servaddr;
	char sendline[MAXLINE], recvline[MAXLINE];
	fd_set rset;
	int maxfdp;
	FILE *fp=stdin;
	int pfd1;
	
	/* Get the pipe descripter and use dupe2 to use stderr for the pipe. */
	sscanf(argv[2],"%d",&pfd1);
	dup2(pfd1,2);
	
	Signal(SIGINT,handlerint);
	
	/* Checks if the number of arguments passed to the function is three*/
	if (argc != 3){
		perror("Echo Child Error - Usage: Specify <IPaddress>");
		exit(0);
	}
		
	/* Create a scocket for connection. */
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Echo Child Error - Socket Creation Error");
		exit(0);
	}
		
	/* Set all the bytes to zero for the addresses. */
	bzero(&servaddr, sizeof(servaddr));
	
	/* Specify the family and port number. */
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(15794);	/* echo server port number. */
	
	/* Convert the string ip address to binary. */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
		perror("Echo Child Error - Inet_pton error");
		exit(0);
	}
		
	/* Connect to the server. */
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
		perror("Echo Child Error - Socket Connection Error");
		exit(0);
	}
	/* Initialize the set: all bits off.*/	
	FD_ZERO(&rset);
	
	for(;;){
		/* Turn on bits for socket and stdin. */
		FD_SET(fileno(fp),&rset);
		FD_SET(sockfd,&rset);
		/* To specify in the select. */
		maxfdp = max(fileno(fp),sockfd) +1;
			/* Set select on the two descripters. */
		Select(maxfdp,&rset,NULL,NULL,NULL);
		/*If data is ready on the socket. */
		if(FD_ISSET(sockfd,&rset)){
			if(Readline(sockfd,recvline,MAXLINE)==0){
				perror("Echo Child Error - Server Terminated Prematurely");
				exit(0);
			}
			Fputs(recvline,stdout);
		}
		/*If data is entered on the stdin*/
		if(FD_ISSET(fileno(fp),&rset)){
			if(Fgets(sendline,MAXLINE,fp)==NULL){
				perror("Echo Child - Connection closed");
				exit(0);
			}				
			Writen(sockfd,sendline,strlen(sendline));
		}
	}
	exit(0);
}


void handlerint(int signo){
	perror("Echo child - Connection Closed.");
	exit(0);
}
