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
#ifndef ALOG
#define ALOG

#include <string>
#include <iostream>
#include <sstream>
#include "Log.h"
#include "../clib/filestream.h"
using namespace std;

 class ALog {
   int precedence;
   string file, stackmsg, Class, BaseClass;
   bool on;
   long bufferthreshold;
   
   public:
     void setup(int p){
         file= "/usr/share/scorpion/vm/log/out.log";
         bufferthreshold = 126 * (1000);  // 126 kb
         Class = "System";
         BaseClass = Class;
         on = true;

         int status = FileStream::out(file.c_str(), "");

         if(status != 0){
            cout << "warning: I/O err, unable to open system log file!\n";
            on = false; // we do this to skip logging
            return;
         }
            
         precedence = p;
         stackmsg = "";
         stackmsg += "=======System Dev Log begin=======\n";
     }
     void setClass(string name){
         Class = name;
     }
     string getFile(){
        return file;
     }
     void ALOGA(string message){
         stackmsg = Log::a(Class, message, file, on, stackmsg, precedence);
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";
         Class = BaseClass;
     }
     void ALOGE(string message){
         stackmsg = Log::e(Class, message, file, on, stackmsg, precedence);
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";
         Class = BaseClass;
     }
     void ALOGW(string message){
         stackmsg = Log::w(Class, message, file, on, stackmsg, precedence);  
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";
         Class = BaseClass;
     }
     void ALOGI(string message){
         stackmsg = Log::i(Class, message, file, on, stackmsg, precedence);
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";
         Class = BaseClass; 
     }
     void ALOGD(string message){
         stackmsg = Log::d(Class, message, file, on, stackmsg, precedence);
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";
         Class = BaseClass;  
     }
     void ALOGV(string message){
         stackmsg = Log::v(Class, message, file, on, stackmsg, precedence);  
         if(stackmsg.size() >= bufferthreshold)
            stackmsg = "";
         Class = BaseClass;
     }
 };

 extern ALog alog;

#endif // ALOG
