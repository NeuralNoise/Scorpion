/*
 * Copyright (C) 2015 The Scorpion Programming Language
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Portions of the code surrounded by "// Begin Dalvik code" and
 * "// END Delvik code" are copyrighted and licensed separately, as
 * follows:
 *
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements. The ASF licenses this file to You
 * under the Apache License, Version 2.0 (the "License"); you may not use 
 * this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "clib/filestream.h"
#include "clib/binary.h"
#include "clib/zlib/zlib.h"
#include "clib/u2.h"
#include "clib/u4.h"
#include "clib/u1.h"
#include "Archive.h"
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
using namespace std;

bool eof = false;
static std::string fcontent;
unsigned long fPos=0;

Zlib zlib;

char nextToken()
{
    if(fPos < fcontent.size())
      return fcontent.at(fPos++);
    else {
      eof = true;
      return -1;
    }
}


void Archive::clean()
{
    delete[] fmap;
    sourceFiles="";
    archiveHeader h;
    header =h;
}
      
int Archive::extract(const char* file)
{
    fcontent = FileStream::getfile(file);
    Zlib::AUTO_CLEAN=(true);
    
    if(((header.magic.byte1 = nextToken()) == (char) 0xaf) && ((header.magic.byte1 = nextToken()) == (char) 0x6c) &&
        ((header.magic.byte1 = nextToken()) == (char) 0xbe) && ((header.magic.byte1 = nextToken()) == (char) 0xf3)){
         fPos++;
         
         if((nextToken() == (char) 0xAF) && !((header.minor_version.byte1 = nextToken()) == (char) -1) && 
             !((header.minor_version.byte2 = nextToken()) == (char) -1)){
            fPos++;
            
            if(((header.major_version.byte1 = nextToken()) == (char) 0xCE) &&  ((header.major_version.byte1 = nextToken()) == (char) 0x10) && 
                ((header.major_version.byte1 = nextToken()) == (char) 0x11)){
                fPos++;
                
                if(nextToken() == (char) 0xFC){
                     stringstream ss;
                     for(int f = fPos; f < fcontent.size(); f++){
                        char token = nextToken();
                        if(eof)
                            return -2;
                                   
                        if(token != 0x0)
                            ss << token;
                        else{
                            break;
                        }
                     }
                        
                    stringstream __ostream_buf__;
                    Binary::decode_str(ss.str(), __ostream_buf__);
                        
                    header.sourcecount.byte1 = atoi(__ostream_buf__.str().c_str());
                    
                    if(nextToken() == (char) 0xBD){
                        stringstream sfiles;
                        for(int f = fPos; f < fcontent.size(); f++){
                            char token = nextToken();
                            
                            if(eof)
                               return -2;
                                   
                            if(token != 0x0)
                               sfiles << token;
                            else
                               break;
                        }
                        
                        sourceFiles = sfiles.str();
                        fPos += 12 + 1 + 20; // skip null
                        
                        // get packaged file content
                        stringstream __buf__;
                        char c;
                        
                        do {
                          c = nextToken();
                          if(!eof)
                              __buf__ << c;
                        }
                        while(!eof);
                        
                        int rez=FileStream::out("/usr/share/scorpion/lib/lib-tmp.zip", __buf__.str());
                        __buf__.str("");
                            
                        stringstream __ostream_buf__;
                        zlib.Decompress_File2Buffer("/usr/share/scorpion/lib/lib-tmp.zip", __ostream_buf__);
                            
                        if(rez==0)
                            remove( "/usr/share/scorpion/lib/lib-tmp.zip" );
                               
                        if(zres._warnings_.str() != "")
                            cout << zres._warnings_.str();
                               
                        if(zres.response == ZLIB_FAILURE){
                            fcontent="";
                            cout << "\n" << zres.reason.str() << "Error is not recoverable, exiting with exit status: -1.\n";
                            zlib.Cleanup();
                            exit(-1);
                        }
                        
                        if(unpack(__ostream_buf__.str()) != 0)
                            return -1;
                      
                      __ostream_buf__.str("");
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
    else if(eof)
        return -2;
    else
        return -3;
    
    zlib.Cleanup();
    fcontent="";
    return 0;
}

string toinclude(string str)
{
    int pushback=0;
    
    for(int i = str.size() - 1; i > 0; i--)
    {
        if(str.at(i) == '.')
           break;
        else
          pushback++;
    }
    
    stringstream ss;
    for(int i = 0; i < str.size() - pushback - 1; i++)
    {
        if(str.at(i) == '/' || str.at(i) == '\\'){
            ss << ".";
        }
        else
          ss << str.at(i);
    }
    
    return ss.str();
}

int Archive::splitfiles()
{
    int scount = 0;
    stringstream str;
    for(int i = 0; i < sourceFiles.size(); i++){
        if(sourceFiles.at(i) == ';'){
            if(str.str() != ""){
                if(scount >= header.sourcecount.byte1){
                    cout << "sar:  error: there are more source files than expected ammount.\n";
                    return -1;
                }
                
                fmap[scount].name = str.str();
                fmap[scount++].include = toinclude(str.str());
                str.str("");
            }
        }
        else
          str << sourceFiles.at(i);
    }
    
    if(scount != header.sourcecount.byte1){
       cout << "sar:  error: source files do not meet the expected size.\n";
       return -1;
    }
    
    return 0;
}

int Archive::unpack(string __outbuf__)
{
    fmap = new FileMap[header.sourcecount.byte1];
    
    if(splitfiles() != 0)
       return -1;
       
    
    long filecount = 0;
    
    stringstream filecontent;
    if(!(__outbuf__.size() >0))
    {
        cout << "sar:  error: this does not look like a packaged file.\n";
        return -1;
    }
    else if(__outbuf__.at(0) != (char) 0xAA){
        cout << "sar:  error: this does not look like a packaged file.\n";
        return -1;
    }
    
    for(long count = 1; count < __outbuf__.size(); count++){
        
        if((__outbuf__.at(count) == ((char) 0xAA)) || !((count + 1) < __outbuf__.size())){
            stringstream __cmp_buf__, _decompile_buf__;
            Binary::decode_str(filecontent.str(), _decompile_buf__);
            
         //   cout << "file " << fmap[filecount].name << "\ncontents " << _decompile_buf__.str() << endl;
            zlib.Compress_Buffer2Buffer(_decompile_buf__.str(), __cmp_buf__, ZLIB_LAST_SEGMENT);
            filecontent.str("");
            
            if(zres._warnings_.str() != "")
                cout << zres._warnings_.str();
                               
            if(zres.response == ZLIB_FAILURE){
                fcontent="";
                cout << "\n" << zres.reason.str() << "Error is not recoverable, exiting with exit status: -1.\n";
                zlib.Cleanup();
                exit(-1);
            }
            
            fmap[filecount++].contents = __cmp_buf__.str();
            __cmp_buf__.str("");
            _decompile_buf__.str("");
        }
        else
          filecontent << __outbuf__.at(count);
    }
    
    return 0;
}

