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
 #include <stdlib.h>    
 #include <unistd.h>
 #include <stdio.h>
 #include <cstdlib> 
 #include <iostream>
 #include <termios.h>
 #include <unistd.h>
 #include <stdio.h>
 #include <iomanip>
 #include "../memory/block_allocator.h"
 #include "../log/log.h"
 #include "../log/log_service.h"
 #include "../../clib/arraylist.h"
 #include "../../libxso/xso_reader.h"
 #include "../memory/vm_env.h"
 #include "../memory/object_scheme_controller.h"
 #include "../io/signal_handler.h"
 #include "../memory/scorpion_assert.h"
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
 using namespace scorpionvm::memory::schema;
 using namespace scorpionvm::memory::environment;
 using namespace scorpionvm::log::log_service::debug;
 using namespace scorpionvm::memory::schema::object_scheme_controller;
 
 #define nullptr ((void *)0)
 
 typedef long long instruction;

 extern Log ldebug;
 long DOUBLE_MAX_PRECISION=16;
 void _stdout(string out, scorpion_env* en, ScorpionVM* vm);
 
 void segfault()
 {
    cout << "Segmentation fault\n"; 
    sig_handler::raise_sig(SIGQUIT);
 }
 
 void scorpionvm::vm::interpreter::Invoke_Method
        (scorpion_env* env, ScorpionVM* vm_state, uint64_t ptr, uint64_t l)
 {
     if(ptr > env->sizeinfo(3,0) || ptr < 0)
     {
         // error
         stringstream ss;
         ss << "Pointer to method at address #" << ptr << " is outside of allocated method space."
             << " Note: method size: " << env->sizeinfo(3,0);
         g_Svm.MethodIndexOutOfBoundsException.error(ss.str());
         g_Svm.MethodIndexOutOfBoundsException._throw_(vm_state);
     }
     
     vm_state->func_tracker.add_func(env->method_stack.valueAt(ptr), l);
         
     env->method_stack.valueAt(ptr).ret = vm_state->pc;
     vm_state->pc = env->method_stack.valueAt(ptr).jmp;
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
 
 void check_obj(long addr, scorpion_env* env, ScorpionVM* vm)
 {
    if(addr >= env->sizeinfo(0,0)) g_Svm.HeapIndexOutOfBoundsException._throw_(vm);
    if(!env->m_heap[addr].init) g_Svm.NullPointerException._throw_(vm); 
 }
       
 void return_method(long ptr, ScorpionVM* vm, scorpion_env* env){
     if(ptr >= env->sizeinfo(3,0)){
         // error
         stringstream ss;
         ss << "Pointer to method at address #" << ptr << " is outside of allocated method space."
             << " Note: method size: " << env->sizeinfo(3,0);
         g_Svm.MethodIndexOutOfBoundsException.error(ss.str());
         g_Svm.MethodIndexOutOfBoundsException._throw_(vm);
     }
     
     
     vm->pc = env->method_stack.valueAt(ptr).ret;
 }

 string data(long size_t, ScorpionVM* vm)
 {
     stringstream ss;
     for(long i = 0; i < size_t; i++)
         ss << (char)vm->image_stream.valueAt(vm->pc++);
     
     return ss.str();
 }

 void _printf_obj(long addr, char form, scorpion_env* env, ScorpionVM* vm)
 {
     //check_obj(addr,env,vm);
     
     if(env->m_heap[addr].array)
     {
         printf("%#x", (unsigned int) addr);
         return;
     }
     
      if(form == 'i')
        cout << (sdouble) ((sint) env->m_heap[addr].schema_value());
      else if(form == 's')
        cout << (sdouble) ((sshort)  env->m_heap[addr].schema_value());
      else if(form == 'B')
        cout << (sdouble) ((sbyte)  env->m_heap[addr].schema_value());
      else if(form == 'l')
        cout << (sdouble) ((slong)  env->m_heap[addr].schema_value());
      else if(form == 'p')
        printf("%#x", (unsigned int) addr);
      else if(form == 'S' || env->m_heap[addr].valuetype->type == ObjectSchema::SSTRING)
        _stdout(env->m_heap[addr].string_schema_get_(), env,vm);
      else if(form == 'c' || env->m_heap[addr].valuetype->type == ObjectSchema::SCHAR)
        cout << (char) env->m_heap[addr].schema_value();
      else if(form == 'b' || env->m_heap[addr].valuetype->type == ObjectSchema::SBOOL)
        cout << (((sbool) env->m_heap[addr].schema_value() == 1) ? "true" : "false");
      else if(form == 'f' || env->m_heap[addr].valuetype->type == ObjectSchema::SFLOAT)
        cout << std::setprecision((DOUBLE_MAX_PRECISION/2) - 1) << (sfloat) env->m_heap[addr].schema_value();
      else if(form == 'd' || env->m_heap[addr].valuetype->type == ObjectSchema::SDOUBLE)
        cout << std::setprecision(DOUBLE_MAX_PRECISION) << (double) env->m_heap[addr].schema_value();
      else
        cout << (sdouble) env->m_heap[addr].schema_value();
 }
 
 void _stdout(string out, scorpion_env* env, ScorpionVM* vm)
 {
    // cout << "outputting " << out << endl;
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
            case '[': // print variable data example:  $[v483;
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
                       _printf_obj(addr, form, env, vm);
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
 
  ObjectSchema::t OPTYPE(long op)
  {
      switch( op ) // TODO: Do arrays
      {
          case OP_ICONST:
          case OP_IACONST:
            return ObjectSchema::SINT;
          case OP_SCONST:
          case OP_SACONST:
            return ObjectSchema::SSHORT;
          case OP_LCONST:
          case OP_LACONST:
            return ObjectSchema::SLONG;
          case OP_BYTE_CONST:
          case OP_BYTE_ACONST:
            return ObjectSchema::SBYTE;
          case OP_CCONST:
          case OP_CACONST:
            return ObjectSchema::SCHAR;
          case OP_DCONST:
          case OP_DACONST:
            return ObjectSchema::SDOUBLE;
          case OP_FCONST:
          case OP_FACONST:
            return ObjectSchema::SFLOAT;
          case OP_BCONST:
          case OP_BACONST:
            return ObjectSchema::SBOOL;
          default:
            return ObjectSchema::SINT;
      }
  }
 
 double scorpion_cast(int cast, double data, long addr)
 {
     if(cast == 1) return (schar) data;
     else if(cast == 2) return (sbyte) data;
     else if(cast == 3) return (sint) data;
     else if(cast == 4) return (sshort) data;
     else if(cast == 5) return (slong) data;
     else if(cast == 6) return (slong) addr;
     else if(cast == 7) return (sbool) data;
     else if(cast == 8) return (sfloat) data;
     else return data;
 }
 
 bool scorpion_numcompare(long op, double a, double b)
{
   
    if(op == OP_ISLT)
       return (a<b);
    else if(op == OP_ISGT)
       return (a>b);   
    else if(op == OP_ISGE)
       return (a>=b);   
    else if(op == OP_ISLE)
       return (a<=b);   
    else if(op == OP_OR)
       return (a||b);   
    else if(op == OP_AND)
       return (a&&b);   
    else if(op == OP_ISEQ)
       return (a==b); 	
    else return false;
}
 
bool scorpion_strcompare(long op, string str, string str2)
{
	if(op == OP_ISLT)
       return (str<str2);
    else if(op == OP_ISGT)
       return (str>str2);   
    else if(op == OP_ISGE)
       return (str>=str2);   
    else if(op == OP_ISLE)
       return (str<=str2);   
    else if(op == OP_ISEQ)
       return (str==str2); 
    else return false;	
}
 
 bool scorpion_Vcompare(long id2, long id3, scorpion_env* env, long op, ScorpionVM* vm)
 {
     //check_obj(id2,env, vm);
     //check_obj(id3,env,vm);
     
     if(isnumber(env->m_heap[id2]) && isnumber(env->m_heap[id3]))
       return scorpion_numcompare
         (op,env->m_heap[id2].schema_value(),env->m_heap[id3].schema_value());
     else if(env->m_heap[id2].string_ && env->m_heap[id3].string_)
       return scorpion_strcompare
           (op,env->m_heap[id2].string_schema_get_(),env->m_heap[id3].string_schema_get_());
     else return false;
 }
 
 /**
  * Scorpion macros
  * These macros sub routines are used to segment 
  * off code from the interpreter to make code cleaner and faster
  * 
  */
 #define scorpion_Vassign(i,i2,env) { \
           long id = i, id2 = i2; \
           if(isnumber(env->m_heap[id]) && isnumber(env->m_heap[id2])) \
               env->m_heap[id].set_schema_(env->m_heap[id2].schema_value());  \
           else if(env->m_heap[id].array && env->m_heap[id2].array) \
               arrycpy(env->m_heap[id], env->m_heap[id2]); \
           else if(env->m_heap[id].string_ && env->m_heap[id2].string_) \
               env->m_heap[id].string_schema_set_(env->m_heap[id2].string_schema_get_(),false); \
       }
 
 #define func_return(i, scorpion_Vstate, env) { \
            long long adr = i; \
            --scorpion_Vstate->func_depth; \
            if((adr == 0) && (scorpion_Vstate->func_depth <= 0)) { \
              ldebug.LOGA("Exiting main() function: __init__"); \
              sig_handler::raise_sig(SIGQUIT); \
            } \
            else return_method(adr, scorpion_Vstate, env); \
        }

 void scorpion_Vinc_dec(int op, long addr, scorpion_env* env, ScorpionVM* vm)
 {
   //check_obj(addr,env,vm); 
   if(isnumber(env->m_heap[addr])) 
   {
       if(op == OP_INC)
        env->m_heap[addr].schema_inc(); 
      else
        env->m_heap[addr].schema_dec(); 
      return;
   }
   else
   {
       g_Svm.RuntimeException.error("Cannot perform math on num math object");
       g_Svm.RuntimeException._throw_(vm);
   }
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
    vm_state->sp=-1;
    vm_state->func_depth=1;
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
             sig_handler::raise_sig(SIGQUIT);
             continue;
          }
          case OP_RETURN: {
             func_return(vm_state->image_stream.valueAt(vm_state->pc++),
                   vm_state, env);
             continue;
          }
          case OP_COUT: {
             _stdout(data(vm_state->image_stream.valueAt(vm_state->pc++), vm_state), env, vm_state);
             continue;
          }
          case OP_INC: {
              scorpion_Vinc_dec(i,vm_state->image_stream.valueAt(vm_state->pc++), env,vm_state);
              continue;
          }
          case OP_DEC: {
              scorpion_Vinc_dec(i,vm_state->image_stream.valueAt(vm_state->pc++), env,vm_state);
              continue;
          }
          case OP_IF: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              //check_obj(id,env,vm_state);
              if(vm_state->state == vm_state_normal &&
                 (bool)env->m_heap[id].schema_value() == 0)
              {
                  vm_state->if_depth = vm_state->if_count;
                  vm_state->state = vm_state_passive_frozen;
              }
                  
              vm_state->if_count++;
              continue;
          }
          case OP_PUSH: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              //check_obj(id,env,vm_state);
              
              if(++vm_state->sp >= env->sizeinfo(1,0)) g_Svm.StackOverflowException._throw_(vm_state);
              env->m_stack[vm_state->sp] = id;
              continue;
          }
          case OP_POP: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              //check_obj(id,env, vm_state);
              
              if(vm_state->sp > 0) g_Svm.StackUnderflowException._throw_(vm_state);
              //check_obj(env->m_stack[vm_state->sp],env, vm_state);
              scorpion_Vassign(id,env->m_stack[vm_state->sp--],env);
              continue;
          }
          case OP_KILL: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              if(id >= env->sizeinfo(0,0)) g_Svm.HeapIndexOutOfBoundsException._throw_(vm_state);
              
              kill_object(env->m_heap[id]);
              continue;
          }
          case OP_NEG: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              //check_obj(id,env,vm_state);
              
              env->m_heap[id].set_schema_(!((bool)env->m_heap[id].schema_value()));
              continue;
          }
          case OP_JMP: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              //check_obj(id,env,vm_state);
              
              vm_state->pc = env->m_heap[id].schema_value();
              goto newframe;
          }
          case OP_CALL: {
              vm_state->func_depth++;
              Invoke_Method(env, vm_state, vm_state->image_stream.valueAt(vm_state->pc), 
                   vm_state->image_stream.valueAt(vm_state->pc+1));
              continue;
          }
          case OP_MTHD: {
              vm_state->pc++;
              continue;
          }
          case OP_LBL: {
              vm_state->pc++;
              continue;
          }
          case OP_DELETE: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              if(id >= env->sizeinfo(0,0)) g_Svm.HeapIndexOutOfBoundsException._throw_(vm_state);
              
              delete_object(env->m_heap[id]);
              continue;
          }
          case OP_CONST: {
              long op = vm_state->image_stream.valueAt(vm_state->pc++);
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              if(id >= env->sizeinfo(0,0)) g_Svm.HeapIndexOutOfBoundsException._throw_(vm_state);
              
              create_object(env->m_heap[id], OPTYPE(op), 0);
              env->m_heap[id].set_schema_(vm_state->image_stream.valueAt(vm_state->pc++));
              continue;
          }
          case OP_STRCONST: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              if(id >= env->sizeinfo(0,0)) g_Svm.HeapIndexOutOfBoundsException._throw_(vm_state);
              
              create_object(env->m_heap[id], ObjectSchema::SSTRING, 0);
              env->m_heap[id].string_schema_set_
                  (data(vm_state->image_stream.valueAt(vm_state->pc++), vm_state), false);
              continue;
          }
          case OP_ACONST: {
              long op = vm_state->image_stream.valueAt(vm_state->pc++);
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              long id2 = vm_state->image_stream.valueAt(vm_state->pc++);
              if(id >= env->sizeinfo(0,0)) g_Svm.HeapIndexOutOfBoundsException._throw_(vm_state);
              //check_obj(id2,env,vm_state);
              
              create_object(env->m_heap[id], OPTYPE(op), env->m_heap[id2].schema_value());
              continue;
          }
          case OP_CAST: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              long id2 = vm_state->image_stream.valueAt(vm_state->pc++);
              long id3 = vm_state->image_stream.valueAt(vm_state->pc++);
              //check_obj(id,env,vm_state);
              //check_obj(id2,env,vm_state);
              
              env->m_heap[id].set_schema_(scorpion_cast
                   (id3, env->m_heap[id2].schema_value(), id2));
              continue;
          }
          case OP_JIF:
          case OP_JIT: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              long id2 = vm_state->image_stream.valueAt(vm_state->pc++);
              //check_obj(id,env,vm_state);
              //check_obj(id2,env,vm_state);
              
              if((bool) env->m_heap[id2].schema_value() == ((i==OP_JIT) ? 1 : 0))
                vm_state->pc = env->m_heap[id].schema_value();
              goto newframe;
          }
          case OP_LSHFT:
          case OP_RSHFT: {
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              long id2 = vm_state->image_stream.valueAt(vm_state->pc++);
              if(id >= env->sizeinfo(0,0)||id2 >= env->sizeinfo(0,0)) g_Svm.HeapIndexOutOfBoundsException._throw_(vm_state);
              
              env->m_heap[id].schema_shift
                 (env->m_heap[id2].schema_value(), ((i==OP_RSHFT) ? true : false));
              continue;
          }
          case OP_CMP: {
              long op = vm_state->image_stream.valueAt(vm_state->pc++);
              long id = vm_state->image_stream.valueAt(vm_state->pc++);
              long id2 = vm_state->image_stream.valueAt(vm_state->pc++);
              long id3 = vm_state->image_stream.valueAt(vm_state->pc++);
              //check_obj(id,env,vm_state);
              //check_obj(id2,env,vm_state);
              //check_obj(id3,env,vm_state);
              
              env->m_heap[id].set_schema_(scorpion_Vcompare(id2,id3,env,op,vm_state));
              continue;
          }
          default: {
              scorpion_assert(!( i>sMaxOpcodeLimit || i<0 ), "!( i>sMaxOpcodeLimit || i<0 )");
          }
       }
    }
 }
 
 