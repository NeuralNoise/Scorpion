
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
#ifndef SCORPION_C_H
#define SCORPION_C_H

#include "clib/filestream.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class Compiler {
    
    string* libraries;
    int size_t, libPos;
    
    public:
      void libSize(int size){
          size_t = size;
          libPos = 0;
          libraries = new string[size];
      }
      int loadLibrary(string lib){
        stringstream file;
        file << "/usr/share/scorpion/" << lib;
        
          if(libPos >= size_t){
              cout << "scorpionc:  error: failure to load library " << lib << " max library limit reached.\n";
              return -1;
          }
          
          if(FileStream::endswith(".sar", file.str())){
              if(FileStream::exists(file.str().c_str()))
                  libraries[libPos++] = file.str();
              else {
                  cout << "scorpionc:  error: library " << lib << " is is no such file.\n";
                  return -1;
              }
          }
          else {
              cout << "scorpionc:  error: library " << lib << " is not a Scorpion archive file.\n";
              return -1;
          }
          
          return 0;
      }
      int compile();
};

#endif // SCORPION_C_H


