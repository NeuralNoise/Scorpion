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
#ifndef SCORPION_INTERPRETER
#define SCORPION_INTERPRETER

#include "../clib/u1.h"
#include "scorpionvm.h"
#include <string>
using namespace std;

/*
* Invokes the main method; should be called before any other 
* Scorpion_...() functions
*
* This function will simply just set up the main metho to be executed
*/
int Scorpion_InvokeMain(ScorpionVmState* vm);

/*
* Invoke a fuction based on its address
* 
* This fucction uses Bock address failure protection to give 
* the user a better understanding of what went wrong.
*
* If user attempts to call a fuction that dosent exist,
* throw a MethodNotFoundException 
*/
int Scorpion_InvokeMethod(long ptrValue, ScorpionVmState* vm);

/*
* This method speaks for itsself, this method represents 
* the entimre runtime system for the Scorpion Virtual Machine.
*/
 void Scorpion_VMExecute(ScorpionVmState* vmstate);

#endif // SCORPION_INTERPRETER