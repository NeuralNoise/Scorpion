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
 #include <stdlib.h>
 #include <string.h>
 #include <sys/wait.h>
 #include "../../clib/arraylist.h"
 #include "../log/log.h"
 
 using namespace std;
 
  #define nullptr ((void *)0)
 
 extern Log ldebug; 
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
       struct sigaction act, old;
   
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
                if (-1 == sigaction(sig_values.valueAt(i), &act, &old))
            		return SIG_REGRISTRATION_FAILED;
            }
            return SIG_REGRISTRATION_SUCCESS;
       }
       
       static void raise_sig(sig_atomic_t sig)
       {
              raise(sig);
       }
       
       private:
         void sig_list()
         {
             /* --------- Arithmetic and execution signals --------- */

             sig_values.add(SIGFPE);
            //  sig_values.add(SIGKILL);
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
         
         // TODO: process all IO signals
         static void _sig_handler(int signo, siginfo_t *sinfo, void *context /* unused */)
         {
             stringstream sig_info;
             sig_info << "signal caught:" << "\n\tsignal = " << signo << " (" << sig_str(signo) << ")" 
               << "\n\tproc = " << (long)getpid() << "\n\texit code (" << sinfo->si_status
                 << ")" << "\n\texit reason " << sinfo->si_code << endl;
             ldebug.LOGA(sig_info.str(), "sig_handler");
             
             switch( signo )
             {
                case SIGINT:
                    handle_signal_SIGINT("");
                case SIGSEGV:
                    handle_signal_SIGSEGV();
                case SIGQUIT:
                    handle_signal_SIGQUIT();
             }
             
             ldebug.LOGV("--- VM is down, process exiting.");
            
             exit(1);
         }
       
         private:
           static string sig_str(int signo)
           {
             switch( signo )
             {
                case SIGINT:
                    return "SIGINT";
                case SIGSEGV:
                    return "SIGSEGV";
                case SIGQUIT:
                    return "SIGQUIT";
                case SIGABRT:
                    return "SIGABRT";
                default:
                    return "SIGUNO";
             }
           }
           static void handle_signal_SIGINT(const std::string error){}
           static void handle_signal_SIGSEGV()
           {
           }
           static void handle_signal_SIGQUIT()
           {
           }
 };
 
 #endif // SCORPION_SIG_HANDLER