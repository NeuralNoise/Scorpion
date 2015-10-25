#include "sarheader.h"
#include "clib/filestream.h"
#include "clib/random.h"
#include "clib/binary.h"       // text encoding algorithm
#include "clib/zlib/zlib.h"
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;

long bytestatus = 0; 
extern int _next_file;
int check = 0;
stringstream pkg;

Zlib zlib;

int getper(long data, long size)
{
    double a = 1.0, b = 1.0, c;
    a = a*data;
    b = b*size;
    
    c = (a/b) * 100;
    return (int) c;
}

string packagefiles()
{
    Zlib::AUTO_CLEAN=(true);
    string contents;
    for(int i = 0; i < h.sourcecount.byte1; i++){
        contents = FileStream::getfile(h.sourcefiles[i].c_str());
        pkg << (char) _next_file;
        pkg << Binary::encode_str(contents);
    }
    
    bytestatus = 0;
    stringstream package;
    contents = pkg.str();
    pkg.str("");
    cout << endl;
    long len = contents.size();
    
    for(int i = 0; i < len; i++){
        bytestatus++;
        printf("\rformatting file... (%d%s)", getper(bytestatus, len), "%");
          package << contents.at(i);
          if(check <= 0){
               check = rand_n(243) + (rand() % 32);
               package << "\n";
          }
          else 
            check--;
    }
    
    cout << "\ncompressing...  " << std::flush;
    
    stringstream __ostream_buf__;
    zlib.Compress_Buffer2Buffer(package.str(), __ostream_buf__, ZLIB_LAST_SEGMENT);
    
    if(zres.response == ZLIB_FAILURE){
       cout << "\n" << zres.reason.str() << "Shutting down.\n";
       exit(0);
    }
    else { cout << "done."; return __ostream_buf__.str(); }
    return package.str();
}