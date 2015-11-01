#include "clib/filestream.h"
#include "libheader.h"
#include "clib/binary.h"
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>  
#include <stdio.h>
#include <unistd.h>
using namespace std;

lheader lh;
string lib;
stringstream library;
string s_files;
long lastchar = 1, len;

int unpack(string file);

bool equals(char c, long x)
{
    return c == ((char) x);
}

char get(long i)
{
    if(i < len)
      return lib.at(i);
    else
      return (char) 25935;
}

int readheader(string file)
{
    int i = 0;
    lib = "";
    lib = file;
    len = file.size();
    
    // magic number
    if(equals(get(i++), 0xaf) && equals(get(i++), 0x6c) 
       && equals(get(i++), 0xbe) && equals(get(i++), 0xf3)){
           
           i++;
           // get minor version
           if(equals(get(i++), 0xAF) && !equals(get(i++), 25935) && !equals(get(i++), 25935)){
               
               i++;
               // get major version
                if(equals(get(i++), 0xCE) && equals(get(i++), 0x10) && equals(get(i++), 0x11)){
                    i++;
                    // source count
                    if(equals(get(i++), 0xFC)){
                        stringstream ss;
                        for(int f = i; f < file.size(); f++){
                            if(get(i) == 25935)
                                  return -1;
                                   
                            if(get(i) != 0x0)
                                ss << get(i++);
                            else{
                                i++;
                                break;
                            }
                        }
                        stringstream __ostream_buf__;
                        Binary::decode_str(ss.str(), __ostream_buf__);
                        
                        lh.sourcecount.byte1 = atoi(__ostream_buf__.str().c_str());
                        // source files
                        if(equals(get(i++), 0xBD)){
                            stringstream sfiles;
                            for(int f = i; f < file.size(); f++){
                                
                                if(get(i) == 25935)
                                   return -1;
                                   
                                if(get(i) != 0x0)
                                    sfiles << get(i++);
                                else
                                  break;
                            }
                            s_files = sfiles.str();
                            i += 34; // skip null
                            
                        //    cout << "sfiles " << s_files << endl;
                            // get packaged file content
                            stringstream l;
                            for(int f = i; f < file.size(); f++){
                                
                                if(get(i) == 25935)
                                   return -1;
                                   
                                if(get(i) != 0x0)
                                    l << get(i++);
                                else
                                  break;
                            }
                            
                            if(unpack(l.str()) != 0)
                               return -1;
                        }
                        else
                           return -1;
                    }
                    else
                       return -1;
                }
                else
                   return -1;
           }
           else
              return -1;
    }
    else
       return -1;
    
    return 0;
}

long bytestatus = 0, bytesize = 0;


int splitfiles()
{
    int scount = 0;
    stringstream str;
    for(int i = 0; i < s_files.size(); i++){
        if(s_files.at(i) == ';'){
            if(str.str() != ""){
                fmap[scount++].name = str.str();
                str.str("");
            }
        }
        else
          str << s_files.at(i);
    }
    
    if(scount != lh.sourcecount.byte1)
       return -1;
    return 0;
}


string rm_nwln(string data){
    stringstream ss;
    for(int i =0; i < data.size(); i++){
        if((data.at(i) == '\n') || (data.at(i) == 10)){ }
        else
          ss << data.at(i);
    }
    return ss.str();
}

FileMap *fmap;

int unpack(string file)
{
    fmap = new FileMap[lh.sourcecount.byte1];
    //bytesize = skip_nwln(file) - (lh.sourcecount.byte1);
    
    if(splitfiles() != 0)
       return -1;
    
    long filecount = 0;
    
    // TODO: Take out new lines in unpackaged content
    stringstream filecontent;
    for(long count = lastchar; count < file.size(); count++){
        
        if((file.at(count) == ((char) 0xAA)) || !((count + 1) < file.size())){
            fmap[filecount++].contents = rm_nwln(filecontent.str());
            filecontent.str("");
        }
        else
          filecontent << file.at(count);
        lastchar++;
    }
    
    if(filecount == lh.sourcecount.byte1){
        
         for(int i = 0; i < lh.sourcecount.byte1; i++)
             bytesize += fmap[i].contents.size();
         bytesize += lh.sourcecount.byte1 + 1;    
         
         filecount = 0;
         printf("\rsar:  unpacking %lu file(s), %lu total bytes of data. (%lu/%lu)", lh.sourcecount.byte1, bytesize, bytestatus, bytesize);
         for(int i = 0; i < lh.sourcecount.byte1; i++){
               stringstream __ostream_buf__;
               Binary::decode_str(fmap[filecount].contents, __ostream_buf__);
               fmap[filecount].contents = __ostream_buf__.str();
               filecount++;
         }
    }
    else
      return -1;
    return 0;
}
