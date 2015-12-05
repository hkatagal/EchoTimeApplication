/*
* Author: Harishkumar Katagal(109915793)
* FileName: echo_srv.c
* This program is an implementation of simple echo server.
*/

#include	"unp.h"

void
echo_srv(int sockfd)
{
	/* Specify Buffer sizes. */
	ssize_t		n;
	char		buf[MAXLINE];

	/* Read the socket continously. */
	again:
		/*If read size is greater than 0 write the same onto socket. */
		while ( (n = read(sockfd, buf, MAXLINE)) > 0)
			Writen(sockfd, buf, n);
		
		/*If n<0 and error type is EINTR continue. */
		if (n < 0 && errno == EINTR)
			goto again;
		/* If the read returns 0 then socket is closed. */
		else if(n==0){
			perror("Echo Client termination: socket read returned with value of 0");
		}
		/*If n <0 client has been terminated. */
		else if (n < 0){
				perror("Client termination");
		}
}
