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
#include "scorpionc.h"
#include "clib/filestream.h"
#include "compiler.h"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
using namespace std;

Compiler compiler;

int main(int argc, const char **argv)
{
    int optionCount, argIdx, res=0;
    int needExtra = 0;
    std::string lastFlag;
    string problemFlag;
    
    setup();
    
    if(argc >= 2){
    
       needExtra = 0;
       for (argIdx = 1; argIdx < argc; argIdx++) {
          lastFlag = argv[argIdx];

            /* some options require an additional arg */
            if ((lastFlag == "-o") ||
                (lastFlag == "--build") ||
                (lastFlag == "-Xsz") ||
                (lastFlag == "-O"))
                /* others? */
            {
                if(!((argIdx + 1) < argc)){
                    problemFlag = lastFlag;
                    needExtra = 1;
                    break;
                }
                
                string s = argv[argIdx + 1];
                if(s.at(0) == '-'){
                    problemFlag = lastFlag;
                    needExtra = 1;
                }
            }
        }

    
        if (needExtra != 0) {
            printf("error:  Faulty Argument flag. Scorpion compiler requires a value after option flag: %s\n", problemFlag.c_str());
            return -1;
        }
    
       parseargs(argc, argv);
    }
    else if(argc == 1)
       help();
 
    if(options.ags_t > 0){
        int index = 0;
        string *files = new string[options.ags_t];
        for(int i = file_start + 1; i < argc; i++){
            stringstream ss;
            ss << argv[i];
            files[index++] = ss.str(); 
        } 
        
        if(options.build_file != ""){
             int ret = 0;
             for(int i = 0; i < options.ags_t; i++){
                 if(!FileStream::exists(files[i].c_str())){
                     cout << "Error: file '" << files[i] << "' is no such file." << endl;
                     ret = -1;
                 }
                 else{
                     if(!FileStream::endswith(".scorpion", files[i]) && !FileStream::endswith(".sn", files[i])){
                           cout << "Error: file '" << files[i] << "' is not a scorpion source file." << endl;
                           ret = -1;
                     }
                 }
             }
             
             if(FileStream::endswith(".dev", options.build_file)){
                  if(FileStream::exists(options.build_file.c_str())){
                      if(ret != 0){
                         cout << "Shutting down." << endl;
                         return -1;
                     }
                     /* Load compiler libraries */
                     compiler.libSize(1);
                     
                     res+=compiler.loadLibrary("lib/lib-compiler-scorpionc-0.1.0.0.sar");
                     
                     if(res == 0){
                       if(compiler.compile() != 0)
                          return -1;
                       
                       // run compiled file
                       // system("scorpion compiler.xso [args..]");
                     }
                     else
                       return res;
                  }
                  else{
                     cout << "Error: could not locate build script file. \nA fatal Error has occurred, shutting down." << endl;
                     return -1;
                  }
             }
             else{
                 cout << "Error: build file is not a Dev script file!. Shutting down." << endl;
                 return -1;
              }
        }
        else {
           cout << "Error: build script file not provided. \nA fatal Error has occurred, shutting down." << endl;
           return -1;
        }
    }
    else {
      cout << "Error: could not start Scorpion compiler. \nA fatal Error has occurred, shutting down." << endl;
      return -1;
    }
 
   return 0;   
}


