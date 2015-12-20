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
 #ifndef SCORPION_GLOBALS
 #define SCORPION_GLOBALS
 
 #include <string>
 #include <stdint.h>
 #include <signal.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include "../memory/allocation_scheme.h"
 #include "../memory/object_container.h"
 #include "../memory/block_allocator.h"
 #include "../io/signal_handler.h"
 #include "func_tracker.h"
 #include "vm.h"
 #include "../memory/vm_env.h"
 #include "../../clib/arraylist.h"
 
 using namespace std;
 using namespace scorpionvm;
 using namespace scorpionvm::vm;
 using namespace scorpionvm::memory;
 using namespace scorpionvm::io::signal;
 using namespace scorpionvm::memory::environment;
 
 namespace scorpionvm
 {
     struct Globals
     {
         sig_handler _sig_handler; /* We handle most OS signals */
         
         ScorpionVM* vmstates;
         scorpion_env* env;
         
         MethodContainer* native_methods;
         
         /* Global pointer to vm & environment */
         uint64_t p_ptr;
     };
 }
 
 extern Globals g_Svm;
 
 #endif // SCORPION_GLOBALS