//
// Created by bnunnall on 8/19/2015.
//

#include "sar.h"
#include "sarheader.h"
#include "clib/filestream.h"
#include "clib/time.h"
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

int main(int argc, const char**args)
{
    setup();
    c_time.reset();
    
    if(argc >= 2)
       parseargs(argc, args);
    else if(argc == 1)
       help();

    if(fsize > 0){
        int index = 0;
         string *files = new string[fsize];
         for(int i = file_start + 1; i < argc; i++){
             stringstream ss;
             ss << args[i];
             files[index++] = ss.str(); 
         } 
         
         if(sarheader::makeheader(fsize, files) == 0){
              delete[] files;
              
              printf("\rsar:  packaging %lu file(s), %lu total bytes of data. (%lu/%lu)", h.sourcecount.byte1, h.filesize.byte1, bytestatus, h.filesize.byte1);
              
              stringstream pfiles;
              pfiles << packagefiles() << (char) 0;
              
              archive << pfiles.str();
              stringstream fout;
              fout << packagefile << ".sar";
              FileStream::out(fout.str().c_str(), archive.str());
              archive.str("");
              fout.str("");
              cout << endl;
              printf("total time: %.3fs\n", c_time.gettime());
         }
         else
            return -1;
    }
    else
      cout << "Error: could not start sar. \nA fatal Error has occurred, shutting down." << endl;
    return 0;
}

