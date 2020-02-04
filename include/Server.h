#ifndef SERVER_H
#define SERVER_H

/******************************************************************************************
 * Server - Parent class for student's TCP server
 *
 *  	   Server(Const): right now does nothing (still should call for future portability
 *  	   ~Server(Dest): no cleanup required in parent
 *
 *  	   bind - binds to an IP address and port on the server
 *  	   listen - starts listening on a single-threaded process (non-blocking) and loops
 *                listening for connections and handling the existing connections
 *  	   shutdown - shuts down the server properly
 *  	   
 *  	   Exceptions: sub-classes should throw a std::exception with the what string field
 *  	               populated for any issues.
 *
 *****************************************************************************************/

class Server { 
   public:
      virtual ~Server();

      // Overload me!
      virtual void BindServer(const char *ip_addr, unsigned short port) = 0;
      virtual void ListenServer() = 0;

      // if you overload, don't forget to call me
      // ???WHAT DOES THE VIRTUAL VOID MEAN???   AND
      // ??? WHY DO I NEED THE =0; AT THE END... ADDING THIS RESOLVED MY COMPILIER
      // ERROR OF NOT RECURSIVE SOMETHING OR OTHER
      virtual void HandleAcceptedObjects(/*socket_object, socket_id*/) =0;
      virtual void ShutdownServer() = 0;

   protected:

      // Do not forget, your constructor should call this constructor
      Server();

   private:
 

      // Stuff to be left alone
};

#endif
