
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
#include "scorpionvm.h"
#include "policy.h"
#include "exception.h"
#include "../clib/filestream.h"
#include "../libxso/xso.h"
#include "../logservice/Log.h"
#include "scorpion_env.h"
#include "Globals.h"
#include <stdlib.h>
#include "../logservice/alog.h"
#include "interp.h"
#include "security.h"
#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

extern void printusage();

string *ags;
bool TRUE = true, FALSE = false;


string build_version = "1.0_12";
int revision_num = 7;

string OPTION = "";
int arg_start = 0;
int XSO_PATH = 0;
int SAR_PATH = 0;

#define NUM_OPTIONS 19
#define SINGLE_ENVIRONMENT 1
string args[ NUM_OPTIONS ];

option options;
extern initdata idata;

void setuparguments()
{
   options.minHeap = HEAP_STANDARD;
   options.maxHeap = HEAP_STANDARD *2;
   options.llevel = VERBOSE;
   args[0] = "-version";
   args[1] = "-showversion";
   args[2] = "-help";
   args[3] = "-?";
   args[4] = "-da";
   args[5] = "-disableassertions";
   args[6] = "-Xms";
   args[7] = "-Xmx";
   args[8] = "-sdb";
   args[9] = "-sar";
   args[10] = "-?std";
   args[11] = "-out";
   args[12] = "-log=verbose";
   args[13] = "-log=debug";
   args[14] = "-log=info";
   args[15] = "-log=warning";
   args[16] = "-log=error";
   args[17] = "-log=assert";
   args[18] = "-log=off";
}

bool arg(string arg)
{
  for(int i = 0; i < NUM_OPTIONS; i++)
  {
     if(arg == args[i]){
        OPTION = arg;
        return true;
     }
  }

  return false;
}

bool hasdash(string arg)
{
   if(arg.at(0) == '-')
     return true;

  return false;
}

string trim(string str)
{
  return str.substr(0, str.size()-1);
}

void print_non_std_usage()
{
  string product_website = "https://github.com/AndroDevcd/Scorpion-Dist/wiki";
  
  printf("Usage scorpion [-options] xso [args...]\n");    
  printf("           (to execute scorpion object file)\n");
  printf("or    scorpion [-options] -sar sarfile [args...]\n"); 
  printf("           (to execute a sar file)\n\n");
  printf("[-options]\n\n");
  printf("    -out            output application info upon execution.\n");
  printf("    -log=<level>    set the system log level (standard level: verbose).\n");
  printf("                    \"-log=off\" to shut off system logging.\n");
  printf("See %s for more details.\n", product_website.c_str());
}


void process_ags(int argc, const char** args)
{
      Exception::trace.addproto("vm.internal.system.process_ags", "ScorpionVM", 1);
      string data = "";
      for(int i = 1; i < argc; i++){
         data = args[i];

         if(arg(data)) {}
         else if(hasdash(data))
         {
            cout << "Unrecognized command line option: " << data << endl;
            cout << "Try 'scorpion -help' or 'scorpion -?' for more information.\n";
            Init_ShutdownScorpionVM();
         }
         else{  // time for running application
            XSO_PATH=(1);
            options.XSOF = args[i];
            int program_args = (argc - i);
            options.ags_t = program_args;
            options.ags = new string[program_args];
            int curArg = 0;
            
            // add program arguments
            for(int x = i; x < argc; x++)
                options.ags[curArg++] = args[x];
                
            return;
         }

         if(OPTION == "-help" || OPTION == "-?"){
            printusage();
            alog.ALOGV("--- VM is down, process exiting.");
            exit(0);
         }
         else if(OPTION == "-version"){
            cout << "scorpion version \"" << build_version << "\"" << endl;
            cout << "Scorpion(TM) SDK (build " << build_version << "-b" << revision_num << ")"<< endl;
            cout << "Scorpion(TM) 64-Bit Server VM (v" << revision_num << ")" << endl;
            alog.ALOGV("--- VM is down, process exiting.");
            exit(0);
         }
         else if(OPTION == "-showversion"){
            cout << "scorpion version \"" << build_version << "\"" << endl;
            cout << "Scorpion(TM) SDK (build " << build_version << "-b" << revision_num << ")"<< endl;
            cout << "Scorpion(TM) 64-Bit Server VM (v" << revision_num << ")" << endl;
         }
         else if(OPTION == "-da" || OPTION == "-disableassertions")
            options.Assert = TRUE; 
         else if(OPTION == "-Xms"){
            i++;
            arg_start++;
            data = args[i];
            double segment;
            if(data.at(data.length() - 1) == 'b'){ // bytes
               segment = 1;
               data = trim(data);
            }
            else if(data.at(data.length() - 1) == 'k'){ // kb
               segment = 1000;
               data = trim(data);
            }
            else if(data.at(data.length() - 1) == 'm'){ // mb
               segment = 1000000;
               data = trim(data);
            }
            else if(data.at(data.length() - 1) == 'g'){ // gb
               segment = 1000000000;
               data = trim(data);
            }
            else
               segment = 1000;
            
            double mb = atoi(data.c_str());
            mb *= segment; // convert to actual mem addrs
            options.minHeap = mb;
            if(options.minHeap <= 0){
              options.minHeap = HEAP_STANDARD;
              alog.ALOGV("warning: requested heap may be too large or equal to 0.");
            }
            else if(options.minHeap > options.maxHeap)
               Exception("Minimum heap size cannot be greater than heap Max.", "RuntimeException");
         }
         else if(OPTION == "-Xmx"){
            i++;
            arg_start++;
            data = args[i];
            double segment;
            if(data.at(data.length() - 1) == 'b'){ // bytes
               segment = 1;
               data = trim(data);
            }
            else if(data.at(data.length() - 1) == 'k'){ // kb
               segment = 1000;
               data = trim(data);
            }
            else if(data.at(data.length() - 1) == 'm'){ // mb
               segment = 1000000;
               data = trim(data);
            }
            else if(data.at(data.length() - 1) == 'g'){ // gb
               segment = 1000000000;
               data = trim(data);
            }
            else
               segment = 1000;
    
            double mb = atoi(data.c_str());
            mb *= segment; // convert to actual mem addrs
            options.maxHeap = mb;
            if(options.maxHeap < options.minHeap)
                Exception("Maximum heap size cannot be lower than minmum heap requested.", "RuntimeException");

         }
         else if(OPTION == "-sdb")
            options.Debug = TRUE;
         else if(OPTION == "-?std"){
            print_non_std_usage();
            alog.ALOGV("--- VM is down, process exiting.");
            exit(0);
         }
         else if(OPTION == "-out")
            options.out = TRUE;
         else if(OPTION == "-sar"){
             if(!((i + 1) < argc))
                Exception("SAR file not provided.", "RuntimeException");
                
            i++;
            options.SARF = args[i];
            int program_args = (argc - i);
            options.ags_t = program_args;
            options.ags = new string[program_args];
            int curArg = 0;
            
            // add program arguments
            for(int x = i; x < argc; x++)
                options.ags[curArg++] = args[x];
          
            SAR_PATH=(1);
            return;
         }
         else if(OPTION == "-log=verbose")
            options.llevel = VERBOSE;
         else if(OPTION == "-log=debug")
            options.llevel = DEBUG;
         else if(OPTION == "-log=info")
            options.llevel = INFO;
         else if(OPTION == "-log=warning")
            options.llevel = WARN;
         else if(OPTION == "-log=error")
            options.llevel = ERROR;
         else if(OPTION == "-log=assert")
            options.llevel = ASSERT;
         else if(OPTION == "-log=off")
            options.llevel = ASSERT+1;  // weird trick to filter out logging
   }
   Exception("Failed to properly parse VM args.","RuntimeException");
}

bool iscompatible(XSO x)
{
    if(x.headerInf.minimum_dev_vers.byte1 <= revision_num)
       return true;
   return false;
}

void validateProtoSize(XSO f)
{
     Exception::trace.addproto("vm.internal.system.validateProtoSize", "ScorpionVM", 1); // add fake proto for debugging
     if(f.headerInf.method_size.byte1 > PROTOTYPE_LIMIT)
          Exception("method size > PROTOTYPE_LIMIT (65538) ", "PrototypeSizeOverloadException");
     else if(f.headerInf.method_size.byte1 < 1)
          Exception("The current application has no methods to run!", "MethodNotAvailableException");
}

void appIsCompatible(XSO f)
{
    Exception::trace.addproto("vm.internal.system.appIsCompatible", "ScorpionVM", 1);
    if(!iscompatible(f)){
         alog.ALOGV("Error, the current application is not compatible with the current VM. Have you checked \"minumumDevVersion\" in your .dev build file?");
         stringstream ss;
         ss << "Application " << f.headerInf.application_id << " is not compatible. Minimum Dev version: " 
            << f.headerInf.minimum_dev_vers.byte1 << " required: " << revision_num << ".";
         Exception(ss.str(), "ApplicationNotCompatibleException");
      }
}

int performSecuritySetup(){
    Exception::trace.addproto("vm.internal.system.setup", "SecurityManager", 1);
       
    gSvm.appmanager.controller.setupAccesPermissions();
    gSvm.appmanager.setup();
    
    for(int i = 0; i < gSvm.psize_t; i++){
       if(gSvm.appmanager.checkPermission(gSvm.permissions[i])){
           if(!gSvm.appmanager.hasPermission(gSvm.permissions[i]))
               gSvm.appmanager.add(gSvm.permissions[i]);
           
       }    
       else {
           stringstream ss;
           ss << "warning:  request for permission " << gSvm.permissions[i].getPermission() << " failed.";
           alog.setClass("SecurityManager");
           alog.ALOGD(ss.str());
       }
    }
    
    delete[] gSvm.permissions;
    return 0;
}

int vmStatus = 0;

void Init_CreateScorpionVM(ScorpionVmState *vm, ScorpionEnv* env, XSO* f, const char** args, int ags_t)
{
    /*
    * Upon Creation of a Scorpion Virtual Machine 
    * we want to force shutdown the VM to prevent floating or 
    * not destroyed memory upon program close
    *
    * This is the center of what the Scorpion Virtual Machine
    * is all about, destroying unused and un-nessicary memory that
    * is no longer needed.
    */
    gSvm.ForceShutdown = true;
        
    Exception::trace.addproto("vm.internal.system.Init_CreateScorpionVM", "ScorpionVM", 1);
    env = new ScorpionEnv[SINGLE_ENVIRONMENT];
    gSvm.vmCount++;
    gSvm.envCount++;
    
    // process arguments
    setuparguments();
    process_ags(ags_t,args);
    
    
    /* Setup Logging Service */
    alog.setup(options.llevel);
    alog.setClass("ALog");
    alog.ALOGI("Log setup: using /usr/scorpion/vm/log");
    
    gSvm.Debug =  options.Debug;  
    if(XSO_PATH){
       
       alog.ALOGV("Path found: XSO_PATH.");
       alog.ALOGI("Running path.");
       f = new XSO[1]; // we only need 1 XSO processor

       // add false method calls here (for application debugging purposes)
       Exception::trace.addproto("vm.internal.system.isXSO", "ScorpionVM", 1);
       if(!FileStream::endswith(".xso", options.XSOF))
           Exception("Executable File must be a .xso file.", "InvalidExtensionException");
       else {
          Exception::trace.addproto("vm.internal.system.xsoExists", "ScorpionVM", 1);
          if(!FileStream::exists(options.XSOF.c_str())){
              stringstream ss;
              ss << "Executable file: " << options.XSOF << " does not exist!";
              Exception(ss.str(), "FileNotFoundException");
          }    
       }
       
      alog.setClass("XSO");
      alog.ALOGV("Processing .xso file.");
       // use libxso/ to process xso file
      string xso = FileStream::getfile(options.XSOF.c_str());
      f->process(xso);
      xso = "";


      if(options.out){
          printf("XSO Header content:\n--------------------\n\n");
          f->printfullheaderinfo();
          printf("--------------------\n\n");
      }

      validateProtoSize(f[0]);
      gSvm.appheader = f->headerInf;

      alog.ALOGD("Scorpion Virtual Machine compatibility check.");
      appIsCompatible(f[0]);

      stringstream ss;
      ss << "Application " << f->headerInf.application_id << " is compatible.\n\tMinimum Dev version: " 
        << f->headerInf.minimum_dev_vers.byte1 << "\n\tRequired: " << revision_num;
      alog.ALOGV(ss.str());
      
      alog.setClass("XSOImg");
      alog.ALOGD("Saving application permissions.");
      
      Exception::trace.addproto("vm.internal.system.setPermissions", "XSOImg", 1);
      setPermissions(f[0]);
      
      // setup application Logging service
      alog.ALOGD("Usr log service setup running.");
      gSvm.dlog.setup(f[0], f->headerInf.log_precedence.byte1);
      
      // setup environment structures
      if (gSvm.vmCount > 1)
        Exception("ERROR: Scorpion only supports one VM per process.", "ScorpionVmStartupErr");
    
     //setup virtual machine
     alog.setClass("ScorpionEnv");
     alog.ALOGI("Setting up environment structures and memory.");
     
     int status;
     gSvm.mtds = new (nothrow) Method[f->headerInf.method_size.byte1];
     vm = new (nothrow) ScorpionVmState[1];
     
     if(gSvm.mtds == nullptr || vm == nullptr)
     {
         status = -1;
         goto err;
     }
     
     status = env->InitEnvironmentSetup("ScorpionVirtualMachine", options.minHeap, options.maxHeap, options.minHeap, stack_limit); 
     gSvm.methodc = f->headerInf.method_size.byte1;
    
    err:
      //vm = p_vm;
      if (status != 0) {
           alog.setClass("ScorpionVM");
           alog.ALOGI("The Scorpion Virtual Machine was not created successfully.");
          vmStatus = -1;
          return;
      }
    
     gSvm.env = env;
    
    // we temporarilly do this to prevent a crash right before close
    gSvm.vmstate = vm;
    vm = NULL;
    
    gSvm.vmstate->sp = -1; // we start the stack ptr below the stack
    gSvm.vmstate->methodcount = 0;
    gSvm.vmstate->ifcount = 0;
    gSvm.vmstate->status = vm_status_normal; // normal status
    
    performSecuritySetup();
    gSvm.appolicy.preparePolicy();
    f->preprocess(gSvm.image);

  //  gSvm.appolicy.usingPolicy("scorpion.permission.GPIO"); 

      // set application info
     alog.setClass("ScorpionVM");
     alog.ALOGI("The Scorpion Virtual Machine was created successfully.");
       gSvm.ForceShutdown = false;
       return;
    } // XSO
    
    if(SAR_PATH){
       alog.ALOGV("Path found: SAR_PATH.");
       alog.ALOGI("Running path.");
       
       Exception("The Scorpion Virtual Machine currently does not support running a .sar file.","PathNotSupportedException");
       gSvm.ForceShutdown = false;
       return;
    } // SAR
    
    Exception("Could not find path to start the Scorpion VM.", "InitPathNotFoundException");
}

extern bool includep;
int Init_StartScorpionVM()
{
   Exception::trace.addproto("vm.internal.system.Init_StartScorpionVM", "ScorpionVM", 1);
   Exception::trace.enablenative();
   
   int status;
    /*
    * Program termination code. This is how we know how our program terminated.
    *
    * Initally this is set to be printed (this can be shut off)
    */
    gSvm.vmstate->exc = 1;   // TODO: Update Dev script to 'showExitVal(false)' option for disabling exit value
   
    // TODO: push arguments on the stack to be processed by main method
      if(0 == gSvm.methodc)
           Exception("coold not locate main method.", "MethodNotFoundException");
      
      stringstream main;
      main << gSvm.mtds[0].module << "." << gSvm.mtds[0].name << "<init>";
      Exception::trace.addproto(main.str(), gSvm.mtds[0].clazz, false);
      
      includep=false;
      status = Scorpion_InvokeMain(gSvm.vmstate);
    
    if(status != 0){
        alog.ALOGD("failed to invoke main() method.");
        return -1;
    }

    // TODO: Set the program args in ArrayObject   
    Scorpion_VMExecute(gSvm.vmstate);
   return 0;
}

void Init_ShutdownScorpionVM()
{
    Exception::trace.disablenative();
    bool shutdown = false, exit_proc = false;  
    /*
    * Upon startup, there are so many possible errors that could happen 
    * therfore we must allow the virtual machine to be forceably be shut down
    * 
    */
     if (gSvm.vmstate->status == vm_status_normal || gSvm.ForceShutdown) {
		 shutdown = true;
		 exit_proc = (bool) gSvm.vmstate->exc;
		 
        /*
         * This allows join() and isAlive() on the main thread to work
         * correctly, and also provides uncaught exception handling.
         */
     //   if ((*vm)->DetachCurrentThread(vm) != JNI_OK) {
    //        fprintf(stderr, "Warning: unable to detach main thread\n");
    //        result = 1;
     //   }
         
        if (gSvm.vmstate->DestroyScorpionVM(gSvm.vmstate, 1) != 0)
            fprintf(stderr, "Warning: Scorpion VM did not shut down cleanly\n");
    
      if(gSvm.env != nullptr){ // shut down all block table structures 
         if(gSvm.vmstate != NULL && (gSvm.vmstate->sp >= 0)){
             unsigned long address = gSvm.env->getBitmap().stack->plong[gSvm.vmstate->sp--]; // simple stack pop
             if(isgeneric(__typedef(gSvm.env->getBitmap().objs[address])))
                gSvm.exitval = (slong) svmGetGenericValue(gSvm.env->getBitmap().objs[address]);
         }
           
        alog.ALOGD("Shutting down environments.");     
        svmBitmapMemoryShutdown(gSvm.env->bitmap); 
      }
      
    }
    
    if( !shutdown && gSvm.vmstate != nullptr && !gSvm.ForceShutdown )
    {
        stringstream ss;
        ss << "The Scorpion virtual machine is attempting to shutdown with abnormal status code (" << gSvm.vmstate->status << ").";
        alog.ALOGV(ss.str());
        gSvm.vmstate->status = 0;
        segfault();
    }
    
    if(!gSvm.ForceShutdown && !gSvm.ethrow && exit_proc){
             
      if(gSvm.exitval == 0)
        cout << "program exited with exit code: 0 (normal program termination)" << endl;
      else
        cout << "program exited with exit code: " << gSvm.exitval << endl;
    }
    else gSvm.exitval=0;
    
    alog.ALOGV("--- VM is down, process exiting\n");
    exit(gSvm.exitval);
}


