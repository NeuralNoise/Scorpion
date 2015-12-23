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
 #ifndef SCORPION_RUNTIME_EXCEPTION
 #define SCORPION_RUNTIME_EXCEPTION
 
 #include <string>
 #include <stdint.h>
 #include <signal.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include "signal_handler.h"
 #include "../internal/vm.h"
 #include "../../clib/arraylist.h"
 
 using namespace std;
 using namespace scorpionvm;
 using namespace scorpionvm::vm;
 using namespace scorpionvm::io::signal;
 
  #define nullptr ((void *)0)

 namespace scorpionvm
 {
     namespace io
     {
         namespace runtime_exception
         {
             class Exception
             {
                 public:
                   void error(string __what)
                   {
                       what=(__what);
                   }
                   void error(const char* _what_arg, string _what)
                   {
                     what_arg=(_what_arg);
                     what=(_what);
                   }
                   void _throw_(ScorpionVM* vm)
                   {
                      if(vm != NULL)
                        printf("RuntimeException: Caused by %s: %s\n%s", what_arg, what.c_str(),
                            vm->func_tracker.get_func_trace().c_str());
                      sig_handler::raise_sig(SIGQUIT);
                   }
                   const char* what_arg;
                   std::string what;
             };
             
         }
     }
 }
 
 #endif // SCORPION_RUNTIME_EXCEPTION