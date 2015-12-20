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
 #ifndef SCORPION_XSO_READER
 #define SCORPION_XSO_READER
 
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <signal.h>
 #include <assert.h>
 #include <iostream>
 #include <sstream>
 #include <string>
 #include <stdint.h>
 #include "xso.h"
 #include "../clib/binary.h"
 #include "../vm/log/log.h"
 #include "../clib/zlib/zlib.h"
 #include "xso_processor.h"
 
 using namespace std;
 using namespace scorpionvm::_xso;
 using namespace scorpionvm::log::log_service::debug;
 
 extern Log ldebug;
 namespace scorpionvm
 {
  
     namespace xsoreader
     {
         /*
        * ***************************************
        * ***************************************
        *       Header File Symbols
        *
        *         minor version: 0xAF
        *         major version: 0xAE
        *             file size: 0xFF
        *    target dev version: 0xBC
        *      minimum_dev_vers: 0xFD
        *        version_number: 0xCE
        *                 debug: 0xCC
        *               logging: 0xDB
        *        log precedence: 0xFA
        *              log file: 0x3C
        *        application id: 0xB5
        *           permissions: 0xC1
        *
        * ***************************************
        * ***************************************
        */
        extern int flag_minor_version;
        extern int flag_major_version;
        extern int flag_file_size;
        extern int flag_target_dev_vers;
        extern int flag_minimum_dev_vers;
        extern int flag_version_number;
        extern int flag_debugging;
        extern int flag_logging;
        extern int flag_log_precedence;
        extern int flag_log_file;
        extern int flag_application_id; 
        extern int flag_permissions;
        extern int flag_nameflag;
        extern int flag_methodsize;
        
         class xso_reader
         {
             public:
               xso_reader()
               : i(0),
                 _eof(false),
                 stream("")
               {
               }
               xso xso_file;
               scorpionvm::xsoprocessor::xso_processor processor;
               bool _eof;
               uint64_t i;
               std::string stream;
               
               char read_char() // the safe way to reade from a file stream
               {
                   if(i>stream.size())
                   {
                     _eof = true;
                     return -1;
                   }
                   return stream.at(i++);
               }
               
               string xso_image()
               {
                   stringstream ss;
                   for(uint64_t i2 =i; i2 < stream.size(); i++)
                       ss << stream.at(i2++) << "";
                       return ss.str();
               }
               
               string no_nln(string data)
               {
                   stringstream ss;
                   for(long i2 = 0; i2 < data.size(); i2++){
                       if(data.at(i2) == '\n'){ }
                       else
                         ss << data.at(i2);
                   }
                   return ss.str();
               }
               
               string xso_text(bool decode = false)
               {
                  char c;
                  stringstream ss;
                  for(long i2 = i; i2 < stream.size(); i2++){
                       c = read_char();
                       if(_eof){
                          ss.str("");
                          return "";
                       }
                          
                       if(c == 0)
                         break;
                       else
                         ss << c;
                   }
                  if(decode){    
                    stringstream stream_buf;
                    Binary::decode_str(ss.str(), stream_buf);
                    return stream_buf.str();
                  }
                  else 
                    return ss.str();
               }
               
               
               int ctoi(char c)
               {
                   switch( c ){
                       case '9':
                           return 9;
                       case '8':
                           return 8;
                       case '7':
                           return 7;
                       case '6':
                           return 6;
                       case '5':
                           return 5;
                       case '4':
                           return 4;
                       case '3':
                           return 3;
                       case '2':
                           return 2;
                       case '1':
                           return 1;
                       case '0':
                           return 0;
                       default:
                           return 0;
                   }
               }

               int read(const char* file)
               {  
                   xso_reader();
                   stream = FileStream::getfile(file);
                               
                   if((xso_file.xso_header.magic.byte1 = (long)read_char()) == (char) 0xdf && (xso_file.xso_header.magic.byte2 = (long)read_char()) == (char) 0x4e
                       && (xso_file.xso_header.magic.byte3 = (long)read_char()) == (char) 0xfa && (xso_file.xso_header.magic.byte4 = (long)read_char()) == (char) 0x2b)
                   {
                       i++;
                       if(read_char() == (char)flag_nameflag)
                       {
                           xso_file.xso_header.name = xso_text();
                           if(_eof) return 1;
                           i+=2;
                           
                           if(read_char() == (char)flag_methodsize)
                           {
                              xso_file.xso_header.method_size.byte1 = atol(xso_text(true).c_str());
                              if(_eof) return 1;
                              
                              if(read_char() == (char)flag_minor_version)
                              {
                                 xso_file.xso_header.minor_version.byte1 = (int) read_char();
                                 xso_file.xso_header.minor_version.byte2 = (int) read_char();
                                 i++;
                                 
                                 if(read_char() == (char)flag_major_version)
                                 {
                                    if((xso_file.xso_header.major_version.byte1 = read_char()) == (char)0x7 &&
                                       (xso_file.xso_header.major_version.byte1 = read_char()) == (char)0x10)
                                    {
                                       i++;
                                       if(read_char() == (char)flag_file_size)
                                       {
                                         xso_file.xso_header.filesize.byte1 = atol(xso_text(true).c_str());
                                         
                                         if(read_char() == (char)flag_target_dev_vers)
                                         {
                                            xso_file.xso_header.target_dev_vers.byte1 = (long) read_char();  // TODO: change to  -> atol(xso_text(true).c_str());
                                            i++;
                                            
                                            if(read_char() == (char)flag_minimum_dev_vers)
                                            {
                                               xso_file.xso_header.minimum_dev_vers.byte1 = (long) read_char();
                                               i++;
                                               
                                               if(read_char() == (char)flag_version_number)
                                               {
                                                  xso_file.xso_header.version_number = xso_text(true);
                                                  
                                                  if(read_char() == (char)flag_debugging)
                                                  {
                                                     xso_file.xso_header.debug.byte1 = ctoi(read_char());
                                                     i++;
                                                     
                                                     if(read_char() == (char)flag_logging)
                                                     {
                                                         xso_file.xso_header.logging.byte1 = ctoi(read_char());
                                                         i++;
                                                         
                                                         if(read_char() == (char)flag_log_precedence)
                                                         {
                                                             xso_file.xso_header.log_precedence.byte1 = ctoi(read_char());
                                                             i++;
                                                             
                                                             if(read_char() == (char)flag_log_file)
                                                             {
                                                                 xso_file.xso_header.log_file = xso_text(true);
                                                                 
                                                                 if(read_char() == (char)flag_application_id)
                                                                 {
                                                                     xso_file.xso_header.application_id = xso_text(true);
                                                                     i++;
                                                                     
                                                                     if(read_char() == (char)flag_permissions)
                                                                     {
                                                                         xso_file.xso_header.permissions = xso_text(true);
                                                                         i +=33;
                                                                         
                                                                         ldebug.LOGV("Xso processing successful, getting image.");
                                                                         
                                                                         Zlib zlib;
                                                                         stringstream __img_buf__;
                                                                         
                                                                         zlib.Decompress_Buffer2Buffer(xso_image(), __img_buf__);
                                                                         if(zres.response == ZLIB_FAILURE){
                                                                             cout << "\n" << zres.reason.str() << "Shutting down.\n";
                                                                             zlib.Cleanup();
                                                                             return 2;
                                                                         }
                                                                         else if(zres._warnings_.str() != "")
                                                                             cout << zres._warnings_.str();
                                                                         
                                                                         processor.image = no_nln(__img_buf__.str());
                                                                         processor.img_t = xso_file.xso_header.filesize.byte1;
                                                                         __img_buf__.str("");
                                                                         zlib.Cleanup();
                                                                         return 0;
                                                                     }
                                                                     else
                                                                       return 1;
                                                                 }
                                                                 else
                                                                   return 1;
                                                             }
                                                             else
                                                               return 1;
                                                         }
                                                         else
                                                           return 1;
                                                     }
                                                     else
                                                       return 1;
                                                  }
                                                  else
                                                    return 1;
                                               }
                                               else 
                                                 return 1;
                                            }
                                            else
                                              return 1;
                                         }
                                         else
                                           return 1;
                                       }
                                       else
                                         return 1;
                                    }
                                    else
                                      return 1;
                                 }
                                 else
                                   return 1;
                              }
                              else
                                return 1;
                           }
                           else
                             return 1;
                       }
                       else
                         return 1;
                   }
                   else
                     return 1;
               }
         };
     } // xsoreader
 } // end scorpionvm
 
 #endif // SCORPION_XSO_FILE