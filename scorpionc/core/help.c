/*
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
 *
 */
#include "scorpionc.h"
#include "clib/compilr/core.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

string build_version = "v0.1.0.9";
#define NUM_OPTIONS 10
string args[ NUM_OPTIONS ];

string OPTION = "";
option options;
int file_start = 0;

void setup()
{
   options.build_file = "";
   options.output_file = "application";
   options.ags_t = 0;
   options.compileOnly=false;
   options.warnings=false;
   options.extendedWarnings=false;
   options.sharedLibrary=false;
   
   args[0] = "--help";
   args[1] = "-version";
   args[2] = "--build";
   args[3] = "-o";
   args[4] = "-showversion";
   args[5] = "-?";
   args[6] = "-c";
   args[7] = "-w";
   args[8] = "-wextra";
   args[9] = "-sl";
}

bool isarg(string arg)
{
  for(int i = 0; i < NUM_OPTIONS; i++)
  {
     if(arg == args[i]){
        OPTION = arg;
        return true;
     }
  }

  return false;
}

bool hasdash(string arg)
{
   if(arg.at(0) == '-')
     return true;

  return false;
}

void help()
{
   cout << "Usage: scorpionc [-options] [build-file] [source-files...]" << endl;
   cout << "or     scorpionc [build-file] [source-files...]\n\n";
   cout << "Source file must have a .scorpion extension to be packaged\n" << endl;
   cout << "[-options]\n\n    -version          print the current product version and exit" << endl;
   cout <<               "    -showversion      print the current product version and continue." << endl;
   cout <<               "    --build<file>     set the dev build scipt file. This option is required." << endl;
   cout <<               "    -o<file>          set the output object file. Default is application.xso." << endl;
   cout <<               "    -c                compile only and do not generate object file." << endl;
   cout <<               "    -w                allow warnings to be displayed." << endl;
   cout <<               "    -wextra           allow extended warnings to be displayed." << endl;
   cout <<               "    -sl               create a native Scorpion shared library." << endl;
   cout <<               "    --help -?         display this help message." << endl;
   exit(1);
}

string trim(string str)
{
  return str.substr(0, str.size()-1);
}

void parseargs(int argc, const char **args)
{
      string data = "";
      for(int i = 1; i < argc; i++){
         data = args[i];

         if(isarg(data)) {}
         else if(hasdash(data))
         {
            cout << "Unrecognized command line option: " << data << endl;
            cout << "Try 'scorpionc --help' or 'scorpionc -?' for more information.\n";
            exit(1);
         }
         else{  // time for running application
          //  fsize = (argc - file_start) - 1;
            int program_args = (argc - i);
            
            options.ags_t = program_args;
            options.ags = new string[program_args];
            int curArg = 0;
            
            // add program arguments
            for(int x = i; x < argc; x++)
                options.ags[curArg++] = args[x];
                
            return;
         }

         if(OPTION == "--help" || OPTION == "-?")
            help();
         else if(OPTION == "-version"){
            cout << "Scorpion(TM) Compiler build version: \"" << build_version << "\"" << endl;
            exit(1);
         }
         else if(OPTION == "-showversion")
            cout << "Scorpion(TM) Compiler build version: \"" << build_version << "\"" << endl;
         else if(OPTION == "--build"){
            i++;
            file_start++;
            
            if(!(i < argc)){
               cout << "Error: could not start Scorpion compiler. \nA fatal Error has occurred, shutting down." << endl;
               exit(1);   
            }
            
            options.build_file = args[i];
         }
         else if(OPTION == "-o"){
            i++;
            file_start++;
            
            if(!(i < argc)){
               cout << "Error: could not start Scorpion compiler. \nA fatal Error has occurred, shutting down." << endl;
               exit(1);   
            }
         
            options.output_file = args[i];
         }
         else if(OPTION == "-c")
            options.compileOnly=true;
         else if(OPTION == "-w")
            options.warnings=true;
         else if(OPTION == "-wextra")
            options.extendedWarnings=true;
         else if(OPTION == "-sl")
            options.sharedLibrary=true;
         
         file_start++;
   }
}
