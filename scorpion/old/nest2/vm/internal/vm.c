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
 #include "../memory/block_allocator.h"
 #include "../log/log.h"
 #include "../log/log_service.h"
 #include "../../libxso/xso_reader.h"
 #include "../memory/object_container.h"
 #include "../memory/vm_env.h"
 #include "func_tracker.h"
 #include "globals.h"
 #include "vm.h"
 #include "interp.h"
 #include "../../clib/arraylist.h"
 
 using namespace std;
 
  #define nullptr ((void *)0)

  #define HEAP_STANDARD 1000000  // 1,000 kb
  #define SVM_OK 0
  
  extern Log ldebug;
  extern Log usr_log;
  extern void printusage();
  extern Globals g_Svm;
 
     options console_options;

     bool TRUE = true, FALSE = false;
     string build_version = "1.0_36";
     int revision_num = 8;
     ListAdapter<std::string> vm_args;
    
     void arg_list_init()
     {
       vm_args.add("-version");
       vm_args.add("-showversion");
       vm_args.add("-help");
       vm_args.add("-?");
       vm_args.add("-da");
       vm_args.add("-disableassertions");
       vm_args.add("-sdb");
       vm_args.add("-sar");
       vm_args.add("-?std");
       vm_args.add("-out");
       vm_args.add("-log=verbose");
       vm_args.add("-log=debug");
       vm_args.add("-log=info");
       vm_args.add("-log=warning");
       vm_args.add("-log=error");
       vm_args.add("-log=assert");
       vm_args.add("-log=off");
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

     void exceptionStartup()
     {
         g_Svm.NullPointerException.error("NullPointerException", "Cannot operate on null object.");
         g_Svm.HeapIndexOutOfBoundsException.error("HeapIndexOutOfBoundsException", "Scorpion heap space.");
         g_Svm.StackOverflowException.error("StackOverflowException", "Scorpion statck space.");
         g_Svm.StackUnderflowException.error("StackUnderflowException", "Scorpion statck space.");
         g_Svm.ArrayIndexOutOfBoundsException.error("ArrayIndexOutOfBoundsException", "");
         g_Svm.IllegalTypeException.error("IllegalTypeException", 
               "Cannot operate on objects with different types");
         g_Svm.OutOfMemoryErr.error("OutOfMemoryErr", "");
         g_Svm.RuntimeException.error("RuntimeException", "");
         g_Svm.IOFailure.error("IOFailure", "");
         g_Svm.UnspecifiedException.error("UnspecifiedException", "");
         g_Svm.MethodIndexOutOfBoundsException.error("MethodIndexOutOfBoundsException", "");
         
        //  /for(unsigned long i = 0; i < 500000000; i++){}
     }

     int process_args(int argc, const char **argv)
     {
         /* Pre assign all values*/
         console_options.Assert = true;
         console_options.min_heap = HEAP_STANDARD;
         console_options.max_heap = HEAP_STANDARD*2;
         console_options.Debug = false;
         console_options.xso_file = "";
         console_options.sharedlibs._init_();
         console_options.pargs._init_();
         console_options.out = false;
         console_options.log_level = VERBOSE;
         arg_list_init();
         
         string data;
         for(int i = 1; i < argc; i++){
             data = argv[i];
    
             if(vm_args.contains(data)) {}
             else if(hasdash(data))
             {
                cout << "Unrecognized command line option: " << data << endl;
                cout << "Try 'scorpion -help' or 'scorpion -?' for more information.\n";
                return err_invalid_command;
             }
             else{  // time for running application
                
                console_options.xso_file += argv[i];
                console_options.xso_file += ".xso";
                
                // add program arguments
                for(int x = i+1; x < argc; x++)
                    console_options.pargs.add(argv[x]);
                    
                return 0;
             }
    
             if(data == "-help" || data == "-?"){
                printusage();
                ldebug.LOGV("--- VM is down, process exiting.");
                sig_handler::raise_sig(SIGQUIT);
             }
             else if(data == "-version"){
                cout << "scorpion version \"" << build_version << "\"" << endl;
                cout << "Scorpion(TM) SDK (build " << build_version << "-b" << revision_num << ")"<< endl;
                cout << "Scorpion(TM) 64-Bit Server VM (v" << revision_num << ")" << endl;
                ldebug.LOGV("--- VM is down, process exiting.");
                sig_handler::raise_sig(SIGQUIT);
             }
             else if(data == "-showversion"){
                cout << "scorpion version \"" << build_version << "\"" << endl;
                cout << "Scorpion(TM) SDK (build " << build_version << "-b" << revision_num << ")"<< endl;
                cout << "Scorpion(TM) 64-Bit Server VM (v" << revision_num << ")" << endl;
             }
             else if(data == "-da" || data == "-disableassertions")
                console_options.Assert = TRUE; 
             else if(data == "-sdb")
                console_options.Debug = TRUE;
             else if(data == "-?std"){
                print_non_std_usage();
                ldebug.LOGV("--- VM is down, process exiting.");
                sig_handler::raise_sig(SIGQUIT);
             }
             else if(data == "-out")
                console_options.out = TRUE;
             else if(data == "-log=verbose")
                console_options.log_level = VERBOSE;
             else if(data == "-log=debug")
                console_options.log_level = DEBUG;
             else if(data == "-log=info")
                console_options.log_level = INFO;
             else if(data == "-log=warning")
                console_options.log_level = WARN;
             else if(data == "-log=error")
                console_options.log_level = ERROR;
             else if(data == "-log=assert")
                console_options.log_level = ASSERT;
             else if(data == "-log=off")
                console_options.log_level = ASSERT+1;  // weird trick to filter out logging
         }
         
         /* It should never reach this point */
         return err_std;
     }
     
     int Init_CreateScorpionVM(ScorpionVM* vmstate, scorpion_env* p_env, xso_reader* reader, 
                const char** argv, int argc)
     {
         /**
          * We do this to keep memory allocation
          * consistent accross the virtual machine
          * 
         */
         BlockAllocator<ScorpionVM> vm_allocator;
         BlockAllocator<scorpion_env> env_allocator;
         BlockAllocator<xso_reader> xso_allocator;
         
         int status;
         /* We setup the service anyway to log errors*/
         ldebug.service_setup(VERBOSE, false, "");
         
         int console_response = process_args(argc, argv);
         
         /* reset log service */
         ldebug.service_setup(console_options.log_level, false, "", true);
         if(console_response != 0)
         {
             ldebug.LOGV("error processing console args.", "ScorpionVM");
             return 1;
         }
         
         vmstate = vm_allocator.malloc(1, 0);
         p_env = env_allocator.malloc(1, 0); // we dont need calloc() here
         
         if(vmstate == NULL || p_env == NULL)
         {
             return 1;
         }
         
         exceptionStartup();
         if(!FileStream::endswith(".xso", console_options.xso_file))
         {
             ldebug.LOGV("Executable File must be a .xso file.", "ScorpionVM");
             return 1;
         }
         else 
         {
            if(!FileStream::exists(console_options.xso_file.c_str())){
                stringstream ss;
                ss << "Executable file: " << console_options.xso_file << " does not exist!";
                ldebug.LOGV(ss.str(), "ScorpionVM");
                return 1;
            }    
          }
         
          status = reader->read(console_options.xso_file.c_str());
          if(status != 0)
          {
              ldebug.LOGV("Failed to read xso image, have you compiler your application correctly?", 
                  "xso_reader");
              return status;
          }
          
          if(console_options.out)
          {
              printf("Xso Header content:\n--------------------\n\n");
              printf("File: %s\n", console_options.xso_file.c_str());
              reader->xso_file.print_header_info();
              printf("--------------------\n\n");
          }
          
           if(reader->xso_file.xso_header.method_size.byte1 > prototype_limit)
           {
               stringstream ss;
               ss << "method size > PROTOTYPE_LIMIT (" << prototype_limit << ")";
               ldebug.LOGV(ss.str(), "ScorpionVM");
           }
      
           if(reader->xso_file.xso_header.minimum_dev_vers.byte1 > revision_num)
           {
               stringstream ss;
               ss << "The application you are trying to run is not compatible with the current "
                    << "build of Scorpion. Note: current build " << revision_num
                      << ", supported minmum build " << reader->xso_file.xso_header.minimum_dev_vers.byte1;
               ldebug.LOGV(ss.str(), "ScorpionVM");
           }
           
           ldebug.LOGV("Application is compatible.", "ScorpionVM");
           ldebug.LOGV("Setting up application permissions.", "ScorpionVM");
           for(int i = 0; i < reader->xso_file.xso_header.permissions.size(); i++){
               stringstream ss;
               for(int xx = i; xx < reader->xso_file.xso_header.permissions.size(); xx++){
                   if(reader->xso_file.xso_header.permissions.at(xx) != ';')
                     ss << reader->xso_file.xso_header.permissions.at(xx) << "";
                   else
                     break;
                   i++;
               }
               vmstate->permission_list.add(ss.str()); // The Scorpion API will handle host machine's security
           }
           
           ldebug.LOGD("Usr log service setup running.", "ScorpionVM");
           usr_log.service_setup(reader->xso_file.xso_header.log_precedence.byte1, 
             true, reader->xso_file.xso_header.log_file);
           
           stringstream ss;
           ss << "ScorpionEnv_0"; // assign unique environment name
           p_env->setname(ss.str().c_str());
           
           ss.str("");
           ss << "New Scorpion environment: " << "ScorpionEnv_0";
           ldebug.LOGD(ss.str());
           
           if(p_env->alloc(reader->xso_file.xso_header.address_cap.byte1+1, reader->xso_file.xso_header.method_size.byte1, 
                0x0000, 0xfff) != 0)
           {
               ldebug.LOGV("Environment creation failed. Have you requested too much memory?", "ScorpionVM");
               return 1;
           }
           
           ldebug.LOGD("Environment was created successfully.", "ScorpionVM");   
           
           /**
            * We add both pointers to the global list for running multiple vm's 
            * on different threads
            * 
            */
           g_Svm.env = p_env;
           g_Svm.vmstates = vmstate;
           
           ldebug.LOGD("Preprocessing image file.", "ScorpionVM"); 
           if(reader->processor.preprocess() != 0)
           {
               cout << "Segmentation fault\n";
               ldebug.LOGD("Failed to properly preprocess the image file.", "ScorpionVM"); 
               return err_seg_fault;
           }
           
           ldebug.LOGD("The image file was succefully preprocessed.", "ScorpionVM"); 
           ldebug.LOGD("The Scorpion Virtual Machine was created successfully.", "ScorpionVM"); 
           
           g_Svm.vmstates->func_tracker.init();
           return 0;
     }
     
     /**
      * Its very simple to start the Scorpion VM, generally 
      * there will be no errors thrown upon startup
      */
     int Init_StartScorpionVM()
     {
        /**
        * Program termination code. This is how we know how our program terminated.
        *
        * Initally this is set to be printed (this can be shut off)
        */
         g_Svm.vmstates->exc = 1;
         
         g_Svm.vmstates->func_tracker.add_func(g_Svm.env->method_stack.valueAt(0), 0, true);
         
         g_Svm.env->method_stack.valueAt(0).ret = g_Svm.vmstates->pc;
         g_Svm.vmstates->pc = g_Svm.env->method_stack.valueAt(0).jmp;
     cout << "entry.\n";
         xso_exec(g_Svm.env, g_Svm.vmstates);
         return 1;
     }
     
