/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard Routines for logging data in Scorpion.
* This is used for both system and user logging.
* 
*/

#include <string>
#include <sstream>
#include <sys/time.h>
#include "log.h"

using namespace std;
 
 const int VERBOSE = 2;

 const int DEBUG = 3;

 const int INFO = 4;

 const int WARN = 5;

 const int ERROR = 6;

 const int ASSERT = 7;
 
 string t_ch(int priority)
 {
   switch( priority ){
      case 2:
        return "V";
      case 3:
        return "D";
      case 4:
        return "I";
      case 5:
        return "W";
      case 6:
        return "E";
      case 7:
        return "A";
      default:
        return "?";
   }
 }
 
 void println(int priority,string tag,string message, const char* file, 
            string& stackmsg, string package)
 {
     stackmsg += currtime() + " " + package + " " + t_ch(priority) + "/" + tag + ": " + message + "\n";

     buff_leak( gstate.io );
     buffS_fill( stackmsg, gstate.io );
     cout << "\nlogging " << buf_data( gstate.io ).str();
     buffS_write( file, gstate.io );
 }
 
 void print(int priority, string tag, string message, bool log, const char* file, 
           string& stackmsg, int p, string package)
 {
     if(!log) return;
     else if(log){
         if(priority >= p)
             println(priority,tag,message, file, stackmsg, package);
     }
 }
 
 void a(string tag, string message, string file, bool log, string& stackmsg, int p, string package)
 {
    print(ASSERT,tag,message,log,file.c_str(), stackmsg, p, package);
 }

 void e(string tag,string message, string file, bool log, string& stackmsg, int p, string package)
 {
    print(ERROR,tag,message,log,file.c_str(), stackmsg, p, package);
 }

 void w(string tag,string message, string file, bool log, string& stackmsg, int p, string package)
 {
    print(WARN,tag,message,log,file.c_str(), stackmsg, p, package);
 }

 void i(string tag,string message, string file, bool log, string& stackmsg, int p, string package)
 {
    print(INFO,tag,message,log,file.c_str(), stackmsg, p, package);
 }

 void d(string tag,string message, string file, bool log, string& stackmsg, int p, string package)
 {
    print(DEBUG,tag,message,log,file.c_str(), stackmsg, p, package);
 }
 
 void v(string tag,string message, string file, bool log, string& stackmsg, int p, string package)
 {
    print(VERBOSE,tag,message,log,file.c_str(), stackmsg, p, package);
 }
 
 // Begin Dalvik code
/*
 * Get the wall-clock date/time, in usec.
 */
 long getWallTimeInUsec()
 {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return  tv.tv_usec / 1000LL;
 }
 // END Delvik code

 const string currtime(){
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[90];
    tstruct = *localtime(&now);
    // Time used for logging
    strftime(buf, sizeof(buf), "%Y-%m-%d--%X", &tstruct);
    stringstream ss;
    ss << buf << "." << getWallTimeInUsec();
    return ss.str();
 }