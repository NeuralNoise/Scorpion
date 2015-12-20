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
 #ifndef SCORPION_DEBUG_LOG_SREVICE
 #define SCORPION_DEBUG_LOG_SREVICE
 
 #include <sstream>
 #include "log_service.h"
 #include "../../clib/filestream.h"
 
 using namespace scorpionvm::log::log_service;
 
 namespace scorpionvm
 {
     namespace log
     {
         namespace log_service
         {
             namespace debug
             {
                 #define ENV_DEV
                //  #define ENV_RUNTIME
                 class Log
                 {
                     public:
                       Log()
                       : precedence(VERBOSE),
                         file(""),
                         logstack(""),
                         package("vm.internal"),
                         basepackage("vm.internal"),
                         on(false),
                         bufferthreshold(124 * (1000)) // 124 kb
                       {
                       }
                     
                       void service_setup(int level, bool usr, string _file)
                       {
                          if(on) return; // you cant setup the log service more than once
                          #ifdef ENV_DEV
                            file = "/usr/share/scorpion/vm/log/out.log";
                          #endif
                          #ifdef ENV_RUNTIME
                            file = "/usr/share/scorpion/sre/vm/log/out.log";
                          #endif
                          
                          if(level == ASSERT+1)
                            on=false;
                          else 
                            on=true;
                            
                          if(on) {
                             int status = FileStream::out(file.c_str(), "");
                   
                             if(status != 0){
                                cout << "warning: I/O err, unable to open system log file!\n";
                                on = false; // we do this to skip logging
                                return;
                             }
                             
                             if(usr) file = _file;
                             precedence = level;
                             logstack = "";
                             logstack += "=======System Dev Log begin=======\n";
                             
                             stringstream ss;
                             ss << "Log setup: using " << file;
                             LOGV(ss.str());
                          }
                       }
                       void setpackage(string name){
                          package = name;
                       }
                       void LOGA(string message, string class_ = "System"){
                          a(class_, message, file, on, logstack, precedence, package);
                          if(logstack.size() >= bufferthreshold)
                             logstack = "";
                          package = basepackage;
                       }
                       void LOGE(string message, string class_ = "System"){
                          e(class_, message, file, on, logstack, precedence, package);
                          if(logstack.size() >= bufferthreshold)
                             logstack = "";
                          package = basepackage;
                       }
                       void LOGW(string message, string class_ = "System"){
                          w(class_, message, file, on, logstack, precedence, package);  
                          if(logstack.size() >= bufferthreshold)
                             logstack = "";
                          package = basepackage;
                       }
                       void LOGI(string message, string class_ = "System"){
                          i(class_, message, file, on, logstack, precedence, package);
                          if(logstack.size() >= bufferthreshold)
                             logstack = "";
                          package = basepackage;
                       }
                       void LOGD(string message, string class_ = "System"){
                          d(class_, message, file, on, logstack, precedence, package);
                          if(logstack.size() >= bufferthreshold)
                             logstack = "";
                          package = basepackage;
                       }
                       void LOGV(string message, string class_ = "System"){
                          v(class_, message, file, on, logstack, precedence, package);  
                          if(logstack.size() >= bufferthreshold)
                             logstack = "";
                          package = basepackage;
                       }
                     
                     private:
                       int precedence;
                       string file, logstack, 
                              package, basepackage;
                       bool on;
                       long bufferthreshold;
                 };
             } // end debug
         } // end log_service
     } // end log
 } // end scorpionvm
 
 #endif // SCORPION_DEBUG_LOG_SREVICE