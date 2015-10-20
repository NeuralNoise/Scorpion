//
// Created by bnunnall on 8/19/2015.
//
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

string build_version = "v0.0.1.0";
#define NUM_OPTIONS 4
string args[ NUM_OPTIONS ];

string OPTION = "";
int file_start = 0, fsize;
long imagesize = 0;

string build_file = "";
string application_file = "application";
bool hasbuild = false;

void setup()
{
   args[0] = "--help";
   args[1] = "-version";
   args[2] = "--build";
   args[3] = "-xso";
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
   cout << "Usage: scorpionc [-options] [source-files...]\n" << endl;
   cout << "Source file must have a .scorpion extension to be packaged\n" << endl;
   cout << "[-options]\n\n    -version          print the current product version and exit" << endl;
   cout <<               "    --build<file>     set the dev build scipt file. This option is required." << endl;
   cout <<               "    -o<file>        set the output .xso file. Default is application.xso." << endl;
   exit(1);
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
            exit(1);
         }
         else{  // time for running application
            fsize = (argc - file_start) - 1;
            return;
         }

         if(OPTION == "--help")
            help();
         else if(OPTION == "-version"){
            cout << "Scorpion(TM) Compiler build version: \"" << build_version << "\"" << endl;
            exit(1);
         }
         else if(OPTION == "--build"){
            i++;
            file_start++;
            data = args[i];
            build_file = data;
            hasbuild = true;
         }
         else if(OPTION == "-o"){
            i++;
            file_start++;
            data = args[i];
            application_file = data;
         }


         file_start++;
   }
}
