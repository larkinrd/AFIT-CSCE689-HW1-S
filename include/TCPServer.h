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
   // *userinput is a pointer to a character array
   void ProcessClientResponse(int socketFD, char *userinput);
   //void displayCountdown (int socketFD); was moved to strfuncts.cpp so that both
   //client and server could use the function if desired.NOTE, funciton only used
   //in TCPServer.cpp
	
private:
 
   //set of socket descriptors 
 	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30], max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
	struct sockaddr_in _address;
   std::string message;
	char buffer[1025]; //data buffer of 1K and is zeroed out in the TCPServer Constructor

};
#endif
