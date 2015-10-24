#include "sarheader.h"
#include "clib/filestream.h"
#include "clib/random.h"
#include "clib/binary.h"       // text encoding algorithm
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


string packagefiles()
{
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
    for(int i = 0; i < contents.size(); i++){
        bytestatus++;
        printf("\rformatting file... (%lu) bytes", bytestatus);
          package << contents.at(i);
          if(check <= 0){
               check = rand_n(243) + (rand() % 32);
               package << "\n";
          }
          else 
            check--;
    }
    
    // zip data before returning
    return package.str();
}