//
// Created by bnunnall on 8/19/2015.
//
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

string build_version = "v0.1.0.21";
#define NUM_OPTIONS 6
string args[ NUM_OPTIONS ];

string OPTION = "";
int file_start = 0, fsize;

string packagefile = "lib";
bool verbose = false;

void setup()
{
   args[0] = "--help";
   args[1] = "-version";
   args[2] = "-a";
   args[3] = "-showversion";
   args[4] = "-v"; 
   args[5] = "-?";
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
   cout << "Source file must have a .scorpion or .sn extension to be packaged\n" << endl;
   cout << "[-options]\n\n    -version          print the current product version and exit." << endl;
   cout <<               "    -showversion      print the current product version and continue." << endl;
   cout <<               "    -a<file>          set the output archive file. Default is lib.sar." << endl;
   cout <<               "    -v                \"verbose mode\"; print out file packaging info." << endl;
   cout <<               "    --help -?         display this help message." << endl;
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
            cout << "Try 'sar --help' or 'sar -?' for more information.\n";
            exit(1);
         }
         else{  // time for running application
            fsize = (argc - file_start) - 1;
            return;
         }

         if(OPTION == "--help" || OPTION == "-?")
            help();
         else if(OPTION == "-version"){
            cout << "sar build version: \"" << build_version << "\"" << endl;
            exit(0);
         }
         else if(OPTION == "-showversion")
            cout << "sar build version: \"" << build_version << "\"" << endl;
         else if(OPTION == "-v")
            verbose=true;
         else if(OPTION == "-a"){
            i++;
            file_start++;
            
            if(!(i < argc)){
               cout << "Error: could not start sar. \nA fatal Error has occurred, shutting down." << endl;
               exit(1);   
            }
            
            data = args[i];
            packagefile = data;
         }

         file_start++;
   }
}
