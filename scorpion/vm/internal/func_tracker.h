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

 using namespace std;
 
 namespace scorpionvm
 {
     struct func
     {
         std::string name, _class,
                     package;
         uint64_t line;
         bool native_func;
     };
     
     int max_func_size = 20;
     
     class function_tracker
     {
         public:
            function_tracker()
            {
                function_list._init_();
            }
            void add_func(std::string name, std::string _class, std::string package, 
                    uint64_t call_line, bool native)
            {
                func f;
                f.name = name;
                f._class = _class;
                f.package = package;
                f.line = call_line;
                f.native_func = native;
                
                if(function_list.size() > max_func_size)
                  function_list.remove(0); // remove first function
                function_list.add(f);
            }
            
         private:
           ListAdapter<func> function_list;
     };
 } // end ScorpionVM
 
 #endif // 