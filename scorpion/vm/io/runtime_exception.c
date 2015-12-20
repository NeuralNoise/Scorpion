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
 
 #include <string>
 #include <stdint.h>
 #include <signal.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include "../internal/globals.h"
 #include "../internal/vm.h"
 #include "../../clib/arraylist.h"
 #include "runtime_exception.h"
 
 using namespace std;
 using namespace scorpionvm;
 using namespace scorpionvm::vm;
 using namespace scorpionvm::io::runtime_exception;
 using namespace scorpionvm::io::runtime_exception;
 
  #define nullptr ((void *)0)
 
 void scorpionvm::io::runtime_exception::runtime_error(Exception e, ScorpionVM* vm)
 {
     if(vm != NULL)
       printf("RuntimeException: Caused by %s: %s\n%s", e.what_arg, e.what.c_str(),
           vm->func_tracker.get_func_trace().c_str());
     g_Svm._sig_handler.raise_sig(SIGQUIT);
 }
