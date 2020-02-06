#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Server.h"
#include <string.h>
#include <string>
#include <netinet/in.h>

#define TRUE 1 
#define FALSE 0 

class TCPServer : public Server 
{
public:
   TCPServer();
   ~TCPServer();

   void BindServer(const char *ip_addr, unsigned short port);
   void ListenServer();
   void HandleAcceptedObjects(/*socket_object, socket_id*/);
   void Chat (int socketfdforchattting);
   void ShutdownServer();
   void error(const char *msg);
   void sendMenu(int socketFD);
   //using const char *userinput becuase I dont need to modify userinput buffer
   // *userinput is a pointer to a charachter array
   void ProcessClientResponse(int socketFD, char *userinput);
   //void displayCountdown (int socketFD);
	
private:
 
 //See Compiler Error in TCDPServer.cpp
 
   //set of socket descriptors 
 	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30], max_clients = 2 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in _address;
    //struct in_addr _in_addr; 
    
	//a message 
	//char *message = (char *) "ECHO Daemon v1.0\r\n"; 
   std::string message;
	char buffer[1025]; //data buffer of 1K

};
#endif
