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

//NOTE: Prepoccesor directive above end with a newline.. inserting comments can mess things up
//OR and again OR... this is a VS CODE ism thing!!!

/* MOVE TO CLASS Private variables
int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[1025];
*/

/**********************************************************************************************
 * TCPClient (constructor) - Creates a Stdin file descriptor to simplify handling of user input. 
 *
 **********************************************************************************************/

TCPClient::TCPClient() {
    //I THINK I COULD USE THE CONSTRUCTOR TO BUILD MY LOGCLIENT... BUT I WILL JUST PUT IN
    //CLIENT_MAIN.CPP FOR NOW
}

/**********************************************************************************************
 * TCPClient (destructor) - No cleanup right now
 *
 **********************************************************************************************/

TCPClient::~TCPClient() {

}

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
    std::cout << "58:did this execute\n";
    portno = port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    //server = gethostbyname(ip_addr); //NOT USING HOSTNAMES HERE
    //if (server == NULL) {
    //    fprintf(stderr,"ERROR, no such host\n");
    //    exit(0);
    //}
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    
    std::cout << "71:did this execute\n";
    //REPLACED BELOW 
    //bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    //WITH
    //so I could pass in IP Address
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);

    //set the port number, put in network format with htons()
    serv_addr.sin_port = htons(portno);

    //connct to the socket
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
        error("ERROR connecting");
    } else {
        std::cout << "connected to server..\n";
    }

    //Chat(sockfd); //PROGRAM LATER TO RETURN THE SOCKET FD (AN INTEGER)
    //since sockfd now saved as private class variable, i can access it in other functions 
    //and don't need to pass it.
    std::cout << "89:did this execute after Chat(sockfd)\n";
    
    /* ORIGINAL CODE BLOCK WILL BE MOVED AND MODIFIED IN handleConnection()
    printf("Please enter the message: ");
    std::cout << "84:did this execute\n";
    bzero(buffer,1025);
    fgets(buffer,1025,stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,1025);
    n = read(sockfd, buffer, 1025);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n", buffer);
    // Moving this to close connection close(sockfd);
    //return 0; not returning anything
    */

}

/**********************************************************************************************
 * handleConnection - Performs a loop that checks if the connection is still open, then 
 *                    looks for user input and sends it if available. Finally, looks for data
 *                    on the socket and sends it.
 * 
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::handleConnection() {
    
    
    /* OLD CODE... INSERTING CHAT FUNCTION CODE
    //read from server first
    n = read(sockfd, buffer, 1025);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n", buffer);
    
    //send to server next
    printf("Please enter the message: ");
    std::cout << "115:did this execute\n";
    bzero(buffer,1025);
    fgets(buffer,1025,stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,1025);
    */

   
   //Put in logic for if socket has data... like master_socket in TCPServer, but we have a client with only 
   //its socket connection stored in my private variable of TCPClient
   
   // This data type (explained here: https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html)
   // is a bit array
   fd_set readfds, writefds; //only needed within this function
   
   //For Nonblocking I need a struct timeval... or just the two things in the struct??
   struct timeval timeout;
   
   while(TRUE) 
	{ 
        //neds to be in while loop as it got cleared... overwritten somehow
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;

		//clear the socket set 
		FD_ZERO(&readfds); 
        FD_ZERO(&writefds);
	
		//add socket to set 
		FD_SET(sockfd, &readfds); //comes back with the same value
        FD_SET(sockfd, &writefds); // I THOUGHT ONE WOULD TELL ME IF FILEDESCRIPTOR IS READY FOR READ VS WRITE
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		
		//timeval ... long tv_set
		//create a timeval and put the numbers i want in there which are 0 sec and 10microsend or so
		activity = select( sockfd , &readfds , &writefds , NULL , &timeout); //the last parameter is a timeout... could put 10ms and it would not be blocking
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} else { // activity returns a File Descriptor ready for reading or writing.. call chat function
            Chat();
        }

    }
   
   
   
    
}

void TCPClient::Chat (/*int socketfdforchatting*/){

    //check to see if socket has data with the select() function



    //std::cout << "chat received" << socketfdforchattting;
    //char chatbuffer[1025]; // replaced with buffer from TCPClient.h private variable
    int n; // A local n for counting charaters on chatbuffer with getch
        
    //for (;;) { 
        //bzero(buffer, sizeof(buffer)); 

        // read the message from client and copy it in buffer 
        //printf("From client:\n");
        read(sockfd, buffer, sizeof(buffer)); 
        
        // print buffer which contains the client contents 
        printf(buffer); 
		
        
        bzero(buffer, sizeof(buffer)); 
        n = 0; 
        // copy server message in the buffer 
        
        while ((buffer[n++] = getchar()) != '\n') 
            ; 
        
        //while ((chatbuffer[n++] = getchar_unlocked()) != '\n') 
        //scanf("%[^\n]",ch)

		if(n > sizeof(buffer)) {  
            printf("\n\nAre you trying to fill my buffer? How Rude!\n");
            bzero(buffer, sizeof(buffer));    
            //break; This will break from the function and program if you want
        } 
        // and send that buffer to client 
        write(sockfd, buffer, sizeof(buffer)); 

        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buffer, 4) == 0) { 
            printf("Client Exit...\n"); 
            //break; 
            closeConn();
        } 
    //}
}  

/**********************************************************************************************
 * closeConnection - Your comments here
 *
 *    Throws: socket_error for recoverable errors, runtime_error for unrecoverable types
 **********************************************************************************************/

void TCPClient::closeConn() {
    close(sockfd);
}


