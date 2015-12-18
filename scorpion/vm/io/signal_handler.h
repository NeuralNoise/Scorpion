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
 #ifndef SCORPION_SIG_HANDLER
 #define SCORPION_SIG_HANDLER
 
 #include <string>
 #include <stdint.h>
 #include <signal.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include "../../clib/arraylist.h"
 
 using namespace std;
 
  #define nullptr ((void *)0)
  
 namespace ScorpionVM
 {
     namespace io
     {
         namespace signal
         {
             enum
             {
                 SIG_REGRISTRATION_SUCCESS,
                 SIG_REGRISTRATION_FAILED
             };
             
             class sig_handler
             {
                 public:
                   int sig;
                   ListAdapter <sig_atomic_t> sig_values;
                   struct sigaction act;
	           
                   sig_handler()
                   :  sig(0)
                   {
                   }
                   
                   int register_handler()
                   {
                        memset(&act, 0, sizeof(struct sigaction));
                    	sigemptyset(&act.sa_mask);
                    
                    	act.sa_sigaction = sig_handler::_sig_handler;
                    
                    	act.sa_flags = SA_SIGINFO;
                    
                        sig_list();
                        
                        for(uint64_t i=0; i<sig_values.size(); i++)
                        {
                            if (-1 == sigaction(sig_values.valueAt(i), &act, NULL))
                        		return SIG_REGRISTRATION_FAILED;
                        }
                        return SIG_REGRISTRATION_SUCCESS;
                   }
                   
                   void raise_sig(sig_atomic_t sig)
                   {
                       if(sig_values.contains(sig))
                          raise(sig);
                   }
                   
                   private:
                     void sig_list()
                     {
                         /* --------- Arithmetic and execution signals --------- */

                         sig_values.add(SIGFPE);
                         sig_values.add(SIGKILL);
                         sig_values.add(SIGSEGV);
                         sig_values.add(SIGBUS);

                         /* --------- Termination signals --------- */

                         sig_values.add(SIGABRT);
                         sig_values.add(SIGINT);
                         sig_values.add(SIGTERM);
                         sig_values.add(SIGQUIT);

                         /* --------- Alarm signals --------- */

                         sig_values.add(SIGALRM);
                         sig_values.add(SIGVTALRM);
                         sig_values.add(SIGPROF);

                         /* --------- Operation error signals --------- */

                         sig_values.add(SIGPIPE);
                         sig_values.add(SIGXCPU);
                         sig_values.add(SIGXFSZ);

                         /* --------- Asynchronous I/O signals --------- */

                         sig_values.add(SIGIO);
                         sig_values.add(SIGURG);
                     }
                   
                     static void _sig_handler(int signo, siginfo_t *sinfo, void *context /* unused */)
                     {
                         cout << "sig raised " << signo << endl;
                     }
                   
                     void handle_signal_SIGINT(const std::string error){}
             };
         }
     }
 }
 
 #endif // SCORPION_SIG_HANDLER