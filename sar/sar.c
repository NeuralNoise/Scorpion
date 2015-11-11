//
// Created by bnunnall on 8/19/2015.
//

#include "sar.h"
#include "sarheader.h"
#include "clib/filestream.h"
#include "clib/time.h"
#include "clib/zlib/zlib.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;


extern string packagefile;
extern long bytestatus;
extern long bytes;
extern string packagefiles();

Timer c_time;
extern Zlib zlib;

int main(int argc, const char**argv)
{
    int optionCount, argIdx, res=0;
    int needExtra = 0;
    std::string lastFlag;
    string problemFlag;
    
    setup();
    c_time.reset();
    
    if(argc >= 2){
    
       needExtra = 0;
       for (argIdx = 1; argIdx < argc; argIdx++) {
          lastFlag = argv[argIdx];

            /* some options require an additional arg */
            if ((lastFlag == "-a"))
                /* others? */
            {
                if(!((argIdx + 1) < argc)){
                    problemFlag = lastFlag;
                    needExtra = 1;
                    break;
                }
                
                string s = argv[argIdx + 1];
                if(s.at(0) == '-'){
                    problemFlag = lastFlag;
                    needExtra = 1;
                }
            }
        }

    
        if (needExtra != 0) {
            printf("error:  Faulty Argument flag. Sar requires a value after option flag: %s\n", problemFlag.c_str());
            return -1;
        }
    
       parseargs(argc, argv);
    }
    else if(argc == 1)
       help();

    if(fsize > 0){
        int index = 0;
         string *files = new string[fsize];
         for(int i = file_start + 1; i < argc; i++){
             stringstream ss;
             ss << argv[i];
             files[index++] = ss.str(); 
         } 
         
         if(sarheader::makeheader(fsize, files) == 0){
              delete[] files;
              
              archive << packagefiles();
              stringstream fout;
              fout << packagefile << ".sar";
              FileStream::out(fout.str().c_str(), archive.str());
              archive.str("");
              fout.str("");
              printf("total time: %.3fs\n", c_time.gettime());
              zlib.Cleanup();
         }
         else
            return -1;
    }
    else
      cout << "Error: could not start sar. \nA fatal Error has occurred, shutting down." << endl;
    return 0;
}

