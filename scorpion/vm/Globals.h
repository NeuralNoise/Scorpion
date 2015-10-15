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
#ifndef SCORPION_VM_GLOBALS
#define SCORPION_VM_GLOBALS

 

 #include "../clib/u4.h"
 #include "../clib/u2.h"
 #include "../clib/u1.h"
 #include "exception.h"
 #include "variable.h"
 #include "scorpionvm.h"
 #include "scorpion_env.h"
 #include "../libxso/xso.h"
 #include "policy.h"
 #include "oo/Object.h"
 #include "imgholder.h"
 #include "permission.h"
 #include <sstream>
 #include <iostream>
 #include <string>
 using namespace std;

class SecurityManager;

/*
 * All fields are initialized to zero.
 *
 * Storage allocated here must be freed by a scheduled shutdown function.
 */
 struct SvmGlobals {
  
   int envptr;    // the pointer to which environment we are on
   ScorpionEnv* env;
   ScorpionVM vm;  // Our dear virtual machine
  
  
   string image; // our full executable image
   double* bytestream; // holds the image bytes to be processed
   
   Method* mtds;  // our shared method array
   long methodc;

   Permission *permissions;
   int psize_t;
 
   SecurityManager appmanager;
   Policy appolicy;
   ImageHolder appholder;
 
   header appheader; // application header
   Variable* vars; // tempory created variables popped from the stack
   int var_t; // the size of our temp vars

   int vmCount;
   int envCount;
   long exitval;  // the value our program closes with

   bool ForceShutdown; // force a VM shutdown
   bool ethrow;  // true if an exception is being thrown
   bool Debug;  // are we debugging this app
 };

extern struct SvmGlobals gSvm;
extern string mod;

#endif // SCORPION_VM_GLOBALS
