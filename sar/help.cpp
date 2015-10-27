//
// Created by bnunnall on 8/19/2015.
//
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

string build_version = "v0.1.0.12";
#define NUM_OPTIONS 3
string args[ NUM_OPTIONS ];

string OPTION = "";
int file_start = 0, fsize;

string packagefile = "lib";

void setup()
{
   args[0] = "--help";
   args[1] = "-version";
   args[2] = "-a";
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
   cout << "Usage: sar [-options] [source-files...]\n" << endl;
   cout << "Source file must have a .scorpion extension to be packaged\n" << endl;
   cout << "[-options]\n\n    -version          print the current product version and exit" << endl;
   cout <<               "    -a<file>          set the output archive file. Default is lib.sar." << endl;
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
            cout << "sar build version: \"" << build_version << "\"" << endl;
            exit(1);
         }
         else if(OPTION == "-a"){
            i++;
            file_start++;
            data = args[i];
            packagefile = data;
         }

         file_start++;
   }
}
