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

Zlib zlib;

int getper(long data, long size)
{
    double a = 1.0, b = 1.0, c;
    a = a*data;
    b = b*size;
    
    c = (a/b) * 100;
    return (int) c;
}

extern bool verbose;
long _stdin=0, _stdout=0;

string packagefiles()
{
    stringstream __instream_buf__, __ostream_buf__;
    Zlib::AUTO_CLEAN=(true);
    
    printf("sar:  packaging %lu file(s), %lu total bytes of data.\n", h.sourcecount.byte1, h.filesize.byte1);
    
    for(int i = 0; i < h.sourcecount.byte1; i++){
        __instream_buf__ << (char) _next_file;
        
        if(verbose)
           cout << "adding: " << h.sourcefiles[i] << std::flush;
           
        Binary::encode_str(FileStream::getfile(h.sourcefiles[i].c_str()), __instream_buf__);
        
        if(!((i + 1) < h.sourcecount.byte1))
          zlib.Compress_Buffer2Buffer(__instream_buf__.str(), __ostream_buf__, ZLIB_LAST_SEGMENT);
        else
          zlib.Compress_Buffer2Buffer(__instream_buf__.str(), __ostream_buf__, ZLIB_NEXT_SEGMENT);
        
        if(verbose)
           cout << " (stdin=" << zres.size_t.byte1 << ") (stdout=" << zres.size_t.byte2 << ")  comp. ratio (" << zres.compressionRatio << "%)\n" << std::flush;
        
        if(zres.response == ZLIB_FAILURE){
           cout << "\n" << zres.reason.str() << "Shutting down.\n";
           exit(0);
        }
        else if(zres._warnings_.str() != "")
           cout << zres._warnings_.str();
        
        _stdin+=zres.size_t.byte1;
        _stdout+=zres.size_t.byte2;
        
        __instream_buf__.str("");
    }
    
    cout << "overall: (stdin=" << _stdin << ") (stdout=" << _stdout << ")  comp. ratio (" << (((float) _stdout / (float) _stdin)*10000) << "%)\n";
    return __ostream_buf__.str();
}


