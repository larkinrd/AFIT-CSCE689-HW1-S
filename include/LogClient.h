// #ifndef LOGCLIENT_H //dont think i need this
// #define LOGCLIENT_H // dont think i need this

#include <string>
#include <fstream> //for using input file stream

/******************************************************************************************
 * LogClient - A simple class for an object to write a message to 
 *              a log file (text file) of their choosing
 *
 *  	   LogClient(Const): When called check to ensure log file exists
 *  	   ~LogClient(Dest): ?NOT SURE WHAT I NEED TO CLEAN?
 *
 *         writeMsg - write a message to the log file
 * 
 *  	   Exceptions: ?? I DONT KNOW?? sub-classes should throw a std::exception with the what string field
 *  	               populated for any issues.
 *
 ************************MORE INFO ON FILE VS IFSTREAM*************************************
 *
 *   fstream is a better encapsulation and has higher level concepts.
 *   fstream is exception safe.
 *   fstream is also a stream and can be treated generically as a stream.
 *
 *   Imagine: void read(istream& istr)
 *
 *   We could pass in an ifstream, a istrstream, or even cin. This is very useful for unit testing.
 * 
 *************************MORE INFO ON NAMING CONVENTION***********************************
 * 
 * https://google.github.io/styleguide/cppguide.html#VNaming
 *  
 * FOR CLASSES - MyNewClass{} - capital letter for each new word. No underscores
 * ThisIsMyFunction() - capital letter for each new word. No underscores
 * mycommonvariable - all lowercase
 * myclassdatavariable_ - all lowercase with trailing underscore (_)
 * kMyConstantVariable - starts with k, use sentence case
 * struct MyStruct {int first, int second_one }
 *   - struct name is sentence case, variables are lowercase, no underscore (_)
 * See link for Namespace, Enumerator, Macro
 * 
 * ***************************************************************************/

class LogClient {

public:
    LogClient(); //not sure if I will use this
    LogClient(const std::string infilename);
    ~LogClient();
    
    //need my own file pointer
    int WriteToLogFile(std::string &logmsg); 

private:
    std::string log_file_; //class private variable

    //SO WHY CANT I FIGURE OUT HOW TO PASS AN ISTREAM OSTREAM FSTREAM OBJECT?
};