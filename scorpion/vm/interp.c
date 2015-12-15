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
#include "../clib/u1.h"
#include "../clib/u4.h"
#include "interp.h"
#include "Globals.h"
#include "exception.h"
#include "Opcodes.h"
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

#include "oo/Object.h"
#include "oo/Array.h"
#include "oo/String.h"
#include "alloc/gc.h"
using namespace std;

bool includep = true, isnative = false;
int svm_main = 0, g_max_precision=16;
u4_d op_ags;

void _cout_(string output);

int Scorpion_InvokeMain(ScorpionVmState* vm){
    return Scorpion_InvokeMethod(svm_main, vm, -1);
}

int Scorpion_InvokeMethod(long ptrValue, ScorpionVmState* vm, long line){
    
    if(ptrValue >= gSvm.methodc){
       stringstream ss;
       ss << "pointer to method: " << ptrValue << " is out of allocated method range.";
       Exception(ss.str(), "MethodNotFoundException");
    }
    
    if(includep){
        
        stringstream s;
        s << gSvm.mtds[ptrValue].module << "." << gSvm.mtds[ptrValue].name;

        Exception::trace.addproto(s.str(), gSvm.mtds[ptrValue].clazz, line, gSvm.mtds[ptrValue].native);
    } 
    else
      includep = true;
    
    gSvm.mtds[ptrValue].ref.byte1 = vm->k;
    vm->k = jmpLocation(gSvm.mtds[ptrValue]);
    
     return 0;
}

void return_main() // this is simple lol
{
   Init_ShutdownScorpionVM();
}
 
void return_method(long ptrValue, ScorpionVmState* vm){
    if(ptrValue >= gSvm.methodc){
       stringstream ss;
       ss << "pointer to method: " << ptrValue << " is out of allocated method range.";
       Exception(ss.str(), "MethodNotFoundException");
    }
    
    
    vm->k = returnLocation(gSvm.mtds[ptrValue]);
}

Object& MOJ(long i) { 
    if(i>gSvm.env->bitmap.MaxLimit)  
    {
       stringstream ss;
       ss << "pointer to address: " << i << " is out of allocated data range.";
       Exception(ss.str(), "MethodNotFoundException");
    }
    return gSvm.env->bitmap.objs[i]; 
}

bool isnumber(Object o)
{ return isgeneric(o.instanceData.byte1); }

void setvalue(int op, long i, Object v1, Object v2)
{
    if(!isnumber(MOJ(i))) 
       Exception("Cannot perforn arithmetic on a non numeric value.", "IllegalTypeException");
    
    if(op==OP_ADD) svmSetGenericValue(MOJ(i), svmGetGenericValue(v1)+svmGetGenericValue(v2));
    else if(op==OP_SUB) svmSetGenericValue(MOJ(i), svmGetGenericValue(v1)-svmGetGenericValue(v2));
    else if(op==OP_MULT) svmSetGenericValue(MOJ(i), svmGetGenericValue(v1)*svmGetGenericValue(v2));
    else if(op==OP_DIV) svmSetGenericValue(MOJ(i), svmGetGenericValue(v1)/svmGetGenericValue(v2));
    else if(op==OP_MOD) svmSetGenericValue(MOJ(i), (long)svmGetGenericValue(v1)%(long)svmGetGenericValue(v2));
}

int instrSize(ScorpionVmState* vm)
{
    unsigned long i = vm->i;
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
         return vm->bytestream.valueAt(vm->k++);
     else if(i == OP_STRCONST){ // string 13 'Hello, World!'
         vm->k++;
         return vm->bytestream.valueAt(vm->k++);
     }
}

#define arith_op(op, i, vmstate) {\
          Object v1 = MOJ(vmstate->bytestream.valueAt(vmstate->k++)); \
          Object v2 = MOJ(vmstate->bytestream.valueAt(vmstate->k++)); \
          if(isnumber(v1) && isnumber(v2)) \
            setvalue(op, i, v1, v2);\
          else { Exception("Cannot perforn arithmetic on non numeric values.", "IllegalTypeException"); } \
        }

#define GETARG_SIZE(vmstate)\
          instrSize(vmstate);

#define do_return(i, vmstate) { \
            long adr = i; \
            --vmstate->methodcount; \
            if((adr == 0) && (vmstate->methodcount <= 0)) \
            { \
               gSvm.vmstate = vmstate; \
               return_main(); \
            } \
            else return_method(adr, vmstate); \
        }

bool scorpionVm_strcompare(long op, Object &o, Object &o2)
{
	if(op == OP_ISLT)
       return (getstr(o)<getstr(o2));
    else if(op == OP_ISGT)
       return (getstr(o)>getstr(o2));   
    else if(op == OP_ISGE)
       return (getstr(o)>=getstr(o2));   
    else if(op == OP_ISLE)
       return (getstr(o)<=getstr(o2));   
    else if(op == OP_ISEQ)
       return (getstr(o)==getstr(o2)); 
    else return false;	
}

bool scorpionVm_numcompare(long op, Object &o, Object &o2)
{
   
    if(op == OP_ISLT)
       return (svmGetGenericValue(o)<svmGetGenericValue(o2));
    else if(op == OP_ISGT)
       return (svmGetGenericValue(o)>svmGetGenericValue(o2));   
    else if(op == OP_ISGE)
       return (svmGetGenericValue(o)>=svmGetGenericValue(o2));   
    else if(op == OP_ISLE)
       return (svmGetGenericValue(o)<=svmGetGenericValue(o2));   
    else if(op == OP_OR)
       return (svmGetGenericValue(o)||svmGetGenericValue(o2));   
    else if(op == OP_AND)
       return (svmGetGenericValue(o)&&svmGetGenericValue(o2));   
    else if(op == OP_ISEQ)
       return (svmGetGenericValue(o)==svmGetGenericValue(o2)); 	
    else return false;
}

bool scorpioncompare(long op, Object &o, Object &o2)
{
	if(isnumber(o) && isnumber(o2))
	  return scorpionVm_numcompare(op, o, o2);
	else if(__typedef(o) == TYPEDEF_STRING && __typedef(o2) == TYPEDEF_STRING)
	  return scorpionVm_strcompare(op, o, o2);
	else return false;
}

long GETTYPE(long i)
{
    if(i == OP_ICONST)
      return TYPEDEF_GENERIC_INT;
    else if(i == OP_SCONST) 
      return TYPEDEF_GENERIC_SHORT;
    else if(i == OP_CCONST) 
      return TYPEDEF_GENERIC_CHAR;
    else if(i == OP_LCONST) 
      return TYPEDEF_GENERIC_LONG;
    else if(i == OP_BCONST) 
      return TYPEDEF_GENERIC_BOOL;
    else if(i == OP_FCONST) 
      return TYPEDEF_GENERIC_FLOAT;
    else if(i == OP_DCONST) 
      return TYPEDEF_GENERIC_DOUBLE;
    else if(i == OP_BYTE_CONST) 
      return TYPEDEF_GENERIC_BYTE;
    else if(i == OP_BYTE_ACONST) 
      return TYPEDEF_GENERIC_BYTE_ARRAY;
    else if(i == OP_IACONST) 
      return TYPEDEF_GENERIC_INT_ARRAY;
    else if(i == OP_BACONST) 
      return TYPEDEF_GENERIC_BOOL_ARRAY;
    else if(i == OP_CACONST) 
      return TYPEDEF_GENERIC_CHAR_ARRAY;
    else if(i == OP_SACONST) 
      return TYPEDEF_GENERIC_SHORT_ARRAY;
    else if(i == OP_LACONST) 
      return TYPEDEF_GENERIC_LONG_ARRAY;
    else if(i == OP_STR_ACONST) 
      return TYPEDEF_STRING_ARRAY;
    else if(i == OP_FACONST) 
      return TYPEDEF_GENERIC_FLOAT_ARRAY;
    else if(i == OP_DACONST) 
      return TYPEDEF_GENERIC_DOUBLE_ARRAY;
    else return -1;
}

u4_d arguments; // our dedicated instruction arguments
    
int ch;
struct termios t_old, t_new;

void termreset()
{
    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
}
    
int _getch_() {
    
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}

void printf_obj_content(long addr, char form)
{
   if(isgenericarray(MOJ(addr).instanceData.byte1))
   {
     printf("%#x", (unsigned int) addr);
     return;
   }
  
  if(form == 'i')
    cout << (sdouble) ((sint) svmGetGenericValue(MOJ(addr)));
  else if(form == 's')
    cout << (sdouble) ((sshort) svmGetGenericValue(MOJ(addr)));
  else if(form == 'B')
    cout << (sdouble) ((sbyte) svmGetGenericValue(MOJ(addr)));
  else if(form == 'l')
    cout << (sdouble) ((slong) svmGetGenericValue(MOJ(addr)));
  else if(form == 'p')
    printf("%#x", (unsigned int) addr);
  else if(form == 'S' || MOJ(addr).instanceData.byte1 == TYPEDEF_STRING)
    _cout_(getstr(MOJ(addr)));
  else if(form == 'c' || MOJ(addr).instanceData.byte1 == TYPEDEF_GENERIC_CHAR)
    cout << (char) svmGetGenericValue(MOJ(addr));
  else if(form == 'b' || MOJ(addr).instanceData.byte1 == TYPEDEF_GENERIC_BOOL)
    cout << (((sbool) svmGetGenericValue(MOJ(addr)) == 1) ? "true" : "false");
  else if(form == 'f' || MOJ(addr).instanceData.byte1 == TYPEDEF_GENERIC_FLOAT)
    cout << std::setprecision((g_max_precision/2) - 1) << (sfloat) svmGetGenericValue(MOJ(addr));
  else if(form == 'd' || MOJ(addr).instanceData.byte1 == TYPEDEF_GENERIC_DOUBLE)
    cout << std::setprecision(g_max_precision) << (double) svmGetGenericValue(MOJ(addr));
  else
    cout << (sdouble) svmGetGenericValue(MOJ(addr));
}

void _cout_(string output)
{
   // cout << "outputting " << output << endl;
  for(long i = 0; i < output.size(); i++){
    if(output.at(i) != '\\'){
      cout << output.at(i);
      continue;
    }
    else {
       if(!((i + 1) < output.size())){
          cout << (char) 131;
          return;
        }
        
       
       char bad_char=(131);  
       i++;
       switch ( output.at(i) ){
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
                if(!((i) < output.size())){
                  cout << bad_char;
                  return;
                }
                int form = output.at(i);
                stringstream  ss;
                ss << "";
                i++;
                
                for(long i2 = i; i < output.size(); i++){
                    if(isdigit(output.at(i)))
                      ss << output.at(i);
                    else{
                      if(ss.str() == ""){
                         cout << bad_char;
                         continue;
                      }
                      
                      long addr = atoi(ss.str().c_str());
                      printf_obj_content(addr, form);
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
					for(long i2 = i; i < output.size(); i++){
						if(isdigit(output.at(i)))
						  ss << output.at(i);
						else{
						  if(ss.str() == ""){
							 cout << bad_char;
							 continue;
						  }
						  else if(output.at(i) != '}')
							 cout << bad_char;
						  
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
           default:
              cout << bad_char; continue;
       }
    }
  }
}

/**
 * Interpreter definitions
 * 
*/
#define vmdispatch(i) switch(i)
#define dispatchop(l,b)	case l: {b}  goto top;
#define dispatchopnb(l,b) case l: {b}		/* nb = no break */

void scorpionVmAssign(long o, long o2)
{
   if(isgeneric(__typedef(MOJ(o))) && isgeneric(__typedef(MOJ(o2))))
      svmSetGenericValue(MOJ(o), svmGetGenericValue(MOJ(o2)));
   else if(__typedef(MOJ(o)) == TYPEDEF_STRING && __typedef(MOJ(o2)) == TYPEDEF_STRING)
      assign(MOJ(o), getstr(MOJ(o2)));
   else if(isgenericarray(__typedef(MOJ(o))) && isgenericarray(__typedef(MOJ(o2))))
     arrycpy(MOJ(o), MOJ(o2));
   else
     Exception("Cannot assign data to objects of different types.", "IllegalTypeException");
}

string getStr(ScorpionVmState* vm, long len)
{
     char c;
     stringstream ss;
     for(long i = 0; i < len; i++){
         c = (char) vm->bytestream.valueAt(vm->k++);
         ss << "" << c;
     }
     return ss.str();
}

void Protect(ScorpionVmState* vm)
{
    if(vm->k>=vm->m)
      alog.ALOGV("Attempting to close program unexpectingly.");
    else if(vm->i>sMaxOpcodeLimit || vm->i<0)
    {
        stringstream ss;
        ss << "Attempting to execute invalid instruction (" << vm->i << ").";
        alog.ALOGV(ss.str());
    }
    
    gSvm.vmstate = vm;
    segfault();
}

// TODO: figure out how to add elseif and else in vm opcodes
// TODO: Finish implementing the rest of the instructions
// TODO: Have interpreter utilize Scorpion primitive types during data transfer
// TODO: implement GC check in instruction checkGC(gSvm.env->bitmap);      
void Scorpion_VMExecute(ScorpionVmState* vmstate){
  alog.ALOGD("running application.");
  termreset();
  
  vmstate->methodcount = 0;
  vmstate->m = vmstate->bytestream.size();
  
    // main loop for Scorpion interpreter
  for(;;) {
    top:
    if(vmstate->k>=vmstate->m)
      Protect(vmstate);
      
    vmstate->i = vmstate->bytestream.valueAt(vmstate->k++);
    
    if(vmstate->i>sMaxOpcodeLimit||vmstate->i<0) Protect(vmstate);
    //cout << "i=" << vmstate->i << endl;
    if((vmstate->status == vm_status_no_run && vmstate->i != OP_ENDNO) || 
       (vmstate->status == vm_status_if_ignore && !(vmstate->i == OP_END || vmstate->i == OP_IF))){ // do not run
        vmstate->k+=GETARG_SIZE(vmstate);
        goto top;
    }
        
    // TODO: Debugger run
   // if(gSvm.Debug)
      // do debug stuff

    vmdispatch(vmstate->i){ 
       dispatchop(OP_NOP, 
       )
       dispatchop(OP_END,
          if(vmstate->ifcount > 0)
              vmstate->ifcount--;
          
          if(vmstate->status == vm_status_if_ignore && (vmstate->ifcount == vmstate->ifdepth)){
             vmstate->status = vm_status_normal;
             vmstate->ifdepth = 0;
          }
       )
       dispatchop(OP_NO,
          vmstate->status = vm_status_no_run;
       )
       dispatchop(OP_ENDNO,
          vmstate->status = vm_status_normal;
       )
       dispatchop(OP_HLT,
          gSvm.vmstate = vmstate;
          Init_ShutdownScorpionVM();
       )
       dispatchop(OP_RETURN,
          do_return(vmstate->bytestream.valueAt(vmstate->k++), vmstate);
       )
       dispatchop(OP_COUT,
          _cout_(getStr(vmstate, vmstate->bytestream.valueAt(vmstate->k++)));
       )
       dispatchop(OP_INC,
          svmIncGenericValue(MOJ((long) vmstate->bytestream.valueAt(vmstate->k++)));
       )
       dispatchop(OP_DEC,
          svmDecGenericValue(MOJ((long) vmstate->bytestream.valueAt(vmstate->k++)));
       )
       dispatchop(OP_IF,
          /*
           * VFLAG_IF_DEPTH
           *
           * This is a special flag that tells the Scorpion virtual machine 
           * where the if statment was rejected.
           *
           * The control flow system:
           *
           * Evertime an "if" statement is executed, 
           * VFLAG_IFC is incremented. This flag holds the count 
           * of the "depth", i.e. how many if statements are we inside.
           *
           * We need this number when determining when to allow instructions 
           * to execute and when not to based on if the "if" statement evaluated to
           * true.
           *
           * if an "if" statement evaluates to false we need to save the place or "depth"
           * in which the if statement is in to preserve the block of code that should run after the 
           * "false if" block of code has been skipped.
           *
           * else if an if statement evaluates to true, we do nothing
           *
           * we only set the VFLAG_IF_DEPTH flag if VFLAG_IF_IGNORE evaluates to false.
           */
          if((vmstate->status == vm_status_normal) && 
            (svmGetGenericValue(MOJ((long) vmstate->bytestream.valueAt(vmstate->k++))) == 0))
                vmstate->status = vm_status_if_ignore;
                   
          if(vmstate->status == vm_status_if_ignore)
             vmstate->ifdepth = vmstate->ifcount;
              
          vmstate->ifcount++;
       )
       dispatchop(OP_PUSH, // does not require op_ags
          op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
          long stacksz = gSvm.env->bitmap.stack->length;
      
          if(++vmstate->sp >= stacksz) 
            Exception("The stack has overflowed with too much data.", "StackOverfowlException");
          //cout << "push @" << vmstate->sp << " ->" << (long) op_ags.byte1 << endl;
          gSvm.env->bitmap.stack->plong[vmstate->sp] = (long) op_ags.byte1;
       )
       dispatchop(OP_POP, // does not require op_ags
          op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
          if(vmstate->sp < 0)
              Exception("Failure to pull data from empty stack.", "StackUnderflowException");
          //  cout << "sp->" << vmstate->sp << endl;
         // cout << "pop @" << vmstate->sp << " ->" << (long) op_ags.byte1 << endl;
          scorpionVmAssign((long) op_ags.byte1, gSvm.env->bitmap.stack->plong[vmstate->sp--]);
       )
       dispatchop(OP_KILL,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           if(svmObjectIsAlive(MOJ(op_ags.byte1)))
           {
               svmDumpObject(MOJ(op_ags.byte1));
               checkGC(gSvm.env->bitmap);
           }
       )
       dispatchop(OP_NEG,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           svmSetGenericValue(MOJ(op_ags.byte1), 
               !((bool) svmGetGenericValue(MOJ(op_ags.byte1))));
       )
       dispatchopnb(OP_JMP,
           vmstate->k =  svmGetGenericValue(MOJ((long) vmstate->bytestream.valueAt(vmstate->k++)));
           goto top;
       )
       dispatchop(OP_CALL,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           vmstate->methodcount++;
           Scorpion_InvokeMethod((long) op_ags.byte1, vmstate, (long) op_ags.byte2);
       )
       dispatchop(OP_MTHD,
         vmstate->k++;
       )
       dispatchop(OP_LBL,
         vmstate->k++;
       )
       dispatchop(OP_DELETE,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           if(svmObjectIsAlive(MOJ(op_ags.byte1)))
               freeObj(MOJ(op_ags.byte1));
       )
       dispatchop(OP_CONST,
           long op = vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           u1 sz;
           sz.byte1 = 1;
           SVM_OBJECT_INIT(MOJ(op_ags.byte1), GETTYPE(op), sz);
           svmSetGenericValue(MOJ(op_ags.byte1), op_ags.byte2);
       )
       dispatchop(OP_STRCONST,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           u1 sz;
           sz.byte1 = 1;
           SVM_OBJECT_INIT(MOJ(op_ags.byte1), TYPEDEF_STRING, sz);
           assign(MOJ(op_ags.byte1), getStr(vmstate,op_ags.byte2));
       )
       dispatchop(OP_ACONST,
           long op = vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           u1 sz;
           sz.byte1 = svmGetGenericValue(MOJ(op_ags.byte2));
           SVM_OBJECT_INIT(MOJ(op_ags.byte1), GETTYPE(op), sz);
       )
       dispatchop(OP_CAST, // TODO: Make sure to cast everything (pointer casting)
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           sdouble newData;
           sdouble data = svmGetGenericValue(MOJ(op_ags.byte2));
           int cast = op_ags.byte1;
           if(cast == 1) newData = (sint) data;
           else if(cast == 2) newData = (sshort) data;
           else if(cast == 3) newData = (schar) data;
           else if(cast == 4) newData = (sbool) data;
           else if(cast == 5) newData = (sfloat) data;
           else if(cast == 7) newData = (slong) op_ags.byte2;
           else if(cast == 8) newData = (sbyte) op_ags.byte2;
           else if(cast == 9) newData = (slong) op_ags.byte2;
           else newData = data;
           
           svmSetGenericValue(MOJ(op_ags.byte2), newData);
       )
       dispatchopnb(OP_JIT,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           if((bool) svmGetGenericValue(MOJ(op_ags.byte2)) == 1)
               vmstate->k =  svmGetGenericValue(MOJ(op_ags.byte1));
           goto top;
       )
       dispatchopnb(OP_JIF,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           if((bool) svmGetGenericValue(MOJ(op_ags.byte2)) == 0)
               vmstate->k =  svmGetGenericValue(MOJ(op_ags.byte1));
           goto top;
       )
       dispatchop(OP_RSHFT,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           long num = svmGetGenericValue(MOJ(op_ags.byte1));
           num >>= (long) svmGetGenericValue(MOJ(op_ags.byte2));
           svmSetGenericValue(MOJ(op_ags.byte1), num);
       )
       dispatchop(OP_LSHFT,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           long num = svmGetGenericValue(MOJ(op_ags.byte1));
           num <<= (long) svmGetGenericValue(MOJ(op_ags.byte2));
           svmSetGenericValue(MOJ(op_ags.byte1), num);
       )
       dispatchop(OP_THROW,
           Exception(getstr(MOJ((long) vmstate->bytestream.valueAt(vmstate->k++))), 
           getstr(MOJ((long) vmstate->bytestream.valueAt(vmstate->k++))));
       )
       dispatchopnb(OP_CIN,
           svmSetGenericValue(MOJ((long) vmstate->bytestream.valueAt(vmstate->k++)), _getch_());
           goto top;
       )
       dispatchop(OP_STR_APND,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           
           if(isgeneric(__typedef(MOJ(op_ags.byte2))))
           {
			   stringstream ss;
			   ss.str("");
			   ss << (char) svmGetGenericValue(MOJ(op_ags.byte2));
               concat(MOJ(op_ags.byte1), ss.str());
		   }
		   else
		   {
             concat(MOJ(op_ags.byte1), 
                getstr(MOJ(op_ags.byte2)));
		   }
       )
       dispatchop(OP_ASSN,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           scorpionVmAssign((long) op_ags.byte1, (long) op_ags.byte2);
       )
       dispatchop(OP_AT,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte3=vmstate->bytestream.valueAt(vmstate->k++);
           
           svmSetGenericValue(MOJ(op_ags.byte1), 
              at(MOJ(op_ags.byte2), 
                svmGetGenericValue(MOJ(op_ags.byte3))));
       )
       dispatchop(OP_ALOAD,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte3=vmstate->bytestream.valueAt(vmstate->k++);
           
           long pos = svmGetGenericValue(MOJ(op_ags.byte3));
           if(isgeneric(__typedef(MOJ(op_ags.byte1)))){
               svmSetGenericValue(MOJ(op_ags.byte1), 
               get(MOJ(op_ags.byte2), pos));
           }
           else if(__typedef(MOJ(op_ags.byte2)) == TYPEDEF_STRING_ARRAY){
                str_location = pos;
                assign(MOJ(op_ags.byte1), 
                    getstr(MOJ(op_ags.byte2)));
           }
           else
             Exception("Object is not an array type.", "IncompatibleTypeException");
       )
       dispatchop(OP_ASTORE,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte3=vmstate->bytestream.valueAt(vmstate->k++);
           
           long pos = svmGetGenericValue(MOJ(op_ags.byte3));
           if(isgenericarray(__typedef(MOJ(op_ags.byte1)))){
               set(MOJ(op_ags.byte1), pos,
                   svmGetGenericValue(MOJ(op_ags.byte2)));
           }
           else if(__typedef(MOJ(op_ags.byte1)) == TYPEDEF_STRING_ARRAY){
               str_location = pos;
               assign(MOJ(op_ags.byte1), getstr(MOJ(op_ags.byte2)));
           }
           else
             Exception("Object is not an array type.", "IncompatibleTypeException");
       )
       dispatchop(OP_CMP,
           long op =vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
           op_ags.byte3=vmstate->bytestream.valueAt(vmstate->k++);
           
           svmSetGenericValue(MOJ(op_ags.byte1),
                  scorpioncompare(op, MOJ(op_ags.byte2),
                   MOJ(op_ags.byte3)));
       )
       dispatchop(OP_ADD,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           arith_op(OP_ADD, op_ags.byte1, vmstate);
       )
       dispatchop(OP_SUB,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           arith_op(OP_SUB, op_ags.byte1, vmstate);
       )
       dispatchop(OP_MULT,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           arith_op(OP_MULT, op_ags.byte1, vmstate);
       )
       dispatchop(OP_DIV,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           arith_op(OP_DIV, op_ags.byte1, vmstate);
       )
       dispatchop(OP_MOD,
           op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
           arith_op(OP_MOD, op_ags.byte1, vmstate);
       )
    }
  }
}
