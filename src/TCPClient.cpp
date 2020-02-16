#include "TCPClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <iostream>
#include <getopt.h>
#include "strfuncts.h"
#include <limits>
#include <chrono>
#include <thread>


//NOTE: Prepoccesor directiveS above end with a newline.. inserting comments or spaces seems
//to mess things up... but that MIGHT be a VS CODE ism thing!!!

/**********************************************************************************************
 * TCPClient (constructor) - Creates a Stdin file descriptor to simplify handling of user input. 
 *
 **********************************************************************************************/

TCPClient::TCPClient() {
    bzero(buffer,sizeof(1025));
}

/**********************************************************************************************
 * TCPClient (destructor) - No cleanup right now
 *
 **********************************************************************************************/

TCPClient::~TCPClient() {
}

// SEE https://linux.die.net/man/3/perror
//When a system call fails, it usually returns -1 and sets the variable errno to a value 
//describing what went wrong. (These values can be found in <errno.h>.) Many library functions 
//do likewise. The function perror() serves to translate this error code into human-readable form. 
//Note that errno is undefined after a successful library call
void TCPClient::error(const char *msg)
{
    perror(msg);
    exit(0);
}

/**********************************************************************************************
 * connectTo - Opens a File Descriptor socket to the IP address and port given in the
 *             parameters using a TCP connection.
 *
 *    Throws: socket_error exception if failed. socket_error is a child class of runtime_error
 **********************************************************************************************/


void TCPClient::connectTo(const char *ip_addr, unsigned short port) {
    
    //SET THE PORT NUMBER ENTERED IN BY THE USER
    portno = port;

    //CREATE A TCP SOCKET
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) //RETURNS -1 IF FAILED TO CREATE A SOCKET
        error("ERROR opening socket");
    
    //ZERO OUT THE STRUCT SOCKADDR_IN
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //SET TO IPV4
    
    //THIS WAS THE OLD CODE, YOU USE '->' TO ACCESS METHODS/FUNCTIONS OF POINTERS
    //bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    //I REPLACED THE CODE WITH THIS SO THAT THE USER CAN PASS IN THE IP ADDRESS FOR THE SERVER
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);

    //set the port number, put in network format with htons()
    serv_addr.sin_port = htons(portno);

    //connct to the socket
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        error("ERROR connecting");
    } else {
        std::cout << "connected to server..\n";
    }
}

/**********************************************************************************************
 * handleConnection - Performs a loop that checks if the connection is still open, then 
 *                    looks for user input and sends it if available. Finally, looks for data
 *                    on the socket and sends it.
 * 
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::handleConnection() {
    
   // This data type (explained here: https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html)
   // is a bit array
   fd_set readfds;//, writefds - I don't need writefds per Lt Col Noel; 
   
   //For Nonblocking I use the struct timeval
   struct timeval timeout;
   int connected = 1;
   while(connected == 1) 
	{ 
        //zero out the buffer before reading or writing
        bzero(buffer, sizeof(buffer)); 
        
        //neds to be in while loop as it got cleared... overwritten somehow
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;

		//clear the socket set 
		FD_ZERO(&readfds); 
        //FD_ZERO(&writefds); unnecessary
	
		//add socket to set 
		FD_SET(sockfd, &readfds); //comes back with the same value
        FD_SET(STDIN_FILENO, &readfds);
        //FD_SET(sockfd, &writefds); // I THOUGHT ONE WOULD TELL ME IF FILEDESCRIPTOR IS READY FOR READ VS WRITE
	
		//create a timeval and put the numbers I want in there which are 0 sec and 10microsend
        //the last parameter is a timeout... could put 10ms and it would not be blocking
		activity = select( sockfd + 1, &readfds , NULL , NULL , &timeout); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
        
        // activity returns a File Descriptor from server to client, is it the server socket?
        if (FD_ISSET(sockfd, &readfds)){
            if ( read(sockfd, buffer, sizeof(buffer)) != 0) {
            /*****IT TOOK ME ONE HOUR TO FIGURE OUT I NEEDED std::endl!****/
            std::cout << buffer << std::endl; //AND I DONT KNOW WHY
            /*****IT TOOK ME ONE HOUR TO FIGURE OUT I NEEDED std::endl!****/
            } else {
                std::cout << "Problem with server connection.\nIssuing client disconnect.\n";
                connected = 0;
            }
        }    
                
        //Read from STDIN, loop over STDIN until everyting is read, do client side checking
        //and write to the socket to send a message to the server
        if (FD_ISSET(STDIN_FILENO, &readfds)) { //FileDescriptor 0 is STDIN
		
			std::string inputbuf;
			size_t sz = 0;
			while ((sz = read(STDIN_FILENO, buffer, 1024)) == 1024) {
				if (sz == 1024) {
					buffer[1024] = 0;
				}
				inputbuf += buffer;
			}				
            inputbuf += buffer;
            
            //convert userinput to all lowercase with Lt Col Noels function
            lower(inputbuf);
			
            //TEST FOR BUFFER OVERFLOW FIRST
            if(inputbuf.size() > 1025) {  //remember buffer is set to 1025
            std::cout << "\n\nAre you trying to fill my buffer? How Rude!\nType 'menu' to see the menu again." << std::endl;    
            bzero(buffer, strlen(buffer));
            //std::cin.clear();  //THIS DIDN'T WORK; LT COL NOELS LOOPING TO COLLECT ALL USERINPUT WORKED
            //std::cin.sync();  // THIS DIDN'T WORK
            //ELSE TEST FOR EXIT 
            } else if (strncmp("exit", inputbuf.c_str(), 4) == 0) { 
            connected = 0; //RELY ON SERVER TO SEVER CONNECTION
            write(sockfd, "exit", 4);
            } else if (strncmp("1", inputbuf.c_str(), 1) == 0) { 
            write(sockfd, buffer, sizeof(buffer));
            } else if (strncmp("2", inputbuf.c_str(), 1) == 0) { 
            write(sockfd, buffer, sizeof(buffer));
            } else if (strncmp("3", inputbuf.c_str(), 1) == 0) { 
            write(sockfd, buffer, sizeof(buffer));
            } else if (strncmp("4", inputbuf.c_str(), 1) == 0) { 
            write(sockfd, buffer, sizeof(buffer));
            } else if (strncmp("5", inputbuf.c_str(), 1) == 0) { 
            write(sockfd, buffer, sizeof(buffer));
            } else if (strncmp("hello", inputbuf.c_str(), 5) == 0) { 
            write(sockfd, buffer, sizeof(buffer));
            } else if (strncmp("passwd", inputbuf.c_str(), 6) == 0) { 
            write(sockfd, buffer, sizeof(buffer));
            }else if (strncmp("menu", inputbuf.c_str(), 4) == 0) { 
            write(sockfd, buffer, sizeof(buffer));
            } else { //INVALID INPUT DON'T SEND
                std::cout << "INVALID INPUT.\n";
                bzero(buffer, sizeof(buffer));
                write(sockfd, "menu", 4);
            }
            bzero(buffer, sizeof(buffer));
            
        }
        //zero out the buffer
        bzero(buffer, sizeof(buffer));     
    }  
}


/**********************************************************************************************
 * closeConnection - close returns zero on success, a -1 on failure
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::closeConn() {
    //give the server 2 seconds to close the connection first
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    
    //check for one last read from server to collects its goodbye message
    if ( read(sockfd, buffer, sizeof(buffer)) != 0) {
    std::cout << buffer << std::endl; 
    }
            
    //then close the socket on the client side
    if (close(sockfd) != 0) {
        std::cout << "error occured when closing socket FD";
    }
}

