#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

string build_version = "v1.0.9";
#define NUM_OPTIONS 7
string args[ NUM_OPTIONS ];

string OPTION = "";
int file_start = 0, fsize;
string distributionPackage = "", reinstallDir = "";

void setup()
{
   args[0] = "--help";
   args[1] = "-version";
   args[2] = "-showversion";
   args[3] = "-i";
   args[4] = "-r";
   args[5] = "-u";
   args[6] = "-?";
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
   cout << "Usage: install [-options]" << endl;
   cout << "or     install [-options] [distribution package]\n" << endl;
   cout << "Your distribution package must be a .tar.gz file.\n" << endl;
   cout << "[-options]\n\n    -version          print the current product version and exit" << endl;
   cout <<               "    -showversion      print the current product version and continue." << endl;
   cout <<               "    -i<file>          set the distribution package file to be installed." << endl;
   cout <<               "    -r<dir>           set the directory to witch to reinstall Scorpion." << endl;
   cout <<               "    -u                uninstall the current version of Scorpion on your machine." << endl;
   cout <<               "    --help -?         display this help message." << endl;
   exit(1);
}


bool _install= false, uninstall= false, reinstall= false;

void parseargs(int argc, const char **args)
{
      string data = "";
      for(int i = 1; i < argc; i++){
         data = args[i];

         if(isarg(data)) {}
         else if(hasdash(data))
         {
            cout << "Unrecognized command line option: " << data << endl;
            cout << "Try 'install --help' or 'install -?' for more information.\n";
            exit(1);
         }
         else{  // time for running application
            fsize = (argc - file_start) - 1;
            
            if(!(i < argc))
               return;
            
            _install = true;
            data = args[i];
            distributionPackage = data;
            return;
         }

         if(OPTION == "--help" || OPTION == "-?")
            help();
         else if(OPTION == "-version"){
            cout << "install build version: \"" << build_version << "\"" << endl;
            exit(0);
         }
         else if(OPTION == "-showversion")
            cout << "install build version: \"" << build_version << "\"" << endl;
         else if(OPTION == "-i"){
            i++;
            
            if(!(i < argc)){
               cout << "Error: could not start installer. \nA fatal Error has occurred, shutting down." << endl;
               exit(1);   
            }
            
            _install = true;
            data = args[i];
            distributionPackage = data;
         }
         else if(OPTION == "-r"){
            i++;
            
            if(!(i < argc)){
               cout << "Error: could not start installer. \nA fatal Error has occurred, shutting down." << endl;
               exit(1);   
            }
            
            reinstall = true;
            data = args[i];
            reinstallDir = data;
         }
         else if(OPTION == "-u")
            uninstall = true;
   }
}

