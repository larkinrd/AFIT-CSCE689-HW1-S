#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include "Client.h"
#include <netinet/in.h>

#define TRUE 1

// The amount to read in before we send a packet
const unsigned int stdin_bufsize = 50;
const unsigned int socket_bufsize = 100;

class TCPClient : public Client
{
public:
   TCPClient();
   ~TCPClient();

   virtual void connectTo(const char *ip_addr, unsigned short port);
   virtual void handleConnection();
   void Chat (/*int socketfdforchattting*/);
   virtual void closeConn();
   void error(const char *msg);

private:

int sockfd, portno, n, activity;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[1025];

};


#endif
