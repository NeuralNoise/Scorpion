#include "../clib/u1.h"
#include "interp.h"
#include "imgholder.h"
#include "Globals.h"
#include "exception.h"
#include "Opcodes.h"
#include <string>
#include <stdlib.h>    
#include <unistd.h>
#include <stdio.h>
#include <cstdlib> 
#include <iostream>

#include "oo/Object.h"
#include "oo/Array.h"
#include "oo/String.h"
#include "alloc/gc.h"
using namespace std;

bool includep = true, isnative = false;
int svm_main = 0;

stringstream mthdname, classname, modulename;
int mthdptr = 0;
void _cout_(string output);

void getMethodName(long ptr){
    
    mthdname.str("");
    classname.str("");
    modulename.str("");
    
    mthdname << gSvm.mtds[ptr].name;
    classname << gSvm.mtds[ptr].clazz;
    modulename << gSvm.mtds[ptr].module;
    isnative = gSvm.mtds[ptr].native;
    mthdptr = jmpLocation(gSvm.mtds[ptr]);
}

int Scorpion_InvokeMain(){
    return Scorpion_InvokeMethod(svm_main);
}

int Scorpion_InvokeMethod(long ptrValue){
    if(ptrValue >= gSvm.methodc){
       stringstream ss;
       ss << "pointer to method: " << ptrValue << " is out of allocated method range.";
       Exception(ss.str(), "MethodNotFoundException");
    }
    
    if(includep){
    
        getMethodName(ptrValue);
        
        stringstream s;
        s << modulename.str() << "." << mthdname.str();

        Exception::trace.addproto(s.str(), classname.str(), isnative);
    }
    else
      includep = true;
    
    long pc = gSvm.vm.vStaticRegs[VREG_PC];
    gSvm.vm.vStaticRegs[VREG_PC] = mthdptr;
    gSvm.mtds[ptrValue].ref.byte1 = pc;
    
     return 0;
}

void return_main() // this is simple lol
{
   Init_ShutdownScorpionVM();
}
 
void return_method(long addr){
    if(addr >= gSvm.methodc){
       stringstream ss;
       ss << "pointer to method: " << addr << " is out of allocated method range.";
       Exception(ss.str(), "MethodNotFoundException");
    }
    
    gSvm.vm.vStaticRegs[VREG_PC] = returnLocation(gSvm.mtds[addr]);
}

u4_d arguments; // our dedicated instruction arguments
long compare(long instruction){
     double a,b;
     a = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte2]);
     b = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte3]);
     
     if(instruction == OP_ISEQ)
        return a == b;
     if(instruction == OP_ISGE)
        return a >= b;
     if(instruction == OP_ISGT)
        return a > b;
     if(instruction == OP_ISLE)
        return a <= b;
     if(instruction == OP_ISLT)
        return a < b;
     if(instruction == OP_ISNEQ)
        return a != b;
     if(instruction == OP_ISNGE)
        return !(a >= b);
     if(instruction == OP_ISNGT)
        return !(a > b);
     if(instruction == OP_ISNLE)
        return !(a <= b);
     if(instruction == OP_ISNLT)
        return !(a < b);
     if(instruction == OP_OR)
        return a || b;
     if(instruction == OP_AND)
        return a && b;
     else
        return 0;
}

void printf_obj_content(long addr, char form)
{
   if(gSvm.env->getBitmap().objs[addr].instanceData.byte1 == TYPEDEF_GENERIC_ARRAY ||
      gSvm.env->getBitmap().objs[addr].instanceData.byte1 == TYPEDEF_STRING_ARRAY)
   {
     printf("0x%08x", (unsigned int) addr);
     return;
   }
   
  if(form == 'c')
    cout << (char) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 'b')
    cout << (bool) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 'f')
    cout << (float) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 'd')
    cout << (double) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 'i')
    cout << (long) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 's')
    cout << (int) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 'S')
    _cout_(fromchararray(gSvm.env->getBitmap().objs[addr].obj->strobj->array[default_loc]));
  else //form == v
    cout << svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
}

double math(long instruction){
     double a,b;
     a = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte2]);
     b = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte3]);
     
     if(instruction == OP_IADD)
        return (long) (a + b);
     if(instruction == OP_ISUB)
        return (long) (a - b);
     if(instruction == OP_IDIV)
        return (long) (a / b);
     if(instruction == OP_IMULT)
        return (long) (a * b);
     if(instruction == OP_SADD)
        return (int) (a + b);
     if(instruction == OP_SSUB)
        return (int) (a - b);
     if(instruction == OP_SDIV)
        return (int) (a / b);
     if(instruction == OP_SMULT)
        return (int) (a * b);
     if(instruction == OP_DADD)
        return (double) (a + b);
     if(instruction == OP_DSUB)
        return (double) (a - b);
     if(instruction == OP_DDIV)
        return (double) (a / b);
     if(instruction == OP_DMULT)
        return (double) (a * b);
     if(instruction == OP_FADD)
        return (float) (a + b);
     if(instruction == OP_FSUB)
        return (float) (a - b);
     if(instruction == OP_FDIV)
        return (float) (a / b);
     if(instruction == OP_FMULT)
        return (float) (a * b);
     if(instruction == OP_CADD)
        return (char) (a + b);
     if(instruction == OP_CSUB)
        return (char) (a - b);
     if(instruction == OP_CDIV)
        return (char) (a / b);
     if(instruction == OP_CMULT)
        return (char) (a * b);
     if(instruction == OP_IMOD)
        return (long) a % (long) b;
     if(instruction == OP_SMOD)
        return (int) a % (int) b;
     if(instruction == OP_CMOD)
        return (char) a % (char) b;
     else
        return 0;
}

void _cout_(string output)
{
  for(long i = 0; i < output.size(); i++){
    if(output.at(i) != '$'){
      cout << output.at(i);
      continue;
    }
    else {
       if(!((i + 1) < output.size())){
          cout << (char) 244;
          return;
        }
        
       
       char bad_char=(131);  
       i++;
       switch ( output.at(i) ){
           case '$':
              cout << '$'; continue;
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
           case '"':
              cout << "\""; continue;
           case '\'':
              cout << "'"; continue;
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
                         return;
                      }
                      
                      long addr = atoi(ss.str().c_str());
                      printf_obj_content(addr, form);
                      continue;
                    }
                }
                break;
              }
           case '#':
              cout << std::flush; continue;
           case '!': // TODO: change color
              {
                return;
              }
           default:
              cout << bad_char; continue;
       }
    }
  }
}

// TODO: Finish implementing the rest of the instructions
// TODO: implement GC check in instruction checkGC(gSvm.env->bitmap);      
void Scorpion_VMExecute(){
  alog.ALOGD("running application.");
  
  long i, g; // our dedicated instruction and igroup
  exe:
   // cout << "exe" << endl;
    i = gSvm.appholder.getNextInstr();
    g = gSvm.appholder.getGroup();
   // cout << "i=" << i << endl;
    if(i == tok_eof){
      alog.ALOGV("fatal error: application attempting to close unexpectingly.");
      segfault();
    }
    else if(i == tok_floating){
      alog.ALOGV("fatal error: attempting to execute unknown instruction.");
      segfault();
    }

    if(gSvm.vm.flags[VFLAG_NO] == 1 && i != OP_ENDNO) // do not run
        goto exe;
        
    if(gSvm.vm.flags[VFLAG_IF_IGNORE] == 1 && (i != OP_END || i != OP_IF)) // do not run
        goto exe; 
        
    arguments = gSvm.appholder.getAgs();
    // TODO: Debugger run
   // if(gSvm.Debug)
      // do debug stuff

    if(g == 0)
      goto group0;
    if(g == 1)
      goto group1;
    if(g == 2)
      goto group2;
    if(g == 3)
      goto group3;
    if(g == 4)
      goto group4;
    else{
      alog.ALOGV("unspecified fatal error: could not find executable group.");
      segfault();
    }

    group0:
       switch( i ) {
          case OP_NOP: goto exe; // do nothing
          case OP_END: 
              if(gSvm.vm.flags[VFLAG_IFC] > 0)
                  gSvm.vm.flags[VFLAG_IFC]--;
                  
              if(gSvm.vm.flags[VFLAG_IF_IGNORE] && (gSvm.vm.flags[VFLAG_IFC] == gSvm.vm.flags[VFLAG_IF_DEPTH])){
                 gSvm.vm.flags[VFLAG_IGNORE] = false;
                 gSvm.vm.flags[VFLAG_IF_IGNORE] = false;
              }
          goto exe;
          case OP_NO:
               gSvm.vm.flags[VFLAG_NO] == 1; goto exe;
          case OP_ENDNO:
               gSvm.vm.flags[VFLAG_NO] == 0; goto exe;
          case OP_HLT:
               Init_ShutdownScorpionVM();
       } // run each instr
       goto exe; // we do this for non implemented instructions
    group1:
       switch( i ) {
          case OP_RETURN:
               if(((long) arguments.byte1 == 0) && (--gSvm.vm.flags[VFLAG_MTHDC] <= 0))
                  return_main();
               else
                 return_method(arguments.byte1);
          goto exe;
          case OP_INC:
               svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], 
                      svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1]) + 1);
          goto exe;
          case OP_DEC:
               svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], 
                      svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1]) - 1);
          goto exe;
          case OP_IF:
               /*
               * VFLAG_IF_DEPTH
               *
               * This is a special flag that tells the Scorpion virtual machine 
               * that here if where the if was rejected.
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
               if(!gSvm.vm.flags[VFLAG_IF_IGNORE])
                  gSvm.vm.flags[VFLAG_IF_DEPTH] = gSvm.vm.flags[VFLAG_IFC];
                  
               gSvm.vm.flags[VFLAG_IFC]++;
               
               if(!gSvm.vm.flags[VFLAG_IF_IGNORE] && !svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1])){
                   gSvm.vm.flags[VFLAG_IF_IGNORE] = 1;
                   gSvm.vm.flags[VFLAG_IGNORE] = 1;
               }
          goto exe;
          case OP_PUSH:
            {
               long stacksz = gSvm.env->getBitmap().stack->length;
      
               if(++gSvm.vm.vStaticRegs[VREG_SP] >= stacksz) 
                 Exception("The stack has overflowed with too much data.", "StackOverfowlException");
            //   cout << "push @" << gSvm.vm.vStaticRegs[VREG_SP] << " ->" << (long) arguments.byte1 << endl;
               gSvm.env->getBitmap().stack->generic[gSvm.vm.vStaticRegs[VREG_SP]] = (long) arguments.byte1;
            }
          goto exe;
          case OP_POP:
               if(gSvm.vm.vStaticRegs[VREG_SP] < 0)
                   Exception("Failure to pull data from empty stack.", "StackUnderflowException");
                
               svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], 
                      gSvm.env->getBitmap().stack->generic[gSvm.vm.vStaticRegs[VREG_SP]--]);
          goto exe;
          case OP_KILL:
               {
                   if(!svmObjectIsAlive(gSvm.env->getBitmap().objs[(long) arguments.byte1]))
                     goto exe;
                     
                   svmDumpObject(gSvm.env->getBitmap().objs[(long) arguments.byte1]);
                   checkGC(gSvm.env->bitmap);
               }
          goto exe;
          case OP_JMP:
               gSvm.vm.vStaticRegs[VREG_PC] =  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1]);
          goto exe;
          case OP_CALL:
               {
                 gSvm.vm.flags[VFLAG_MTHDC]++;
                 long m = Scorpion_InvokeMethod((long) arguments.byte1);
                 if(m != 0)
                   Exception("Failure to invoke unknown method.", "MethodInvocationFailure");
               }
          goto exe;
          case OP_SLP:
                {
                    long time = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1]);
                    sleep(time);
                }
          goto exe;
          case OP_USLP:
               {
                   long time = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1]);
                   usleep(time);
               }
          goto exe;
          case OP_MTHD: goto exe; // this instruction does nothing, it was executed during vm init
          case OP_LBL: goto exe;  // this instruction does nothing, it was executed during vm init
          default:
             if(i == OP_DELETE || i == OP_DELETE_ARRY){
                 if(!svmObjectIsAlive(gSvm.env->getBitmap().objs[(long) arguments.byte1]))
                         goto exe;
                         
                   freeObj(gSvm.env->getBitmap().objs[(long) arguments.byte1]); // arrays and generic objects can be freed the same way
             }
          goto exe;
       } // run each instr
       goto exe;
    group2:
       switch( i ) {
          case OP_ICONST:
               {
                  //  cout << "iconst @" << (long) arguments.byte1 << " ->" << (long) arguments.byte2 << endl;
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->bitmap.objs[(long) arguments.byte1], TYPEDEF_GENERIC, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], (long) arguments.byte2);
               }
          goto exe;
          case OP_DCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->bitmap.objs[(long) arguments.byte1], TYPEDEF_GENERIC, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], arguments.byte2);
               }
          goto exe;
          case OP_FCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) arguments.byte1], TYPEDEF_GENERIC, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], (float) arguments.byte2);
               }
          goto exe;
          case OP_SCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) arguments.byte1], TYPEDEF_GENERIC, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], (int) arguments.byte2);
               }
          goto exe;
          case OP_BCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) arguments.byte1], TYPEDEF_GENERIC, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], (bool) arguments.byte2);
               }
          goto exe;
          case OP_CCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) arguments.byte1], TYPEDEF_GENERIC, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], (char) arguments.byte2);
               }
          goto exe;
          case OP_JIT:
               if(svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte2]) == 1)
                  gSvm.vm.vStaticRegs[VREG_PC] =  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1]);
          goto exe;
          case OP_JIF:
               if(svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte2]) == 0)
                  gSvm.vm.vStaticRegs[VREG_PC] =  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1]);
          goto exe;
          case OP_RSHFT:
               {
                 long num = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1]);
                 num >> (long) arguments.byte2;
                 svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte2], num);
               }
          goto exe;
          case OP_LSHFT:
               {
                 long num = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1]);
                 num << (long) arguments.byte2;
                 svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte2], num);
               }
          goto exe;
          case OP_THROW: 
                   Exception(fromchararray(gSvm.env->getBitmap().objs[(long) arguments.byte1].obj->strobj->array[default_loc]), 
                             fromchararray(gSvm.env->getBitmap().objs[(long) arguments.byte2].obj->strobj->array[default_loc]));
          goto exe;
          case OP_CIN:
               {
                  system("stty raw");
                  int i;
                  
                  if(arguments.byte2 == 0){ // do not print char to screen
                    i = getchar();
                    cout << "\b \b" << std::flush;
                  }
                  else
                    i = getchar();
                  system("stty cooked");
                  
                  svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], i);
               }
          goto exe;
          case OP_STR_APND:
                 concat(gSvm.env->getBitmap().objs[(long) arguments.byte1], 
                        fromchararray(gSvm.env->getBitmap().objs[(long) arguments.byte2].obj->strobj->array[default_loc]));
          goto exe;
          case OP_ASSN:
                 gSvm.env->getBitmap().objs[(long) arguments.byte1] = gSvm.env->getBitmap().objs[(long) arguments.byte2];
          goto exe;
       } // run each instr
       goto exe;
    group3:
       switch( i ) {
          case OP_AT:
               {
                   svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], 
                          at(gSvm.env->getBitmap().objs[(long) arguments.byte2], (long) arguments.byte3));
               }
          goto exe;
          case OP_ALOAD:
               {
                   long pos = (long) arguments.byte3;
                   if(__typedef(gSvm.env->getBitmap().objs[(long) arguments.byte2]) == TYPEDEF_GENERIC_ARRAY){
                       svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte2], 
                           get(gSvm.env->getBitmap().objs[(long) arguments.byte2], pos));
                   }
                   else {
                       str_location = pos;
                       string data = getstr(gSvm.env->getBitmap().objs[(long) arguments.byte2]);
                       assign(gSvm.env->getBitmap().objs[(long) arguments.byte1], data);
                   }
               }
          goto exe;
          case OP_ASTORE:
               {
                   long pos = (long) arguments.byte3;
                   if(__typedef(gSvm.env->getBitmap().objs[(long) arguments.byte2]) == TYPEDEF_GENERIC_ARRAY){
                       set(gSvm.env->getBitmap().objs[(long) arguments.byte1], pos,
                           svmGetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte2]));
                   }
                   else {
                       str_location = pos;
                       string data = getstr(gSvm.env->getBitmap().objs[(long) arguments.byte2]);
                       assign(gSvm.env->getBitmap().objs[(long) arguments.byte1], data);
                   }
               }
          goto exe;
          default:
               if(i == OP_ISEQ || i == OP_ISNEQ || i == OP_ISLT || i == OP_ISNLT || i == OP_ISLE || i == OP_ISNLE
                  || i == OP_ISGT || i == OP_ISNGT || i == OP_ISGE || i == OP_ISNGE || i == OP_OR || i == OP_AND)
               svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], compare(i));
               else if(i == OP_IADD || i == OP_ISUB || i == OP_IMULT || i == OP_IDIV || i == OP_SADD
                  || i == OP_SSUB || i == OP_SMULT || i == OP_SDIV || i == OP_DADD
                  || i == OP_DSUB || i == OP_DMULT || i == OP_DDIV || i == OP_FADD
                  || i == OP_FSUB || i == OP_FMULT || i == OP_FDIV || i == OP_CADD
                  || i == OP_CSUB || i == OP_CMULT || i == OP_CDIV || i == OP_IMOD 
                  || i == OP_SMOD || i == OP_CMOD)
              svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arguments.byte1], math(i));
          goto exe;
       } // run each instr
       goto exe;
    group4:
       switch( i ) {
          case OP_STRCONST:
               {
                 string output =  gSvm.appholder.getStr();
                 u1 sz;
                 sz.byte1 = 1;
                 SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) arguments.byte1], TYPEDEF_STRING, sz);
                 assign(gSvm.env->getBitmap().objs[(long) arguments.byte1], output);
               }
          goto exe;
          case OP_COUT: // output data to the console;
              _cout_(gSvm.appholder.getStr());
          goto exe;
       } // run each instr
       goto exe;

}
