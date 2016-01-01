/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard structure for scorpion virtual machine
* 
*/

#include "sstate.h"
#include "log.h"
#include "vm.h"
#include <stdlib.h>

extern int vm_assert, vm_debug, vm_list,
         vm_log;
         
unsigned int prototype_limit=((84*1024)+(8*156));         
extern void std_err_(string n);
    
int destroy_state(scorpion_state* s_state, int N)
{
    int status=0;
    if(s_state == NULL || N==0)
      return 1;
      
    if(N==1)
    {
        if(s_state->permission_t != 0)
          free( s_state->permission_list );
        
        if(s_state->size_t != 0)
          free( s_state->heap );
          
        if(s_state->p_args != NULL) free( s_state->p_args );
        if(s_state->stack != NULL) free( s_state->stack );
        if(s_state->i_heap != NULL) free( s_state->i_heap );
        if(s_state->static_methods != NULL) free( s_state->static_methods );
          
        if(s_state->state != state_normal)
        {
          stringstream ss;
          ss << "The Scorpion virtual machine is attempting to shutdown with abnormal state (" << s_state->state << ").";
          log.LOGV( ss.str() );
          status = 1;
        } 
        
        s_state->pc = 0;
        s_state->size_t = 0;
        s_state->permission_t = 0;
        s_state->method_t = 0;
        s_state->args_t = 0;
        s_state->sp = 0;
        s_state->if_count = 0;
        s_state->if_depth = 0;
        s_state->func_depth = 0;
        s_state->state = 0;
        s_state->stack_t = 0;
        s_state->iheap_t = 0;
        
        free( s_state );
    }
      
    return ( status );
}

void Scorpion_Init_start_state(scorpion_state* s_state)
{
    s_state->func_tracker.add_func(s_state->static_methods[0], 0, true);
    s_state->pc = s_state->static_methods[0].goto_;
    
    scorpion_vexecute( s_state );
}

int Scorpion_Init_create_state( BIO* io, scorpion_state* s_state, Eso *reader, 
          sstring* ScorpionOptions, int argc )
{
   // Set up system log service
   log.log();
   log.service_setup( vm_log, false, "" );
   
   s_state->init();
   s_state->func_tracker.init();
   s_state->args_t = argc;
   s_state->p_args = ScorpionOptions; // These will be handled later
   reader->read( ScorpionOptions[0].str().c_str(), io );
   
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
   int psize=0,iter=0;
   for(unsigned int x = 0; x < reader->header.permissions.size(); x++){
       if(reader->header.permissions.at(x) == ';')
        psize++;
   }
   s_state->permission_list = (sstring*)malloc( psize*sizeof(sstring) );
   if(s_state->permission_list == NULL) return 1;
   s_state->permission_t = psize;
   
   for(int i = 0; i < psize; i++){
       stringstream ss;
       for(unsigned int xx = i; xx < reader->header.permissions.size(); xx++){
           if(reader->header.permissions.at(xx) != ';')
             ss << reader->header.permissions.at(xx) << "";
           else
             break;
           i++;
       }
       s_state->permission_list[iter].hash_start();
       s_state->permission_list[iter++].str( ss.str() ); // The Scorpion API will handle host machine's security
   }
   
   log.LOGI("Setting up application log service.");
   if(!reader->header.logging) reader->header.log_precedence = ASSERT+1;
   usr_log.service_setup(reader->header.log_precedence, 
     true, reader->header.log_file.str());

   log.LOGD("Setting up memory structures.");
   if(s_state->alloc(reader->header.address_cap+1, 0xffff, reader->header.method_size) != 0)
   {
       log.LOGV("Failed to allocate memory for the Scorpion "
                "virtual machine, try optimizing your application.");
       return 1;
   }
   
   log.LOGD("Memory allocation was successful.");
   log.LOGI("Pre-processing eso image.");
   if(reader->process( s_state ) != 0)
   {
       log.LOGV("failed to pre-process eso image.");
       return 1;
   }
   
   log.LOGV("Eso image was processed successfully.");
   log.LOGD("Scorpion virtual machine was created successfully.");
   return ( 0 );
}
