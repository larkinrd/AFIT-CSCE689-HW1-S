#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string> 
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <iostream>
#include <getopt.h>
#include <errno.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include "TCPServer.h"
//LARKIN ADDITIONS
#include <thread>
#include <chrono>
#include "strfuncts.h"

//#define TRUE 1 moved to header
//#define FALSE 0 moved to header file

using namespace std;

/***************** SOCKET OVERVIEW
 * Server Socket from https://www.bogotobogo.com/cplusplus/sockets_server_client.php
 * 1. create a socket - Get the file descriptor!
 * 2. bind to an address -What port am I on?
 * 3. listen on a port, and wait for a connection to be established.
 * 4. accept the connection from a client.
 * 5. send/recv - the same way we read and write for a file.
 * 6. shutdown to end read/write.
 * 7. close to releases data.
 * ***************/

TCPServer::TCPServer() {

   //max_clients is set in the HEADER FILE and is set to 30
   //initialise array client_socket[] to store n slots for sockets up to max_clients
   for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
   //initilize clase wide string variable
   message = "\nWelcome to the CSCE 689 Server\n\n===================================";
   //zero out the send/receive buffer between server and client
   bzero(buffer, sizeof(buffer));
}


TCPServer::~TCPServer() {

}

/**********************************************************************************************
 * bindSvr - Creates a network socket and sets it nonblocking so we can loop through looking for
 *           data. Then binds it to the ip address and port
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 * 1. Create a socket with the socket() system call.
 * 2. The server process gives the socket a name. In linux file system, local sockets are given 
 * a filename, under /tmp or /usr/tmp directory. For network sockets, the filename will be a 
 * service identifier, port number, to which the clients can make connection. This identifier 
 * allows to route incoming connections (which has that the port number) to connect server 
 * process. A socket is named using bind() system call.
 **********************************************************************************************/

//SEE TCP Client for more information on perror() inside this TCPServer error() function
void TCPServer::error(const char *msg)
{
    perror(msg);
    exit(1);
}

void TCPServer::BindServer(const char *ip_addr, short unsigned int port) {

  	//create a master socket which will MOST LIKELY be socket number//file descriptor 3
   //since 0 = Standard In; 1 = Standard out; and 2 = Standard error
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	//create IPv4 type of socket 
	_address.sin_family = AF_INET; 
	
    //REPLACE INADDR_ANY which grabs this servers IP address
    //address.sin_addr.s_addr = INADDR_ANY; 
    //TO 
    _address.sin_addr.s_addr = inet_addr(ip_addr);
    //SET THE PORT NUMBER WITH THE ONE PASSED IN FROM COMMAND LINE ARGS[]
    //and convert it to network byte order
	_address.sin_port = htons( port ); 
		
	//bind the socket to localhost port passed in as an argument
   //the default IP was 127.0.0.1 and default port is 9999 
	if (bind(master_socket, (struct sockaddr *)&_address, sizeof(_address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 

    /*******************************/
    // inet_aton() converts the Internet host address cp from the IPv4
    //   numbers-and-dots notation into binary form (in network byte order)
    //   and stores it in the structure that inp points to. 
     // bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
     // bind() passes file descriptor, the address structure, 
     // and the length of the address structure
     // This bind() call will bind  the socket to the current IP address on port, portno
     //if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
     //         error("ERROR on binding");
     /*******************************/
   
}

/**********************************************************************************************
 * listenSvr - Performs a loop to look for connections and create TCPConn objects to handle
 *             them. Also loops through the list of connections and handles data received and
 *             sending of data. 
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 * 
 * 3. The server process then waits for a client to connect to the named socket, which is basically 
 * listening for connections with the listen() system call. If there are more than one client are 
 * trying to make connections, the listen() system call make a queue. The machine receiving the 
 * connection (the server) must bind its socket object to a known port number. A port is a 16-bit 
 * number in the range 0-65535 that's managed by the operating system and used by clients to uniquely 
 * identify servers. Ports 0-1023 are reserved by the system and used by common network protocols.
 **********************************************************************************************/

void TCPServer::ListenServer() {

     /*******************************/
     // This listen() call tells the socket to listen to the incoming connections.
     // The listen() function places all incoming connection into a backlog queue
     // until accept() call accepts the connection.
     // Here, we set the maximum size for the backlog queue to 5.
     //listen(sockfd,5);
     /*******************************/
    
    //try to specify maximum of 3 pending connections for the master socket 
	if (listen(master_socket, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 

     
	
}

/***************
 * 5. Send and receive data.
 * 6. The named socket remains for further connections from other clients. A typical web server 
 * can take advantage of multiple connections. In other words, it can serve pages to many clients 
 * at once. But for a simple server, further clients wait on the listen queue until the server is ready again.
 * ************/

void TCPServer::HandleAcceptedObjects(/*socket_object, socket_id*/) {
    
/*************MY NOTES******************/
    /***** Accept a connection with the accept() system call, after accept() completes, a new socket is 
 * created that is distinct from the named socket. This new socket is used solely for communication with this 
 * particular client. For TCP servers, the socket object used to receive connections IS NOT THE SAME
 * socket used to perform subsequent communication with the client. In particular, the accept() 
 * system call returns a new socket object that's actually used for the connection. This allows a server 
 * to manage connections from a large number of clients simultaneously.
 * **********/
     // This accept() function will write the connecting client's address info 
     // into the the address structure and the size of that structure is clilen.
     // The accept() returns a new socket file descriptor for the accepted connection.
     // So, the original socket file descriptor can continue to be used 
     // for accepting new connections while the new socker file descriptor is used for
     // communicating with the connected client.
     //newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     
	
   //For Nonblocking functionality, I need an object to hold a set of file descitpors and
   //I need a struct timeval to pass as a parameter to select() function so that the
   //program doesn't just sit their waiting for activity on the socket
   
   
   //get the size/length of the sockaddr_in structure
   //per instruction from Col Noel, many different types of connections use
   //the sock_addr in structure such as IPv4 or IPv6 or TCP or UDP... the compuer
   //can diferentiate between the different combinations based on the size of certain
   //pieces and parts in the sockaddr_in structure. 
/*************END MY NOTES******************/
   
   fd_set readfds; 
   struct timeval timeout; 
   addrlen = sizeof(_address); 
   
   while(TRUE) {
	 
		timeout.tv_sec = 0; //zero seconds
      timeout.tv_usec = 100;//100 microseconds, maybe this could be 10, but the program works
      
      //clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
			
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			//socket descriptor 
			sd = client_socket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) {
				FD_SET( sd , &readfds); 
   			    //highest file descriptor number, need it for the select function 
			    if(sd > max_sd) 
				   max_sd = sd; 
			}
				
		} 
	
		//check to see if there is activity on a socket, wait for only 100 microseconds
      //NOTE the last parameter was NULL which meant wait forever for activity
      //when changed to 100microseconds, this becomes a nonblocking operation		
		activity = select( max_sd + 1, &readfds , NULL , NULL , &timeout); //the last parameter is a timeout... could put 10ms and it would not be blocking
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
			
		//If something happened on the master socket, then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&_address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			
			//print to server console information about the new connection
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n", 
                new_socket , inet_ntoa(_address.sin_addr) , ntohs(_address.sin_port)); 
		
			//send new connection greeting message
			if( write(new_socket, message.c_str(), message.size()) != message.size() ) 
			{ 
				perror("did not write full message to socket"); 
			} else {
            sendMenu(new_socket);
			   puts("Welcome message sent successfully"); 
         }
				
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty, add new socket to the first empty position 
				if( client_socket[i] == 0 ) 
				{ 
					client_socket[i] = new_socket; 
					printf("Storing in socket position %d\n" , i); 
					break; 
				} 
			} 
		} 
			
		//If nothing triggered on the MASTER socket, its some other IO operation on some other socket
      //LOOP to figure out which socket 
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i]; 
				
			if (FD_ISSET( sd , &readfds)) 
			{ 
            int numbytesread = 0;
            //read from the buffer, processes client input, and send a response to client
            if((numbytesread = read( sd , buffer, 1025)) > 0) {
               
               ProcessClientResponse(sd, buffer);

               //if exit was selected... finish processing the server side client disconnect here
               //by printing information about the closing socket to the console
               if (strncmp("exit", buffer, 4) == 0) { 
                  getpeername(sd , (struct sockaddr*)&_address , (socklen_t*)&addrlen);
                  printf("Host ip %s on port %d with socket fd %d in socket array position %d disconnected.\n", 
                     inet_ntoa(_address.sin_addr) , ntohs(_address.sin_port), sd, i); 
                  close( sd ); 
                  client_socket[i] = 0; 
               }
               
            }
            //after all operations, zero out the buffer
            bzero(buffer, sizeof(buffer));
			} 
		} 
	}

}



/**********************************************************************************************
 * shutdown - Cleanly closes the socket FD.
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPServer::ShutdownServer() {

     close(master_socket);
     
}

/**********************************************************************************************
 * sendMenu - sends the menu to the user via their socket
 *
 *    Throws: runtime_error for unrecoverable issues
 **********************************************************************************************/
void TCPServer::sendMenu(int socketFD) {
   
   std::string menustr;

   // Make this your own! - OK
   menustr += "\nAvailable choices: \n\n";
   menustr += "  1 - Tell you Industrial Control System (ICS) terminology.\n";
   menustr += "  2 - Tell you my favorite movie?\n";
   menustr += "  3 - Tell you my favorite ps -auquote?\n";
   menustr += "  4 - What is 2 + 2?\n";
   menustr += "  5 - Tell you my favorite N64 Video Game\n\n";
   menustr += "Other commands: \n";
   menustr += "  hello - a personal greeting from Spaz's insecure server\n";
   menustr += "  passwd - change your password\n";
   menustr += "  menu - display this menu\n";
   menustr += "  exit - disconnect.\n\nNOTE: 3 seconds are given to READ response from server\nEnter Your Choice Below: ";
   
   if( write(socketFD, menustr.c_str(), menustr.size()) != menustr.size() ){
      std::cout << "DID NOT write() full menustr in sendMenu(int socketFD)\n";
   }    
}

/**********************************************************************************************
 * getMenuChoice - Gets the user's command and interprets it, calling the appropriate function
 *                 if required.
 *
 *    Throws: runtime_error for unrecoverable issues
 **********************************************************************************************/

void TCPServer::ProcessClientResponse(int socketFD, char *userinput) {

   //using the passed in socket and user input, convert to std::string, do comparisons, and generate
   //appropriate reponse. Except for exit (which breaks out of this funciton and returns), each option
   //sends a response to the client, initiates a 3 second delay so client can read the response, and then
   //the server resends the menu to the client
   std::string passedinput = userinput; 
   clrNewlines(passedinput);
   lower(passedinput);      
   
   std::string msg;
   if (passedinput.compare("hello") == 0) {
	   msg = "\nWhat.. I already gave you a welcome greeting. Pick another option.\n";
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("menu") == 0) {
	   msg = ""; //write a blank message and basically DO NOTHING
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("exit") == 0) {
	  msg = "Disconnecting...goodbye!\n";
	  write(socketFD, msg.c_str(), msg.size());
     return; //RETURN and DO NOT send the menu as the last instruction in this function
   } else if (passedinput.compare("passwd") == 0) {
      msg += "\nPartially coded up in my HW2\n";
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("1") == 0) {
      msg += "\nI recommend you look up the terms Industrial Internet of Things (IIoT), Operational Technoglogy (OT) networks "
             "Information Technology (IT) networks, business networks, and Industray 4.0 to name a few.\n";
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("2") == 0) {
	  msg = "\nJurassic Park\n";
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("3") == 0) {
      msg = "\nThe only thing necessary for the triumph of evil is that good men do nothing.\n";
      write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("4") == 0) {
      msg = "\nHa, I think AFIT has a math class where you prove that? or maybe disprove it? I don't know....\n";
      write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("5") == 0) {
      msg = "\nF-ZEROX\n";
      write(socketFD, msg.c_str(), msg.size());
   } else {
      msg = "\nUnrecognized command: ";
      msg += userinput;
      msg += "\n";
	  write(socketFD, msg.c_str(), msg.size());
     sendMenu(socketFD);
   }
   //display counted for n seconds to server console screen
   //FUNCTION found in strfuncts.cpp
   displayCountdown(3); 
   sendMenu(socketFD);  
}