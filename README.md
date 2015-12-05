# Network-Programing---A1
USER DOCUMENTATION
Following are the files that contain the source codes for the assignment 1.
Server Side
	1. tcpechotimesrv.c
	2. day_srv.c
	3. echo_srv.c

Client Side
	1. tcpechotimecli.c
	2. time_cli.c
	3. echo_cli.c

To compile and generate executables use the "Makefile" and issue a "make". This will generate the *.o and the executable files. The two main important files that we want are "server" and "client". Use these two files to run the server and client respectively. To run the server use "./server" command, this will starts the server and creates two listening sockets on port numbers 15794 and 15793. To run the client we must specify the IP address/domain name of the server as arguments along with the "./client". Once the client is started it asks the user to enter service that it wants to request that is either echo or time. If we enter "echo", a new xterm window is started to service the request. This is same for "time" as well. You can exit the xterm window by either CTRL+C or CTRL+D(time). Some sample examples are shown below.

Ex1:   	./server
		./client localhost
	   
Ex2:	./server
		./client 127.0.0.1

Ex3:	./server
		./client compserv1.cs.stonybrook.edu
**************************************************************************************************
		
SYSTEM DOCUMENTATION
The system documentation is explained separately for client and server below.

1. Client - tcpechotimecli.c
	- Supports two services echo and time.
	- Client is evoked from command line either by passing IP address or the domain name as 	arguments. If IP address is specified the program displays the domain name corresponding to the IP and if domain name is specified it displays the domain name. If no arguments are specified, the user is asked to enter the arguments. The program also verifies whether the arguments specified are valid or not. If not an error message is displayed. 
	- The program then prompts the user to specify the type of service that is needed. Once the user selects the service a new process is created using fork and execlp is used to run the process on a new xterm window. A pipe is also created for communication between the two processes. The echo and time processes are explained below.
	- Signal Handler is used to to handle the SIGCHLD signal. The child process is terminated and status message is sent by the signal handler.
	- It also uses select on the pipe and stdin to check if the user is communicating on the wrong window, if the user enters something on this window rather than the xterm window, a message is displayed. 
	- The pipe continually reads the status messages sent from the child processes and displays it on the window. 
	- Once the child process terminates the program then goes back to query the user for the service to be selected. 
	
	a. Echo service - echo_cli.c 
		- The echo client establishes a connection with the server at the given IP and port number.
		- It then continually sends and receives messages to and from the server. 
		- Select is used to detect the connection close between the server and client. This is achieved by blocking on the socket and stdin. If user presses CTRL+D, an appropriate message is sent to parent message and the child process is killed.
		- A signal SIGINT handler is used to detect the and close the connection when CTRL+C is pressed. In the handler a message is sent to the parent process and the child process is killed.
		- If the server process is killed, this is detected by the select and the process is killed.
	
	b. Time service - time_cli.c
		- The time establishes a connection with the server at the given IP and port number.
		- It then continually reads the time sent by the server and displays it.
		- A signal SIGINT handler is used to detect the and close the connection when CTRL+C is pressed. In the handler a message is sent to the parent process and the child process is killed.
		- If the server process is killed, this process sends a message to the parent before it is killed.
		
2. Server - tcpechotimesrv.c
	- The server creates two sockets for the two services and listens on both the socket. This implementated by using the select.
	- Before the sockets are made listening sockets, they are made nonblocking using fcntl function. And after the accept the sockets are then made blocking.
	- Once a client connects, a thread is created and detached to handle the request. 
	- Also set socket option is used to make the server sockets reusable.
	- Proper status messages are displayed at the server when a client is terminated. 
	- The two services are discribed below.
	
	a. Echo service - echo_srv.c
		- After the thread is detached from the main thread this function is called to handle the echo service. 
		- It simply reads the data and echos it back to the client.
		- If the client is terminated either by CTRL+C or CTRL+D, the thread is terminated and a status message is displayed on the main thread.
		
	b. Time Service - day_srv.c
		- After the thread is detached from the main thread this function is called to handle the time service.
		- It uses select on socket descriptor to implement the timeout of 5 seconds and also to detect the termination of the client process.
		- After every 5 seconds it sends the time to the client.
		- Once it detects the termination of the client, it then displays a message on the stdout and then terminates the thread.
