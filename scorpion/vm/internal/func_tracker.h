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
 #ifndef SCORPION_FUNCTION_TRACKER
 #define SCORPION_FUNCTION_TRACKER
 
 #include <string>
 #include <sstream>
 #include <sys/time.h>
 #include <stdint.h>
 #include <stdio.h>
 #include "../../clib/arraylist.h"
 
 using namespace std;
 
 namespace scorpionvm
 {
     struct func
     {
         std::string name, file, _class,
                     package;
         uint64_t line;
         bool native_func;
     };
     
     class function_tracker
     {
         public:
            function_tracker()
            {
                function_list._init_();
            }
            void add_func(std::string name, std::string file, std::string package, 
                    std::string _class, uint64_t call_line, bool native)
            {
                func f;
                f.name = name;
                f.file = file;
                f.package = package;
                f.line = call_line;
                f.native_func = native;
                f._class = _class;
                
                if(function_list.size() > 20)
                  function_list.remove(0); // remove first function
                function_list.add(f);
            }
            
            string get_func_trace()
            {
                stringstream ss;
                ss << "Traceback (most recent call last)\n";
                for(uint64_t i=0; i < function_list.size(); i++)
                {
                    ss << "at " << function_list.valueAt(i).package 
                        << "." << function_list.valueAt(i)._class << "." 
                          << function_list.valueAt(i).name << "(";
                          
                    if(function_list.valueAt(i).native_func)
                      ss << "Native Function)\n";
                    else
                    {
                      ss << function_list.valueAt(i).file << ":"
                          << function_list.valueAt(i).line << ")\n";
                    }
                }
                return ss.str();
            }
            
         private:
           ListAdapter<func> function_list;
     };
 } // end ScorpionVM
 
 #endif // SCORPION_FUNCTION_TRACKER