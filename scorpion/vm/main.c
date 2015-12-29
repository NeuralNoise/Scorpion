/**
* Copyright (C) 2015 The Scorpion Programming Language
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Portions of the code surrounded by "// Begin Dalvik code" and
* "// END Delvik code" are copyrighted and licensed separately, as
* follows:
*
* Copyright (C) 2014 The Android Open Source Project
*
* Licensed to the Apache Software Foundation (ASF) under one or more
* contributor license agreements. The ASF licenses this file to You
* under the Apache License, Version 2.0 (the "License"); you may not use 
* this file except in compliance with the License.  You may obtain a copy
* of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
/**
 * Command line ivocation of the Scorpion virtual machine
*/
#include <iostream>
#include <stdlib.h>
#include "sio.h"
#include "eso.h"
#include "log.h"
#include "sstate.h"
#include "stypes.h"
using namespace std;

#define PROGNAME	"scorpion"		/* default program name */

// Runtime builds
#define VM_64BIT
// #define VM_32BIT
// #define VM_ARM

static const char* progname=PROGNAME;	/* actual program name */
static const char* build_version = "0_1_39";
int revision = 8;
int vm_assert=1, vm_debug=0, vm_list=0,
    vm_log=VERBOSE;
scorpion_gstate gstate;
Log log;
Log usr_log;

void std_err_(string n)
{
  fprintf(stderr,"%s: %s\n",progname,n.c_str());
  exit(EXIT_FAILURE);
}

void nstd_usage()
{
  string product_website = "https://github.com/AndroDevcd/Scorpion-Dist/wiki";
  printf("Usage: scorpion [-options] program [args...]\n" 
       "           (to execute scorpion object file)\n"
       "[-options]\n\n"
       "    -l              list application header details.\n"
       "    -log=<level>    set the precedence of system logging.\n"
       "                    \"log=off\" to shut off system logging.\n"
       "See %s for more details.\n", product_website.c_str());
  exit(EXIT_FAILURE);
}

void usage(string n, bool stderr_)
{
  string product_website = "https://github.com/AndroDevcd/Scorpion-Dist/wiki";
  
  if(stderr_)
   fprintf(stderr,"%s: unrecognized option '%s'\n",progname,n.c_str());
  else
  {
      if(n!="null")
       fprintf(stderr,"%s: %s\n",progname,n.c_str());
      fprintf(stderr, "Usage: scorpion [-options] program [args...]\n" 
         "           (to execute scorpion object file)\n"
         "[-options]\n\n"
         "    -version        print the current product version and exit.\n"
         "    -showversion    print the current product version and continue.\n"
         "    -help -?        display this help message.\n"
         "    -da -disableassertions\n"
         "                    disable assertions.\n"
         "    -sdb            enable the scorpion debug bridge.\n"
         "    -?std           print non-standard options.\n"
         "See %s for more details.\n", product_website.c_str());
  }
  exit(EXIT_FAILURE);
}

#define contains(s)	(strcmp(argv[i],s)==0)

sstring* getargs(int argc, const char** argv, int* N)
{
  sstring* vm_argv;
  for(int i = 1; i < argc; i++)
  {
      if(*argv[i]!='-')
      {
          int argc_ = argc-i,iter=0;
          (*N)=argc_; 
          
          vm_argv = (sstring*)malloc(argc_*sizeof(sstring));
          if(vm_argv==NULL) return NULL;
          
          do
          {
            if(vm_argv[iter++].str(argv[i++])!=0)
                std_err_("could not initalize Scorpion vm args!");
          }
          while(i<argc);
          return vm_argv;
      }
      else if(contains("-version") || contains("-showversion"))
      {
        cout << "scorpion version \"" << build_version << "\"" << endl;
        cout << "Scorpion(TM) SDK (build SCORPION_VERSION_" << build_version << "-b" << revision << ")"<< endl;
        #ifdef VM_64BIT
           cout << "Scorpion(TM) 64-Bit Server VM (v" << revision << ")" << endl;
        #endif
        #ifdef VM_32BIT
           cout << "Scorpion(TM) 32-Bit Server VM (v" << revision << ")" << endl;
        #endif
        #ifdef VM_ARM
           cout << "Scorpion(TM) ARM Server VM (v" << revision << ")" << endl;
        #endif
        if(contains("-version"))
           exit( EXIT_SUCCESS );
      }
      else if(contains("-help") || contains("-?"))
         usage("null", false);
      else if(contains("-da") || contains("-disableassertions"))
         vm_assert = 1;
      else if(contains("-sdb"))
         vm_debug = 1;
      else if(contains("-?std")) 
         nstd_usage();
      else if(contains("-l"))
         vm_list = 1;
      else if(contains("-log=off")) 
         vm_log = 1;
      else if(contains("-log=verbose")) 
         vm_log = 2;
      else if(contains("-log=debug")) 
         vm_log = 3;
      else if(contains("-log=info")) 
         vm_log = 4;
      else if(contains("-log=warn")) 
         vm_log = 5;
      else if(contains("-log=error"))
         vm_log = 6;
      else if(contains("-log=assert"))
         vm_log = 7;
      else {
          stringstream ss;
          ss << "Unrecognized command line option: " << argv[i];
          std_err_(ss.str());
      }
  }
  return NULL;
}

int main(int argc, const char** argv)
{
    BIO* io = NULL;
    scorpion_state* state = NULL;
    Eso reader;
    int result = 0, N=0;
    sstring *ScorpionOptions;
    
    stringstream ss;
    
    io = (BIO*)malloc(1*sizeof(BIO));
    if( io == NULL ) goto bail;
    if( argc == 1 )
        usage("no input files!", false);
    
    ScorpionOptions = getargs(argc, argv,&N);
    if( ScorpionOptions == NULL ) std_err_("could not start Scorpion vm.");
    
    /**
     * Here we initalize our io buffer
     *  This io buffer will be the inital io buffer 
     *  that we will use to process our eso file
     */
    buff_init( io );
    gstate.io = io;
    
    /* Initalize the Scorpion virtual machine */
    if((result = Scorpion_Init_create_state( io, state, &reader, ScorpionOptions, N )) != 0)
        std_err_("could not create the Scorpion vm.");
    
    bail:
    
    return ( result );
}