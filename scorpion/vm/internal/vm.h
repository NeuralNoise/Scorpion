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
 #ifndef SCORPION_VIRTUAL_MACHINE
 #define SCORPION_VIRTUAL_MACHINE
 
 #include <string>
 #include <stdint.h>
 #include "../memory/block_allocator.h"
 #include "../log/log.h"
 #include "../log/log_service.h"
 #include "../../clib/arraylist.h"
 #include "../../libxso/xso_reader.h"
 #include "../memory/vm_env.h"
 #include "func_tracker.h"
 
 using namespace std;
 using namespace scorpionvm;
 using namespace scorpionvm::memory;
 using namespace scorpionvm::xsoreader;
 using namespace scorpionvm::memory::environment;
 using namespace scorpionvm::log::log_service::debug;
 #define nullptr ((void *)0)

  #define HEAP_STANDARD 1000000  // 1,000 kb
  #define prototype_limit (64*1024)
  #define SVM_OK 0
  
 extern Log ldebug;
 namespace scorpionvm // we do this to not confuse compiler
 {
     namespace vm
     {
         namespace console_helper
         {
             
             enum response_codes
             {
                 err_std=1,
                 err_invalid_command=2,
                 err_min_heap=3,
                 err_max_heap=4,
                 err_seg_fault=5
             } ;
             
             struct options
             {
                bool Assert;
                double  min_heap;
                double  max_heap;
                bool Debug;
                string xso_file;
                ListAdapter<std::string> sharedlibs;
                ListAdapter<std::string> pargs;
                bool out;
                int log_level;
             } ;
            
             void arg_list_init();
            
             bool hasdash(string arg);
            
             string trim(string str);
            
             void print_non_std_usage();

             int process_args(int argc, const char **argv);
         }
         
         enum states
         {
             vm_state_normal=0,
             vm_state_frozen=1,
             vm_state_passive_frozen=2
         } ;
         
         class ScorpionVM
         {
             
             public:
               ScorpionVM()
               :  pc(0),
                  size_t(0),
                  if_count(0),
                  if_depth(0),
                  func_depth(0),
                  state(0),
                  exc(true)
               {
                   image_stream._init_();
                   image_stream.enableprotected();
               }
               
               uint64_t pc, size_t, state;
               int64_t if_count, if_depth, func_depth, sp;
               bool exc;
               function_tracker func_tracker;
               ListAdapter<double> image_stream;
               ListAdapter<std::string> permission_list;
               
               int DestroyScorpionVM(ScorpionVM* vm, int N)
               {
                   if(vm == NULL || N == 0)
                     return 1;
                      
	                  int shutdown_response = SVM_OK;
	                  BlockAllocator<ScorpionVM> vm_allocator;
                            
                   if(N == 1)
                   {
                       if(vm->image_stream.size() != 0)
                         vm->image_stream.clear();
                         
                       if(vm->state != vm_state_normal)
                       {
                           stringstream ss;
                           ss << "The Scorpion virtual machine is attempting to shutdown with abnormal state (" << vm->state << ").";
                           ldebug.LOGV(ss.str(), "ScorpionVM");
                       }
                       
                       return vm_allocator.free( vm );
                   }
                   
                   return SVM_OK;
               }
               
               uint64_t sizeinfo()
               {
                   return size_t;
               }

         };
         
         int Init_CreateScorpionVM(ScorpionVM* vmstate, scorpion_env* p_env, xso_reader* reader, 
                    const char** argv, int argc);
         
         int Init_StartScorpionVM();
     } // end vm
 } // end scorpionvm
 
#endif // SCORPION_VIRTUAL_MACHINE