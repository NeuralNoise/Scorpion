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
 
 #include <string>
 #include <stdint.h>
 #include "xso.h"
 
 using namespace std;
 using namespace scorpionvm::_xso;
 
 namespace scorpionvm
 {
     namespace xsoreader
     {
         enum
         {
             eof=-0xfff
         };
         
         class xso_reader
         {
             public:
               xso_reader()
               : i(0)
               {
               }
               xso xso_file;
               uint64_t i;
               std::string stream;
               
               int read_char() // the safe way to reade from a file stream
               {
                   if(i>stream.size())
                     return eof;
                   return stream.at(i++);
               }
               
               int read(std::string& byte_stream)
               {
                   stream = byte_stream;
                   byte_stream="";
                   
                   if((xso_file.xso_header.magic.byte1 = read_char()) == 0xdf && (xso_file.xso_header.magic.byte2 = read_char()) == 0x4e
                       && (xso_file.xso_header.magic.byte1 = read_char()) == 0xfa && (xso_file.xso_header.magic.byte2 = read_char()) == 0x2b)
                   {
                       cout << "success!";
                   }
                   else
                     return 1;
               }
               
               void print_data()
               {
                   
               }
         };
     } // xso
 } // end scorpionvm
 
 #endif // SCORPION_XSO_FILE