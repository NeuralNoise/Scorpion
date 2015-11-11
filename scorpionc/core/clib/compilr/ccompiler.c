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
 * The core software depicted in this library goes to 
 * the creator of SCZ(Simple Compression Utilities and Library)
 *
 * (http://scz-compress.sourceforge.net/) November 26, 2008
 *
 */
 #include "ccompiler.h"
 #include "compilr.h"
 #include "../zlib/zlib.h"
 #include "../binary.h"
 #include "core.h"
 #include "../filestream.h"
 #include <sstream>
 #include <string>
 #include <iostream>
 
 using namespace std;
 
 int Compilr_Compile_Buf(string buf, stringstream &out_buf)
 {
     out_buf << "hello";
     return 0;
 }
 
 // TODO: Spruce up sar and scorpionc before release sar->(clib) scorpionc->(compiler src files)
 int Cmplr_Compile_File( string infilename, char *outfilename )
 {
     if(FileStream::endswith(".sn", infilename.c_str()) || FileStream::endswith(".scorpion", infilename.c_str()))
     {
         if(FileStream::exists(infilename.c_str()))
         {
             if(!FileStream::empty(infilename.c_str()))
             {
                int success=0;
                if(!init)
                {
                   success = cmplr_init();
                   if(success!=0){  return COMPILR_FAILURE; }
                }
                else
                {
                   cmplr_cleanup( false );
                   cplrfreelist2 = new_cmplr_item2();
                }
              
                stringstream __out_buf__;
                success= Compilr_Compile_Buf(FileStream::getfile(infilename.c_str()), __out_buf__);
              
                // TODO: Convert to object file
                if(success==0)
                {
                   stringstream outf, __new_buf__, version, obj_size_t, buf;
                   Binary::encode_str("0.1.0.0", version);
                   
                   buf << cres.size_t.byte1;
                   Binary::encode_str(buf.str(), obj_size_t);
                   
                   __new_buf__ << (char) 0xBA << (char) 0xCA << (char) 0xFE << (char) 0xBE; // add 4 byte magic number 
                   
                   for(int i = 0; i < 3; i++)
                    __new_buf__ << (char) 0x0; // add null dummy data
                    
                   __new_buf__ << (char) 0x0 << version.str() << (char) 0x0 << obj_size_t.str() << (char) 0x0; // add version number and object size
                   for(int i = 0; i < 10; i++)
                    __new_buf__ << (char) 0x0; // add null dummy data
                   
                   __new_buf__ << endl << (char) 0x0; 
                   
                   Zlib zlib;
                   zlib.Compress_Buffer2Buffer(__out_buf__.str(), __new_buf__, ZLIB_LAST_SEGMENT);
                   outf.str("");
                   
                   if(zres.response == ZLIB_FAILURE){
                      cout << "\n" << zres.reason.str() << "Shutting down.\n";
                      return ZLIB_FAILURE;
                   }
                   else if(zres._warnings_.str() != "")
                      cout << zres._warnings_.str();
                   
                   outf << outfilename << ".o";
                   success=FileStream::out(outf.str().c_str(), __new_buf__.str());
                   
                   if(success != 0)
                   {
                      cres.reason << "compilr:  error: could not output to object file '" << outf.str() << "'.\n";
                      __out_buf__.str("");
                      __new_buf__.str("");
                      return ZLIB_FAILURE;
                   }
                   __new_buf__.str("");
                }
                
                __out_buf__.str("");
                
                return success;
             }
             else {
                 cres._warnings_ << "compilr:  warning: file " << infilename << " is empty.\n";
                 return 0;
             }
         }
         else {
             cres.reason << "compilr:  error: file " << infilename << " does not exist.\n";
             return COMPILR_FAILURE;
         }
     }
     else {
         cres.reason << "compilr:  error: file " << infilename << " is not a Scorpion source file.\n";
         return COMPILR_FAILURE;
     }
 }
 
 