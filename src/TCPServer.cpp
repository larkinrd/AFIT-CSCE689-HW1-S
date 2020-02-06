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




/********* MOVING TO CLASS PRIVATE VARIABLE AREA CAUSES THIS COMPILER ERROR
error: field ‘_address’ has incomplete type ‘sockaddr_in’
 **********/////
   //set of socket descriptors 
 /*	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30], max_clients = 2 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in _address; 
    
	//a message 
	//char *message = (char *) "ECHO Daemon v1.0\r\n"; 
   std::string message = "Welcome to the CSCE 689 Server\n\n===================================\n\n";
	char buffer[1025]; //data buffer of 1K
*/
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

//TCPSERVER CLASS VARIABLES



TCPServer::TCPServer() {

    //initialise all client_socket[] to 0 so not checked 
   max_clients = 2;
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
   message = "\nWelcome to the CSCE 689 Server\n\n===================================\n\n";
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

void TCPServer::error(const char *msg)
{
    perror(msg);
    exit(1);
}

void TCPServer::BindServer(const char *ip_addr, short unsigned int port) {

   

     	//create a master socket 
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
		sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	//type of socket created 
	_address.sin_family = AF_INET; 
	_address.sin_addr.s_addr = inet_addr(ip_addr);
    //address.sin_addr.s_addr = INADDR_ANY; 
	_address.sin_port = htons( port ); 
		
	//bind the socket to localhost port passed in as an argument (default is 9999) 
	if (bind(master_socket, (struct sockaddr *)&_address, sizeof(_address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 

     
     
     // create a socket
     // socket(int domain, int type, int protocol)
    // sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd < 0) 
    //    error("ERROR opening socket");

    // clear address structure
    //bzero((char *) &serv_addr, sizeof(serv_addr));

    //portno = port;

     /* setup the host_addr structure for use in bind call */
     // server byte order
    //serv_addr.sin_family = AF_INET;  

    // inet_aton() converts the Internet host address cp from the IPv4
    //   numbers-and-dots notation into binary form (in network byte order)
    //   and stores it in the structure that inp points to. 
    //in_addr_t 
    //inet_aton(ip_addr, serv_addr.sin_addr.s_addr);

    // automatically be filled with current host's IP address
     //serv_addr.sin_addr.s_addr = INADDR_ANY;  //THIS WAS THE DEFAULT CODE FROM WEBSITE

    // fill with IP address of the one passed via command line arguments in server_main.cpp
     //serv_addr.sin_addr.s_addr = inet_addr(ip_addr);    
     
     // convert short integer value for port must be converted into network byte order
     //serv_addr.sin_port = htons(portno);

     // bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
     // bind() passes file descriptor, the address structure, 
     // and the length of the address structure
     // This bind() call will bind  the socket to the current IP address on port, portno
     //if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
     //         error("ERROR on binding");
   
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


//AFTER LISTEN... WHEN YOU DECICDE TO ACCEPT() THE CONNECTION, ACCEPT() RETURNS A SOCKET OBJECT
//I COULD SIMPLY MANAGE 3 SOCKET OBJECTS AND CALL IT GOOD... IE I DEMONSTRATED/LEARNED HOW TO DO THIS

/***** Accept a connection with the accept() system call. At accept(), a new socket is created that is 
 * distinct from the named socket. This new socket is used solely for communication with this 
 * particular client. For TCP servers, the socket object used to receive connections IS NOT THE SAME
 * socket used to perform subsequent communication with the client. In particular, the accept() 
 * system call returns a new socket object that's actually used for the connection. This allows a server 
 * to manage connections from a large number of clients simultaneously.
 * **********/

// PUT ACCEPT() CODE HERE FOR HANDLING SOCKET OBJECTS
     // This listen() call tells the socket to listen to the incoming connections.
     // The listen() function places all incoming connection into a backlog queue
     // until accept() call accepts the connection.
     // Here, we set the maximum size for the backlog queue to 5.
     //listen(sockfd,5);
    
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
    
    //put in infinite loop waiting to handle connections
    //for(;;){
    // The accept() call actually accepts an incoming connection
     //clilen = sizeof(cli_addr);

          // This accept() function will write the connecting client's address info 
     // into the the address structure and the size of that structure is clilen.
     // The accept() returns a new socket file descriptor for the accepted connection.
     // So, the original socket file descriptor can continue to be used 
     // for accepting new connections while the new socker file descriptor is used for
     // communicating with the connected client.
     //newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     
     //if (newsockfd < 0) 
     //     error("ERROR on accept");

     //printf("server: got connection from %s port %d\n",
     //       inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

   std::cout << "243:inhandleobjects" << std::endl;
   
    //accept the incoming connection 
	addrlen = sizeof(_address); 
   //std::cout << "213:Waiting for connections\n";
	fd_set readfds; //only needed within this function
   
   //For Nonblocking I need a struct timeval... or just the two things in the struct??
   struct timeval timeout;
   
   while(TRUE) {
	 
		timeout.tv_sec = 0;
      timeout.tv_usec = 100;
      
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
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		
		//timeval ... long tv_set
		//create a timeval and put the numbers i want in there which are 0 sec and 10microsend or so
		activity = select( max_sd + 1, &readfds , NULL , NULL , &timeout); //the last parameter is a timeout... could put 10ms and it would not be blocking
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
			
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&_address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n", 
                new_socket , inet_ntoa(_address.sin_addr) , ntohs(_address.sin_port)); 
		
			//send new connection greeting message
             
			 //change this to write()
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
				//if position is empty 
				if( client_socket[i] == 0 ) 
				{ 
					client_socket[i] = new_socket; 
					printf("Storing in socket position %d\n" , i); 
						
					break; 
				} 
			} 
		} 
			
		//else its some IO operation on some other socket 
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i]; 
				
			if (FD_ISSET( sd , &readfds)) 
			{ 
				//Check if it was for closing , and also read the 
				//incoming message 
            //I DONT THINK I NEED THIS... I KNOW I NEED TO DO SOMETHING
            //WITH WHATEVER IS ON THE SOCKET

            //READ THE BUFFER AND DO SOMETHING IF THERE ARE MORE
            //THAN ZERO BYTES ON THE BUFFER
            int numbytesread = 0;
            if((numbytesread = read( sd , buffer, 1025)) > 0) {
               //WRITE TO SCREEN
               //std::cout << buffer;// << std::endl;
               
               //Pass input to the getMenuChoice Fucntion 
               //sendMenuReponse(sd, buffer);


               if (strncmp("exit", buffer, 4) == 0) { 
                  getpeername(sd , (struct sockaddr*)&_address , (socklen_t*)&addrlen);
                  printf("Host ip %s on port %d with socket fd %d in socket array position %d disconnected.\n", 
                     inet_ntoa(_address.sin_addr) , ntohs(_address.sin_port), sd, i); 
                  //Close the socket and mark as 0 in list for reuse 
                  close( sd ); 
                  client_socket[i] = 0; 

               }

               //I would get a second processing of a blank "" buffer yieling an
               // unrecognized command after server sends a response
               ProcessClientResponse(sd, buffer);
               sendMenu(new_socket);
            }

            //WRITE BACK TO SOCKET (IE ECHO)
            //write(sd, buffer, sizeof(buffer));
            
            
            
            bzero(buffer, sizeof(buffer));


            /*
            if ((valread = read( sd , buffer, 1024)) < 0) //a -1 is an error
				{ 
					//Somebody disconnected , get his details and print 
					getpeername(sd , (struct sockaddr*)&_address , (socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(_address.sin_addr) , ntohs(_address.sin_port)); 
						
					//Close the socket and mark as 0 in list for reuse 
					close( sd ); 
					client_socket[i] = 0; 
				} else { 
				/*
            //Echo back the message that came in  
                 
                   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';   
                    
                    //THERE IS A DIFFERENCE BETWEEN SEND() AND WRITE()
                    //send(sd , buffer , strlen(buffer) , 0 );   
                    
                    
                    write(sd, buffer, sizeof(buffer));
                
            
            
            	//set the string terminating NULL byte on the end 
					//of the data read 
               
				/*	if (strlen(buffer) > 0){
                     buffer[valread] = '\0'; // read() returned number of bytes read
					      //send(sd , buffer , strlen(buffer) , 0 );
                    std::string tempstring = buffer;
                    getMenuChoice(sd, tempstring);
                    
                    write(sd, buffer, sizeof(buffer));
                    bzero(buffer, sizeof(buffer));  
                    //send(sd, "another", 7, 0);
                    sendMenu(sd);
               //} 
				} */
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
     //close(); // the client sockets should alread be closed OR lopp over and close them
}

/**********************************************************************************************
 * sendMenu - sends the menu to the user via their socket
 *
 *    Throws: runtime_error for unrecoverable issues
 **********************************************************************************************/
void TCPServer::sendMenu(int socketFD) {
   //std::cout << "Enter sendMenu() and DO SOMETHING\n"; 
   std::string menustr;

   // Make this your own!
   menustr += "Available choices: \n";
   menustr += "  1). Tell you Industrial Control System (ICS) terminology.\n";
   menustr += "  2). Tell you my favorite movie?\n";
   menustr += "  3). Tell you my favorite quote?\n";
   menustr += "  4). What is 2 + 2?\n";
   menustr += "  5). Tell you my favorite N64 Video Game\n\n";
   menustr += "Other commands: \n";
   menustr += "  Hello - a personal greeting from Spaz's insecure server\n";
   menustr += "  Passwd - change your password\n";
   menustr += "  Menu - display this menu\n";
   menustr += "  Exit - disconnect.\n\n";

   //std::cout << menustr;
   //send(socketFD, menustr.c_str(), sizeof(menustr), 0);
   write(socketFD, menustr.c_str(), menustr.size()); 
   //_connfd.writeFD(menustr);
   
   //std::cout << "Press Enter Key to Exit sendMenu()\n\n";
   //getchar();   
   
}

/**********************************************************************************************
 * getMenuChoice - Gets the user's command and interprets it, calling the appropriate function
 *                 if required.
 *
 *    Throws: runtime_error for unrecoverable issues
 **********************************************************************************************/

void TCPServer::ProcessClientResponse(int socketFD, char *userinput) {

   //NOTE: std::cout AND printf can be used with CHAR ARRAYS
   //NOTE: You DO NOT ues printf() with STRINGS
   std::cout << "Using std::cout to print CHAR userinput: " << userinput;
   printf("Using printf() to print CHAR userinput: %s", userinput);
   //creating a string and initilizing to value of userinput
   std::string passedinput = userinput; 
   //std::cout << "Using std::cout to print STRING passedinput: " << passedinput;
   clrNewlines(passedinput);
   lower(passedinput);      
   //std::cout << "Using std::cout to print STRING passedinput AFTEROPS: " << passedinput;
   
   // Don't be lazy and use my outputs--make your own! Sir, its 0106... I've been at this since 1730 yesterday 29JAN
   std::string msg;
   if (passedinput.compare("hello") == 0) {
	   msg = "What.. I already gave you a welcome greeting. Pick another option.\n";
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("menu") == 0) {
	   msg = ""; //write a blank message and basically DO NOTHING
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("exit") == 0) {
	  msg = "Disconnecting...goodbye!\n";
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("passwd") == 0) {
      msg += "Partially coded up in my HW2\n";
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("1") == 0) {
      msg += "I recommend you look up the terms Industrial Internet of Things (IIoT), Operational Technoglogy (OT) networks "
             "Information Technology (IT) networks, business networks, and Industray 4.0 to name a few.\n";
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("2") == 0) {
	  msg = "Jurassic Park\n";
	  write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("3") == 0) {
      msg = "The only thing necessary for the triumph of evil is that good men do nothing.\n";
      write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("4") == 0) {
      msg = "Ha, I think AFIT has a math class where you prove that? or maybe disprove it? I don't know....\n";
      write(socketFD, msg.c_str(), msg.size());
   } else if (passedinput.compare("5") == 0) {
      msg = "F-ZEROX\n";
      write(socketFD, msg.c_str(), msg.size());
   } else {
      msg = "Unrecognized command: ";
      msg += userinput;
      msg += "\n";
	  write(socketFD, msg.c_str(), msg.size());
   }
  //displayCountdown(5); //display counted for n seconds to console screen
}




/**********************************************************************************************
 * chat - For chatting over the passed in Socket File Descriptor
 **********************************************************************************************/

void TCPServer::Chat (int socketfdforchattting){

    //std::cout << "chat received" << socketfdforchattting;
    char chatbuffer[256]; 
    int n; // A local n for counting charaters on chatbuffer
        
    for (;;) { 
        bzero(chatbuffer, sizeof(chatbuffer)); 

        // read the message from client and copy it in buffer 
        read(socketfdforchattting, chatbuffer, sizeof(chatbuffer)); 
        // print buffer which contains the client contents 
        //printf("From client: %s\t To client : ", chatbuffer); 
        std::cout << chatbuffer  << std::endl;
        bzero(chatbuffer, sizeof(chatbuffer)); 
        n = 0; 
        // copy server message in the buffer 
        while ((chatbuffer[n++] = getchar()) != '\n') 
            ; 

        // and send that buffer to client 
        write(socketfdforchattting, chatbuffer, sizeof(chatbuffer)); 

        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", chatbuffer, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    }
} 
