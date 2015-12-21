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
 #include "../../clib/arraylist.h"
 #include "../../libxso/xso_reader.h"
 #include "../memory/vm_env.h"
 #include "globals.h"
 #include "Opcodes.h"
 #include "vm.h"
 #include "interp.h"
 #include "func_tracker.h"
 
 using namespace std;
 using namespace scorpionvm;
 using namespace scorpionvm::vm;
 using namespace scorpionvm::memory;
 using namespace scorpionvm::xsoreader;
 using namespace scorpionvm::io::signal;
 using namespace scorpionvm::memory::environment;
 using namespace scorpionvm::log::log_service::debug;
 #define nullptr ((void *)0)
 
 typedef long long instruction;

 extern Log ldebug;
 
 void segfault()
 {
    cout << "Segmentation fault\n"; // TODO: Throw exception instead
    g_Svm._sig_handler.raise_sig(SIGQUIT);
 }
 
 void scorpionvm::vm::interpreter::Invoke_Method
        (scorpion_env* env, ScorpionVM* vm_state, uint64_t ptr)
 {
     if(ptr > env->sizeinfo(3,0) || ptr < 0)
     {
         // error
         stringstream ss;
         ss << "Pointer to method at address #" << ptr << " is outside of allocated method space."
             << " Note: method size: " << env->sizeinfo(3,0);
         ldebug.LOGV(ss.str(), "interpreter");
         segfault();
     }
     
     env->method_stack[ptr].ret = vm_state->pc;
     vm_state->pc = env->method_stack[ptr].jmp;
 }

 long long arg_t(ScorpionVM* scorpion_Vstate, instruction i)
 {
    if(i == OP_NOP || i == OP_END || i == OP_HLT || i == OP_NO || i == OP_ENDNO)
         return 0;
     else if(i == OP_RETURN || i == OP_PUSH || i == OP_POP || i == OP_JMP || i == OP_CALL
       || i == OP_MTHD || i == OP_LBL || i == OP_IF || i == OP_INC || i == OP_DEC
       || i == OP_KILL || i == OP_DELETE || i == OP_NEG || i == OP_CIN) // push 7 or push *x
         return 1;
     else if(i == OP_ICONST || i == OP_DCONST || i == OP_FCONST || 
        i == OP_SCONST || i == OP_BCONST || i == OP_CCONST || i == OP_RSHFT 
        || i == OP_LSHFT || i == OP_STR_APND || i == OP_ASSN 
        || i == OP_JIF || i == OP_JIT || i == OP_THROW
        || i == OP_STR_ACONST || i == OP_CAST || i == OP_BYTE_CONST || i == OP_LCONST
        || i == OP_DACONST || i == OP_IACONST || i == OP_FACONST || i == OP_CACONST 
        || i == OP_BACONST || i == OP_BYTE_ACONST || i == OP_SACONST || i == OP_LACONST) // mthd @9
         return 2;
     else if(i == OP_ISEQ || i == OP_ISLT || i == OP_ISLE || i == OP_ISGT 
       || i == OP_ISGE || i == OP_ADD || i == OP_SUB || i == OP_MULT || i == OP_DIV || i == OP_MOD
       || i == OP_OR || i == OP_AND || i == OP_AT || i == OP_ALOAD || i == OP_ASTORE) //
         return 3;
     else if(i == OP_COUT)
       return scorpion_Vstate->image_stream.valueAt(scorpion_Vstate->pc++)+1;
     else if(i == OP_STRCONST)
     {
       scorpion_Vstate->pc++;
       return scorpion_Vstate->image_stream.valueAt(scorpion_Vstate->pc++)+1;
     }
 }

 void return_method(long ptr, ScorpionVM* vm, scorpion_env* env){
     if(ptr >= env->sizeinfo(3,0)){
         // error
         stringstream ss;
         ss << "Pointer to method at address #" << ptr << " is outside of allocated method space."
             << " Note: method size: " << env->sizeinfo(3,0);
         ldebug.LOGV(ss.str(), "interpreter");
         segfault();
     }
     
     
     vm->pc = env->method_stack[ptr].ret;
 }

 string data(long size_t, ScorpionVM* vm)
 {
     stringstream ss;
     for(long i = 0; i < size_t; i++)
         ss << (char)vm->image_stream.valueAt(vm->pc++);
     
     return ss.str();
 }

 void _stdout(string out, scorpion_env* env)
 {
    // cout << "outting " << out << endl;
   char bad_char=(131);  
   for(long i = 0; i < out.size(); i++){
     if(out.at(i) != '\\'){
       cout << out.at(i);
       continue;
     }
     else {
        if(!((i + 1) < out.size()))
           return;
         
        i++;
        switch ( out.at(i) ){
            case '\\':
               cout << '\\'; continue;
            case 'n':
               cout << endl; continue;
            case 'b':
               cout << '\b'; continue;
            case 't':
               cout << '\t'; continue;
            case 'f':
               cout << '\f'; continue;
            case 'r':
               cout << '\r'; continue;
            case 'v':
               cout << '\v'; continue;
            case '"':
               cout << "\""; continue;
            case '\'':
               cout << "'"; continue;
            case '#':
               cout << std::flush; continue;
            case '[': // print variable data example:  $[v483|
               {
                 i++;
                 if(!((i) < out.size()))
                   return;
                   
                 int form = out.at(i);
                 stringstream  ss;
                 ss << "";
                 i++;
                 
                 for(long i2 = i; i < out.size(); i++){
                     if(isdigit(out.at(i)))
                       ss << out.at(i);
                     else{
                       if(ss.str() == "")
                          continue;
                       
                       long addr = atoi(ss.str().c_str());
                      // printf_obj_content(addr, form);
                       break;
                     }
                 }
                 continue;
               }
            case '{':
               {
 				            i++;
            					stringstream  ss, colorstream;
            					long color=0;
 					           for(long i2 = i; i < out.size(); i++){
 						            if(isdigit(out.at(i)))
 						               ss << out.at(i);
             						else{
             						  if(ss.str() == "")
             							   continue;
             						  
             						  color = atoi(ss.str().c_str());
             						  break;
             						}
 					           }
 					           
            					if(color == 0)
            					  cout << "\033[0m";
            					else
            					{
            					   if(color >=30 && color <=37)  {
            					     colorstream << "\033[1;" << color << "m";
            					     cout << colorstream.str();	
            					   }
            					   else if(color >=40 && color <=47)
            					   {
            						     color -=10;
            					      colorstream << "\033[0;" << color << "m";
            					      cout << colorstream.str();	
            					   }
            					}
 					           continue;
               }
        }
     }
   }
 }
 
 #define func_return(i, scorpion_Vstate, env) { \
            long long adr = i; \
            --scorpion_Vstate->func_depth; \
            if((adr == 0) && (scorpion_Vstate->func_depth <= 0)) \
              g_Svm._sig_handler.raise_sig(SIGQUIT); \
            else return_method(adr, scorpion_Vstate, env); \
        }
        
 #define ARG_t(scorpion_Vstate, i) \
           arg_t(scorpion_Vstate, i);

 void Protect(ScorpionVM* scorpion_Vstate, instruction i)
 {
    if(scorpion_Vstate->pc>=scorpion_Vstate->size_t)
       ldebug.LOGV("Attempting to close program unexpectingly.");
    else
    {
       stringstream ss;
       ss << "Attempting to execute invalid instruction(" << i << ").";
       ldebug.LOGV(ss.str());
    }
    
    segfault();
 }

 void scorpionvm::vm::interpreter::xso_exec(scorpion_env* env, ScorpionVM* vm_state)
 {
    ldebug.LOGD("Running application.");
    
    vm_state->size_t = vm_state->image_stream.size();
    instruction i;
    
    /* Main loop for interpreter */
    for( ;; )
    {
      newframe:
       if(vm_state->pc>=vm_state->size_t)
          Protect(vm_state, i);
       
       i = vm_state->image_stream.valueAt(vm_state->pc++);
       if(i>sMaxOpcodeLimit||i<0) Protect(vm_state, i);
       
       if((vm_state->state == vm_state_frozen && i != OP_ENDNO) ||
          (vm_state->state == vm_state_passive_frozen && (i == OP_END || i == OP_IF)))
       {
          vm_state->pc+=ARG_t(vm_state,i);
          goto newframe;
       }
       
       //printf("Instruction %d\n", (unsigned int)i);
       switch( i )
       {
          case OP_NOP:{ continue; }
          case OP_END: {
            if(vm_state->if_count>0)
              vm_state->if_count--;
              
            if(vm_state->state == vm_state_passive_frozen 
                && (vm_state->if_count==vm_state->if_depth))
               vm_state->state = vm_state_normal;
            
            continue;
          }
          case OP_NO: { 
             vm_state->state = vm_state_frozen;
             continue;
          }
          case OP_ENDNO: {
            vm_state->state = vm_state_normal;
            continue;
          }
          case OP_HLT: {
             g_Svm._sig_handler.raise_sig(SIGQUIT);
             continue;
          }
          case OP_RETURN: {
             func_return(vm_state->image_stream.valueAt(vm_state->pc++),
                   vm_state, env);
             continue;
          }
          case OP_COUT: {
             _stdout(data(vm_state->image_stream.valueAt(vm_state->pc++), vm_state), env);
             continue;
          }
       }
    }
 }
 
 