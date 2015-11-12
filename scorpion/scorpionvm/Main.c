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
 /*
 * Command line invocation of the Scorpion VM
 */
 
 #include "../vm/scorpionvm.h" 
 #include "../vm/exception.h"
 #include "../vm/scorpion_env.h"
 #include "../logservice/alog.h"
 #include "../logservice/filter.h"
 #include "../vm/Globals.h"
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include "../libxso/xso.h"
 #include <signal.h>
 #include <assert.h>
 #include <iostream>
 #include <sstream>
 
 using namespace std;
 
void printusage()
{
  string product_website = "https://github.com/AndroDevcd/Scorpion-Dist/wiki";
  
  printf("Usage: scorpion [-options] xso [args...]\n");    
  printf("           (to execute scorpion object file)\n");
  printf("or    scorpion [-options] -sar sarfile [args...]\n");
  printf("           (to execute a sar file)\n\n");
  printf("[-options]\n\n");
  printf("    -version        print the current product version and exit.\n");
  printf("    -showversion    print the current product version and continue.\n");
  printf("    -help -?        display this help message.\n");
  printf("    -da -disableassertions\n");
  printf("                    disable assertions.\n");
  printf("    -Xms <size>     set the minimum heap size.\n");
  printf("    -Xmx <size>     set the max heap size.\n");
  printf("    -sdb            enable the scorpion debug bridge.\n");
  printf("    -?std           print non-standard options.\n");
  printf("See %s for more details.\n", product_website.c_str());
}

#define nullptr ((void *)0)

ALog alog;
extern int vmStatus;

SvmGlobals gSvm;

/*
* Parse arguments.  Most of it just gets passed through to the VM.
*
* Look at the log! The Scorpion Virtual Machine offers a 
* comprehensive logging system that will tell you everything 
* you need to know when debugging a Scorpion application.
*
* System log dest: /us/share/scorpion/vm/log/out.log
*/
int main(int argc, const char **argv){
    ScorpionVM vm;
    ScorpionEnv* p_env = NULL;
    XSO* x = NULL;
    int optionCount, curOpt, i, argIdx;
    int needExtra = 0;
    int result = 1;
    int status;
    std::string lastFlag;
    string problemFlag;

    if(argc == 1){
       printusage();
       goto bail;
    }

    /*
     * If we're adding any additional stuff, e.g. function hook specifiers,
     * add them to the count here.
     *
     * We're over-allocating, becforause this includes the options to the VM
     * plus the options to the program.
     */
    optionCount = argc - 1;
    Exception::trace.addproto("vm.internal.system.main", "[ScorpionVirtualMachine]", 1);

    /*
     * First level Arg check.  Check if argumets arent faulty
     * all arguments begin with a '-'
     *
     * VM Options:
     *
     * Usage scorpion [-options] xso [args...]
     *            (to execute scorpion object file)
     * or    scorpion [-options] -sar sarfile [args...]
     *            (to execute a sar file)
     *
     *    -version
     *    -showversion
     *    -help -? 
     *    -da(disable assertions)
     *    -Xms <size>
     *    -Xmx <size>
     *    -sdb
     * [Do we need to catch & handle "-sar" here?]
     */
    curOpt = 0;
    needExtra = 0;
    for (argIdx = 1; argIdx < argc; argIdx++) {
        lastFlag = argv[argIdx];

        /* some options require an additional arg */
        if ((lastFlag == "-Xms") ||
            (lastFlag == "-Xmx"))
            /* others? */
        {
            if(!((argIdx + 1) < argc)){
                problemFlag = lastFlag;
                needExtra = 1;
                break;
            }
            
            string s = argv[argIdx + 1];
            if(s.at(0) == '-'){
                problemFlag = lastFlag;
                needExtra = 1;
            }
        }
    }

    
    if (needExtra != 0) {
        printf("error:  Faulty Argument flag. Scorpion VM requires a value after option flag: %s\n", problemFlag.c_str());
        goto bail;
    }

    /*
     * Start VM.  The current thread becomes the main thread of the VM.
     */
     Init_CreateScorpionVM(vm, p_env, x, argv, argc);


    if(vmStatus != 0) { // have method return ScorpionEnv
        printf("error:  Failed to initalize the Scorpion VM.\nA fatal error has occured, shutting down.\n");
        goto bail;
    }

    gSvm.vm.status = 1; // Virtual machine has been created
    x = NULL;

    /* We Skip creating the Main Thread */

    /* Start Virtual Machine */
    status = Init_StartScorpionVM();

    if (status != 0)
        alog.ALOGW("Failed to start the Scorpion Virtual Machine.");



    /*
    * Shut down the Scorpion Virtual Machine due to
    * boot err.
    */
    bail:
        /*printf("Shutting down Scorpion VM\n");*/
     if (gSvm.vm.status != 0) 
        Init_ShutdownScorpionVM();
    
      alog.ALOGV("--- VM is down, process exiting.");
      return result;
}

