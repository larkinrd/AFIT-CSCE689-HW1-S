
#include <string>
#include <fstream> //Stream class to both read and write from/to files
#include "LogClient.h"
#include <iostream>
#include <chrono>
#include "strfuncts.h"

using namespace std;

LogClient::LogClient(){} //empty constructor

/*******
 * The name of the logfile is saved in the classes private variable log_file_
 * such that you can create different log files whenever you create new
 * LogClient objects. The constructor will create the file if it does not
 * exist and send a message to  WriteToLogFile() method
 * 
 * I do not know if it is better to pass a filename or pass an ostream
 * istream or fstream object into this class
 * *****/

LogClient::LogClient(std::string infilename):log_file_(infilename) {

    // Test to see if file exists
    std::fstream myfstreamobject;
    myfstreamobject.open(log_file_, std::fstream::in);
    if(!myfstreamobject.is_open()){
        myfstreamobject.open(log_file_, std::fstream::out | std::fstream::app);
        std::string msg = "Created Log File for ";
        msg += infilename;
        WriteToLogFile(msg);
    }
}

LogClient::~LogClient() {} //empty destructor

/**********************************************************************************************
 * WriteToLogFile (constructor) - Creates a Stdin file descriptor to simplify handling of user input. 
 *
 **********************************************************************************************/

int LogClient::WriteToLogFile(std::string &logmsg) {
    
    //create stream object, test if log_file_ exists
    std::fstream anotherstreamobject; 
    anotherstreamobject.open(log_file_, std::fstream::app);
        if(!anotherstreamobject.is_open())
            std::cout << "Coule not open fstream object and write to " << log_file_ << std::endl;

    // get current date/time based of system, covert to string, clrnewlines
    time_t now = time(0);
    char* dt = ctime(&now);
    std::string mydatetime (dt); // convert char * to string
    clrNewlines(mydatetime);

    //write stream position info to log file for my own learning  
    streampos beforewrite, afterwrite; 
    beforewrite = anotherstreamobject.tellg();
    anotherstreamobject << mydatetime << " :: " << logmsg << "\n";
    afterwrite = anotherstreamobject.tellg();
    anotherstreamobject << "-->" << "Start Pos. " << beforewrite << ". End Pos. " << afterwrite << " : " << (afterwrite - beforewrite) << " bytes written w/newline\n";
    
    // Used for offsetting the streampointer
    //  seekg to GET and the position 67 bytes from the beginning of stream
    //      anotherstreamobject.seekg(67, ios::beg); 
    //  seekp to PUT the position 23 bytes from end of stream
    //      anotherstreamobject.seekp(23, ios::end);
    
  
    return (afterwrite - beforewrite);
}