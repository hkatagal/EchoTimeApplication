/*
* Author: Harishkumar Katagal(109915793)
* FileName: tcpechotimecli.c
* This program is an implementation of client function. It accepts either IP Address or server 
* domain as arguments. And then queries the user to select the service they want to choose. Its 
* either echo or time.
*/


#include	"unp.h"
#include<stdio.h>

/* Signal Handler.*/
void sig_child(int signo);

int
main(int argc, char **argv)
{
	pid_t 	echo_child,time_child;
	char service[20];
	int pfd[2],pfdt[2];
	int pid;
	int r,rt;
	char pfd1[MAXLINE];
	char buf[MAXLINE];
	struct hostent *hptr;
	char str[INET_ADDRSTRLEN];
	char **pptr;
	const char *str1;
	struct in_addr addr;
	char stdget[MAXLINE];
	int lenIp=0;
	int i;
	int nameFlag = 0;
	int sel;
	
	fd_set rset;
	int maxfdp;
	/*If no IP address or domain name is specified. */
	if(argc<2){
		printf("Specify IP Address/Domain name.\n");
		exit(0);
	}
	/* Set the signal handler. */
	Signal(SIGCHLD,sig_child);
	
	lenIp = strlen(argv[1]);
	/* To find if the argument passed is ip address or domain name. */
	for(i=0;i<lenIp;i++){
		if(isdigit(argv[1][i]) || (argv[1][i] == '.')){
			continue;
		}
		else{
			nameFlag = 1;
			break;
		}
	}
	/* If domain name is passed get the IP of corresponding address */
	if(nameFlag == 1){
		if((hptr = gethostbyname(argv[1]))==NULL){
			perror("Error: Invalid host name. Terminating.");
			exit(0);
		}
		pptr = hptr->h_addr_list;
		str1 = Inet_ntop(hptr->h_addrtype,*pptr,str,sizeof(str));
		printf("The server IP is: %s\n",str1);
	}
	/* If IP address is passed get the domain name of corresponding IP. */
	else{		
		inet_pton(AF_INET,argv[1],&addr);
		if((hptr = gethostbyaddr(&addr,sizeof(addr),AF_INET))==NULL){
			perror("Error: Invalid IP address. Terminating.");
			exit(0);
		}
		pptr = hptr->h_addr_list;
		str1 = Inet_ntop(hptr->h_addrtype,*pptr,str,sizeof(str));
		printf("The server host is : %s\n",hptr->h_name);
	}

	/* Initialize the set: all bits off.*/	
	FD_ZERO(&rset);
	for(;;){
		printf("Enter which service that you want to request? echo/time\n");
		scanf("%s",service);
		/* If service type requested is echo. */
		if(strcmp(service,"echo")==0){
				printf("\n");
				/* Establish the pipe for communication. */
				if(pipe(pfd)==-1){
					perror("Pipe failed\n");
					exit(1);
				}
				/* Start a new echo child process and execlp it. */
				if((echo_child =Fork())==0){
					close(pfd[0]);
					snprintf(pfd1,sizeof(pfd1),"%ld",pfd[1]);
					if((execlp("xterm","xterm","-e","./echo_cli",str1,pfd1,(char *)0))<0){
						perror("Exec Error!\n");
					}
					close(pfd[1]);
				}
				/* At the parent process. */
				if(echo_child>0){
					/* Close one end of the pipe.*/
					close(pfd[1]);
					printf("Echo Client created with process id %d created.\n",echo_child);
					/* Start listening in the pipe when pipe is broken come out of loop. */
					for(;;){
						FD_ZERO(&rset);
						FD_SET(pfd[0],&rset);
						FD_SET(0,&rset);
						maxfdp = max(pfd[0],0) +1;
		
						if(sel = select(maxfdp,&rset,NULL,NULL,NULL)<0){
							if(errno == EINTR)
								continue;
							else{
								perror("Select error on pipe and stdin\n");
								break;
							}	
						}
						if(FD_ISSET(pfd[0],&rset)){
							if(r = read(pfd[0],buf,MAXLINE)>0){
								printf("%s\n",buf);
							}
							else if(r<0 && errno ==EINTR)
								continue;
							else if(r<=0){
								break;
							}
						}
						if(FD_ISSET(0,&rset)){
							if(r=fgets(stdget,MAXLINE,stdin)!=NULL){
							
								printf("Wrong window! Use the xterm window to communicate.\n");
							}
							else if(r<0 && errno ==EINTR)
								continue;
							else if(r<0)
								break;
						}
					}
					close(pfd[0]);
				}
		}
		/* If service type requested is time. */
		else if(strcmp(service,"time")==0){
					printf("\n");
					/* Establish the pipe for communication. */
					if(pipe(pfd)==-1){
						perror("Pipe failed\n");
						exit(1);
					}
					/* Start a new echo child process and execlp it. */
					if((time_child =Fork())==0){
							close(pfd[0]);
							snprintf(pfd1,sizeof(pfd1),"%ld",pfd[1]);
							if((execlp("xterm","xterm","-e","./time_cli",str1,pfd1,(char *)0))<0){
								perror("Exec Error!");
							}						
					}
					/* At the parent process. */
					if(time_child>0){
						/* Close one end of the pipe.*/
						close(pfd[1]);
						printf("Time Client created with process id %d created\n",time_child);
						FD_ZERO(&rset);
						/* Start listening in the pipe when pipe is broken come out of loop. */
						for(;;){
								/*if(r = read(pfd[0],buf,MAXLINE)!=0){
									printf("%s\n",buf);
								}
								else if(r==0)
								{
									break;	
								}*/
							
							FD_SET(pfd[0],&rset);
							FD_SET(0,&rset);
							maxfdp = max(pfd[0],0) +1;
			
							if(sel = select(maxfdp,&rset,NULL,NULL,NULL)<0){
								if(errno == EINTR)
									continue;
								else{
									perror("Select error on pipe and stdin\n");
									break;
								}	
							}
							if(FD_ISSET(pfd[0],&rset)){
								if(r = read(pfd[0],buf,MAXLINE)>0){
									printf("%s\n",buf);
								}
								else if(r<0 && errno ==EINTR)
									continue;
								else if(r<=0){
									break;
								}
							}
							if(FD_ISSET(0,&rset)){
								if(r=fgets(stdget,MAXLINE,stdin)!=NULL){
									printf("Wrong window! Use the xterm window to communicate.\n");
								}
								else if(r<0 && errno ==EINTR)
									continue;
								else if(r<0)
									break;
							}
								
								
						}
						close(pfd[0]);
					}
		}
		/* If the service is to be terminated. */
		else if(strcmp(service,"quit") == 0 ||strcmp(service,"exit") == 0  ){
					printf("Quiting\n");
					exit(0);
		}
		else{
			printf("Invalid Input\n");
		}
	}
}

/* Signal Child handler*/
void sig_child(int signo){
	pid_t pid;
	int stat;
	while((pid=waitpid(-1,&stat,WNOHANG))>0)
		err_msg("Child %d terminated",pid);
	return;
}
