/*
* Author: Harishkumar Katagal(109915793)
* FileName: day_srv.c
* This program is an implementation of simple time server. Using select the server sleeps for 5 
* seconds and then writes the time to the socket.
*/

#include	"unp.h"
#include<time.h>
#include<sys/wait.h>

/*Function to sleep the server for 5 seconds. */
int readable_timeo(int fd, int sec);
void
day_srv(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];
	char recvline[MAXLINE];
	int read;
	time_t ticks;
	
	for(;;){
			/* Write the current time onto the socket*/
			ticks = time(NULL);
			snprintf(buf,sizeof(buf),"%.24s\r\n",ctime(&ticks));
			Write(sockfd,buf,strlen(buf));
			/*Check if client is terminated or 5 seconds passed. */
			if(read = readable_timeo(sockfd,5)>0){
				/* If client is terminated return with value of 0*/
				if(n = Readline(sockfd,recvline,MAXLINE)==0){
					perror("Time Client termination - socket read returned with value of 0");
					return;
				}
				else if(n<0){
						perror("Time Client termination: socket read returned with value -1");
						return;
				}
			}			
	}
}

/* Function to check if 5 seconds have passed or if client is terminated. */
int readable_timeo(int fd, int sec){
	fd_set rset;
	struct timeval tv;
	FD_ZERO(&rset);
	FD_SET(fd,&rset);
	tv.tv_sec = sec;
	tv.tv_usec = 0;
	return (select(fd+1,&rset,NULL,NULL,&tv));
}



