#include <algorithm>
#include <termios.h>
#include "strfuncts.h"
#include <thread> //for std::this_thread
#include <chrono>
#include <string.h>
#include <string> 
#include <iostream> //for std::cout
#include <cstdio> //fflush()

// MY LEARNING NOTES
 // std::cout AND printf CAN be used with CHAR ARRAYS
 // You DO NOT ues printf() with STRINGS
 // Using clrNewLines removed the \0 or \n; therefore I could not print to
 //    to the screen correctly
 // https://dev.to/lucpattyn/stdcout-is-not-printing-in-consoleterminal-5emn
 // VSCODE Debugger setup https://www.youtube.com/watch?v=X2tM21nmzfk&app=desktop
 // Netcat binaris https://svn.nmap.org/nmap/ncat/
 /******std::endl vs ‘\n’
  * Using std::endl can be a bit inefficient, as it actually does two jobs: it moves 
  * the cursor to the next line, and it “flushes” the output (makes sure that it shows 
  * up on the screen immediately). When writing text to the console using std::cout, 
  * std::cout usually flushes output anyway (and if it doesn’t, it usually doesn’t 
  * matter), so having std::endl flush is rarely important.
  ********/

//simple function to put program to sleep for n seconds and prints that to the screen
void displayCountdown (int countdown) {      
   std::cout << "countdown: ";
   for (int i=countdown; i>0; --i) {
      std::cout << i << "";
      std::this_thread::sleep_for (std::chrono::seconds(1));
      fflush(stdout); //YOU HAVE TO FLUSH STDOUT IF YOU DONT USE STD::ENDL OR '\n'
   }
   std::cout << "\n";
   //this is NEAT... this will clear the console screen
   //system("clear");
}

void clrNewlines(std::string &str) {
   str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
   str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
}

bool split(std::string &orig, std::string &left, std::string &right, const char delimiter) {
   int del_loc = orig.find(delimiter);
   if (del_loc == std::string::npos)
      return false;

   left = orig.substr(0, del_loc);
   right = orig.substr(del_loc + 1, orig.size());
   right.erase(std::remove(right.begin(), right.end(), '\n'), right.end());
   right.erase(std::remove(right.begin(), right.end(), '\r'), right.end());
   lower(left);

   return true;
}

void lower(std::string &str) {
   std::transform(str.begin(), str.end(), str.begin(),
         [](unsigned char c){ return std::tolower(c); });
}

int hideInput(int fd, bool hide) {
   struct termios tattr;

   if (tcgetattr(fd, &tattr) != 0)
      return -1;

   if (hide)
      tattr.c_lflag &= ~ECHO;
   else
      tattr.c_lflag |= ECHO;

   if (tcsetattr(fd, TCSAFLUSH, &tattr) != 0)
      return -1; 
   return 0;
}

