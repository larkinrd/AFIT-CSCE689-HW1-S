/****************************************************************************************
 * tcp_client - connects to a TCP server for two-way comms
 *
 *              **Students should not modify this code! Or at least you can to test your
 *                code, but your code should work with the unmodified version
 *
 ****************************************************************************************/  

#include <stdexcept>
#include <iostream>
#include <getopt.h>
#include "exceptions.h"
#include "TCPClient.h"
#include "LogClient.h"

using namespace std; 

void displayHelp(const char *execname) {
   std::cout << execname << " <ip_addr> <port>\n";
}


int main(int argc, char *argv[]) {

   // Check the command line input
   if (argc < 3) {
      displayHelp(argv[0]);
      exit(0);
   }

   // Read in the IP address from the command line
   std::string ip_addr(argv[1]);

   // Read in the port
   long portval = strtol(argv[2], NULL, 10);
   if ((portval < 1) || (portval > 65535)) {
      std::cout << "Invalid port. Value must be between 1 and 65535";
      std::cout << "Format: " << argv[0] << " [<max_range>] [<max_threads>]\n";
       exit(0);
   }
   unsigned short port = (unsigned short) portval;
 

   // Get the command line arguments and set params appropriately
   //
   //  I THINK I NEED TO DO THIS!!!
   //
   /////////////

   //Add LogClient to Makefile.am and Create a logging client
   LogClient clientlogger("log.client_main.cpp");
   std::string logmsg; // = "test message from main"; 

   // Try to set up the server for listening
   TCPClient client;
   try {
      cout << "Connecting to " << ip_addr << " port " << port << endl;
      client.connectTo(ip_addr.c_str(), port);
      //std::cout << "59:did this execute";

   } catch (socket_error &e)
   {
      cerr << "Connection failed: " << e.what() << endl;
      //std::cout << "63:did this execute";
      return -1;
   }	   

   cout << "Connection established.\n";

    
   //client.handleConnection();
   
   
   try {
      client.handleConnection();
   } catch (runtime_error &e) {
      cerr << "Client error received: " << e.what() << endl;
      return -1;      
   }

   try {
      //client.handleConnection();
      client.closeConn();
      cout << "Client disconnected\n";

   } catch (runtime_error &e) {
      cerr << "Client Disconnect errored: " << e.what() << endl;
      return -1;      
   }

   return 0;
}
