/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard structure for scorpion virtual machine
* 
*/

#include "sstate.h"
#include "log.h"
#include <stdlib.h>

extern int vm_assert, vm_debug, vm_list,
         vm_log;
         
unsigned int prototype_limit=((84*1024)+(8*156));         
extern void std_err_(string n);

void list_handler(std::string what)
{
  std_err_( what );
}

int emplode(scorpion_state* s_state, int N)
{
    if(s_state == NULL || N==0)
      return 1;
      
    if(N==0)
    {
        if(s_state->i_heap.size() != 0)
          s_state->i_heap.clear();
        
        if(s_state->state != 0)
        {
          cout << "The Scorpion virtual machine is attempting to shutdown with abnormal state (" << s_state->state << ").";
        } // TODO: add logging here
          
        free( s_state );
    }
      
    return ( 0 );
}

int Scorpion_Init_create_state( BIO* io, scorpion_state* s_state, Eso *reader, 
          sstring* ScorpionOptions, int argc )
{
   // Set up system log service
   log.log();
   log.service_setup( vm_log, false, "" );
   
   /* Setup Scorpion state */
   s_state = (scorpion_state*)malloc(1);
   if(s_state == NULL)
     return 1;
   
   reader->read( ScorpionOptions[0].str().c_str(), io );
   s_state->permission_list._init_();
   
   if(vm_list)
   {
     printf("Eso application header:\n--------------------\n");
     printf("File: %s\n%s", ScorpionOptions[0].str().c_str(), reader->header_info().c_str());
     printf("--------------------\n\n");
   }
   
   if(reader->header.method_size > prototype_limit)
   {
     stringstream ss;
     ss << "method size > prototype_limit. Note: method siz(" 
        << reader->header.method_size << ") (limit:" << prototype_limit << ")";
     log.LOGD( ss.str() );
     return 1;
   }
   
   log.LOGI("Setting up application permissions.");
   for(unsigned int i = 0; i < reader->header.permissions.size(); i++){
       stringstream ss;
       for(unsigned int xx = i; xx < reader->header.permissions.size(); xx++){
           if(reader->header.permissions.at(xx) != ';')
             ss << reader->header.permissions.at(xx) << "";
           else
             break;
           i++;
       }
       s_state->permission_list.add(ss.str()); // The Scorpion API will handle host machine's security
   }
   
   log.LOGI("Setting up application log service.");
   usr_log.service_setup(reader->header.log_precedence, 
     true, reader->header.log_file.str());
   return ( 0 );
}
