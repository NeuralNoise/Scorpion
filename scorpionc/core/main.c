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
 * Command line invocation of the Scorpion compiler
*/
#include <iosfwd>
#include <stdlib.h> 
#include <string.h> 
#include "sio.h"
#include "string.h"
#include "bootstrap.h"
using namespace std;

#define PROGNAME     "scorpionc"  /* default program name */

static const char* progname=PROGNAME;	/* actual program name */
static const char* build_version = "0_1_52";
static int fast = 0;
void std_err_(string n)
{
  fprintf(stderr,"%s: %s\n",progname,n.c_str());
  core_exit(EXIT_FAILURE);
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
      fprintf(stderr, "Usage: scorpionc [-options] [source files..]\n" 
         "           (to compile scorpion source files)\n"
         "[-options]\n\n"
         "    -version        print the current product version and exit.\n"
         "    -showversion    print the current product version and continue.\n"
         "    -help -?        display this help message.\n"
         "    --asm           output assembly source.\n"
         "    -c              only compile source files.\n"
         "    -f              print each source file as it is processed.\n"
         "    --fast          perform fast compilation.\n"
         "    -W              enable compiler warnings.\n"
         "    -Wextra         enable extended compiler warnings.\n"
         "    --build         set main build script file.\n"
         "    --output -o     the name of the output file to be created.\n"
         "    -sl             compile project as a scorpion shared librabry.\n"
         "See %s for more details.\n", product_website.c_str());
  }
  exit(EXIT_FAILURE);
}


#define contains(s)	(strcmp(argv[i],s)==0)

string getargs(int argc, const char** argv)
{
  stringstream _argv;
  for(int i = 1; i < argc; i++)
  {
      if(*argv[i]!='-')
      {
          do
          {
            _argv << argv[i++] << " ";
          }
          while(i<argc);
          return _argv.str();
      }
      else if(contains("-"))
          std_err_("syntax error near `-', expected symbol.");
      else if(contains("-version") || contains("-showversion"))
      {
        printf("scorpionc version \"%s\"\n"
            "Scorpion(TM) Compiter (build SCORPION_VERSION_%s-base)\n", build_version, build_version);
        
        if(contains("-version"))
           exit( EXIT_SUCCESS );
      }
      else if(contains("-help") || contains("-?"))
         usage("null", false);
      else if(contains("--fast"))
         fast = 1;
      else if(contains("-f") || contains("--asm") 
             || contains("-sl") || contains("-W")
             || contains("-Wextra") || contains("-c"))
          _argv << argv[i] << " ";
      else if(contains("-o") || contains("--output") 
             || contains("--build"))
      {
          _argv << argv[i++] << " ";
          if(!(i<argc)) return "";
          _argv << argv[i] << " ";
      }
      else {
          stringstream ss;
          ss << "Unrecognized command line option: " << argv[i] << endl;
          ss << "Try '" << progname << " -help' for more information.";
          std_err_(ss.str());
      }
  }
  return "";
}

int main(int argc, const char** argv)
{
    BIO* io = NULL;
    sstring p_args;
    stringstream run;
    core.p = 1;
    
    p_args.hash_start();
    io = (BIO*)malloc(1*sizeof(BIO));
    if(io == NULL) std_err_("Could not start the Scorpion compiler.");
    
    if( argc == 1 )
    {
        run << "no input files!" << endl
            << "Try '" << progname << " -help' for more information.";
        std_err_( run.str() );
    }
        
    p_args.str( getargs(argc, argv) );
    if(p_args.str() == "") std_err_("Could not start the Scorpion compiler.");
    run << "scorpion -log=off /usr/share/scorpion/lib/scorpionc " << p_args.str();
    if(fast) goto compile;
    
    buff_init( io );
    if(init_core_bootstrap( io ) != 0)
        std_err_("error running bootstrap");
    
    if(init_core_bootstrap_start() != 0)
        std_err_("internal error");
    else core_bootstrap_push( io );
    
    compile:
      core_exit( system( run.str().c_str() ) );
}
