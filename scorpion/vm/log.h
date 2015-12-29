/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard Routines for logging data in Scorpion.
* This is used for both system and user logging.
* 
*/

#ifndef log_h
#define log_h

#include "sstate.h"
#include <sstream>
 
#define ENV_DEV
//  #define ENV_RUNTIME

 extern const int VERBOSE;

 extern const int DEBUG;

 extern const int INFO;

 extern const int WARN;

 extern const int ERROR;

 extern const int ASSERT;
 
 string t_ch(int priority);
 
 void println(int priority,string tag,string message, const char* file, 
            string& stackmsg, string package);
 
 void print(int priority, string tag, string message, bool log, const char* file, 
           string& stackmsg, int p, string package);
 
 void a(string tag, string message, string file, bool log, string& stackmsg, int p, string package);

 void e(string tag,string message, string file, bool log, string& stackmsg, int p, string package);

 void w(string tag,string message, string file, bool log, string& stackmsg, int p, string package);

 void i(string tag,string message, string file, bool log, string& stackmsg, int p, string package);

 void d(string tag,string message, string file, bool log, string& stackmsg, int p, string package);
 
 void v(string tag,string message, string file, bool log, string& stackmsg, int p, string package);

 class Log
 {
     public:
       void log()
       {
         precedence=(VERBOSE);
         file=("");
         logstack=("");
         package=("vm.internal");
         basepackage=("vm.internal");
         on=(false);
         bufferthreshold=(124 * (1000)); // 124 kb
       }
     
       void service_setup(int level, bool usr, string _file, bool force_update = false)
       {
          if(on && !force_update) return; // you cant setup the log service more than once
          #ifdef ENV_DEV
            file = "/usr/share/scorpion/vm/log/out.log";
          #endif
          #ifdef ENV_RUNTIME
            file = "/usr/share/scorpion/sre/vm/log/out.log";
          #endif
          
          if(level == ASSERT+1)
            on=false;
          else 
            on=true;
            
          if(on) {
             if(usr) file = _file;
             
             buff_leak( gstate.io );
             buffS_fill( " ", gstate.io );
             int status = buffS_write( file.c_str(), gstate.io );
   
             if(status != 0){
                cout << "warning: I/O err, unable to open system log file!\n";
                on = false; // we do this to skip logging
                return;
             }
             
             precedence = level;
             if(!force_update)
             {
                 logstack=("");
                 logstack += "=======System Dev Log begin=======\n";
                 
                 if(!usr)
                   LOGV("Log setup: system log");
                 else
                   LOGV("Log setup: user log");
             }
          }
       }
       void setpackage(string name){
          package = name;
       }
       void LOGA(string message, string class_ = "System"){
          a(class_, message, file, on, logstack, precedence, package);
          if(logstack.size() >= bufferthreshold)
             logstack=("");
          package = basepackage;
       }
       void LOGE(string message, string class_ = "System"){
          e(class_, message, file, on, logstack, precedence, package);
          if(logstack.size() >= bufferthreshold)
             logstack=("");
          package = basepackage;
       }
       void LOGW(string message, string class_ = "System"){
          w(class_, message, file, on, logstack, precedence, package);  
          if(logstack.size() >= bufferthreshold)
             logstack=("");
          package = basepackage;
       }
       void LOGI(string message, string class_ = "System"){
          i(class_, message, file, on, logstack, precedence, package);
          if(logstack.size() >= bufferthreshold)
             logstack=("");
          package = basepackage;
       }
       void LOGD(string message, string class_ = "System"){
          d(class_, message, file, on, logstack, precedence, package);
          if(logstack.size() >= bufferthreshold)
             logstack=("");
          package = basepackage;
       }
       void LOGV(string message, string class_ = "System"){
          v(class_, message, file, on, logstack, precedence, package);  
          if(logstack.size() >= bufferthreshold)
             logstack=("");
          package = basepackage;
       }
     
     private:
       int precedence;
       string file, package, basepackage;
       string logstack;
       bool on;
       unsigned long bufferthreshold;
 };

 // Begin Dalvik code
/*
 * Get the wall-clock date/time, in usec.
 */
 long getWallTimeInUsec();
 // END Delvik code

 const string currtime();
 
 extern Log log;
 
 extern Log usr_log;

#endif // log_h