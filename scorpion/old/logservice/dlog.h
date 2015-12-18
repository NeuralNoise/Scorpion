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
#ifndef DLOG
#define DLOG

#include <string>
#include <iostream>
#include <sstream>
#include <new>
#include "Log.h"
#include "../vm/exception.h"
#include "../libxso/xso.h"
#include "../clib/filestream.h"
using namespace std;

 class DLog {
   int precedence;
   string file, stackmsg;
   bool on;
   long bufferthreshold;
   
   public:
     void setup(XSO x, int p){
         Exception::trace.addproto("vm.internal.system.setupUsrLogService", "DLOG", 58, 1);
         file = x.headerInf.log_file;
         on = x.headerInf.logging.byte1;
         bufferthreshold = 126 *(1000); // 126kb
         
         if(on) {
            int status = FileStream::out(file.c_str(), "");
   
            if(status != 0){
               stringstream ss;
               ss << "warning: I/O err, unable to open application log file: '" << file << "'!\n";
               cout << ss.str();
               on = false; // we do this to skip logging
               return;
            }
            
            precedence = p;    
            stackmsg = "";
            stackmsg += "=======System Dev Log begin=======\n";
         }
     }
     string getFile(){
        return file;
     }
     void DLOGA(string klass, string message){
         stackmsg = Log::a(klass, message, file, on, stackmsg, precedence);
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";
     }
     void DLOGE(string klass, string message){
         stackmsg = Log::e(klass, message, file, on, stackmsg, precedence);
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";
     }
     void DLOGW(string klass, string message){
         stackmsg = Log::w(klass, message, file, on, stackmsg, precedence);  
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";
     }
     void DLOGI(string klass, string message){
         stackmsg = Log::i(klass, message, file, on, stackmsg, precedence);
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = ""; 
     }
     void DLOGD(string klass, string message){
         stackmsg = Log::d(klass, message, file, on, stackmsg, precedence);
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";  
     }
     void DLOGV(string klass, string message){
         stackmsg = Log::v(klass, message, file, on, stackmsg, precedence);
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";  
     }
 };

#endif // DLOG
