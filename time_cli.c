/*
* Author: Harishkumar Katagal(109915793)
* FileName: time_cli.c
* This program is an implementation of time client. It takes the IP address of the server as 
* argument. It then connects to the server and reads from the socket and displays it on the 
* xterm. It also sends the status messages to the client function using a pipe.
*/

#include	"unp.h"
/* SIGINT handler to handle CTRL+C. */
void handlerint(int signo);

int
main(int argc, char **argv)
{
	int					sockfd, n;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;
	int pfd2;

	Signal(SIGINT,handlerint);
	
	/* Get the pipe descripter and use dupe2 to use stderr for the pipe. */
	sscanf(argv[2],"%d",&pfd2);
	dup2(pfd2,2);
	//Signal(SIGCHLD,sig_child);
	
	/* Checks if the number of arguments passed to the function is three*/
	if (argc != 3){
		perror("Time Child Error - Usage: Specify <IPaddress>");
		exit(0);
	}
	
	/* Create a scocket for connection. */
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Time Child Error - Socket Creation Error");
		exit(0);
	}
		
	/* Set all the bytes to zero for the addresses. */
	bzero(&servaddr, sizeof(servaddr));
	
	/* Specify the family and port number. */
	servaddr.sin_family = AF_INET;	
	servaddr.sin_port   = htons(15793);	/* daytime server port number. */
	
	/* Convert the string ip address to binary. */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
		perror("Time Child Error - Inet_pton error");
		exit(0);
	}
	/* Connect to the server. */
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
		perror("Time Child Error - Socket Connection Error");
		exit(0);
	}
	
	/* Continuosly read the time and display it on the xterm window. */
	while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
		/* If EOF character is detected report Fputs error*/
		if (fputs(recvline, stdout) == EOF){
			perror("Time Child Error - Fputs error to stdout");
			exit(0);
		}
			
	}
	if (n < 0){
		perror("Time Child Error - Read Error");
		exit(0);
	}
	exit(0);
}

/* SIGINT handler to handle CTRL+C. */
void handlerint(int signo){
	perror("Time child - Connection Closed.");
	exit(0);
}
