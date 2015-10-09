#include "../clib/u1.h"
#include "interp.h"
#include "alloc/BlockTable.h"
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
using namespace std;

bool includep = true, isnative = false;
int svm_main = 0;

stringstream mthdname, classname, modulename;
int mthdptr = 0;

void getMethodName(long ptr){
    mthdname.str("");
    classname.str("");
    modulename.str("");
    
    mthdptr = svmBlockFromAddr(gSvm.env->getBlockTable(), METHOD_BLOCK, ptr + 1);
    if(proto == "")
       return;
       
    int pos = 0;
    if(proto.at(0) == '~'){
       pos = 1;
       isnative = true;
    }
    
    for(int i = pos; i < proto.size(); i++){
        if(proto.at(i) == '&'){
            pos++;
            break;
        }
        else
          mthdname << "" << proto.at(i);
        pos++;
    }
    
    for(int i = pos; i < proto.size(); i++){
        if(proto.at(i) == '&'){
            pos++;
            break;
        }
        else
          classname << "" << proto.at(i);
        pos++;
    }
    
    for(int i = pos; i < proto.size(); i++)
          modulename << "" << proto.at(i);
    
}

int Scorpion_InvokeMain(){
    return Scorpion_InvokeMethod(svm_main);
}

int Scorpion_InvokeMethod(long ptrValue){
    if(!svmIsValidAddr(gSvm.env->getBlockTable(), METHOD_BLOCK, ptrValue)){
       stringstream ss;
       ss << "pointer to method: " << ptrValue << " is out of allocated method range.";
       Exception(ss.str(), "MethodNotFoundException");
    }
    
    if(includep){
    
        getMethodName(ptrValue);
        if(mthdname.str() == "" || classname.str() == "" || modulename.str() == "")
           return -1;
        
        stringstream s;
        s << modulename.str() << "." << mthdname.str();
        
        Exception::trace.addproto(s.str(), classname.str(), isnative);
    
    }
    else
      includep = true;
    
    long pc = gSvm.vm.vStaticRegs[VREG_PC];
    
    isnative = false;
    gSvm.vm.vStaticRegs[VREG_PC] = mthdptr;
    svmBlockToAddr(gSvm.env->getBlockTable(), METHOD_BLOCK, ptrValue, pc, proto);
    
     return 0;
}

void return_main() // this is simple lol
{
   Init_ShutdownScorpionVM();
}

void return_method(long addr){
    gSvm.vm.flags[VFLAG_MTHDC]--;
    gSvm.vm.vStaticRegs[VREG_PC] = svmBlockFromAddr(gSvm.env->getBlockTable(), METHOD_BLOCK, addr);
}

u4_d arguments; // our dedicated instruction arguments
long compare(long instruction){
     double a,b;
     a = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK,arguments.byte2);
     b = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK,arguments.byte3);
     
     if(instruction == OP_ISEQ)
        return (int) a == b;
     if(instruction == OP_ISGE)
        return (int) a >= b;
     if(instruction == OP_ISGT)
        return (int) a > b;
     if(instruction == OP_ISLE)
        return (int) a <= b;
     if(instruction == OP_ISLT)
        return (int) a < b;
     if(instruction == OP_ISNEQ)
        return (int) a != b;
     if(instruction == OP_ISNGE)
        return (int) !(a >= b);
     if(instruction == OP_ISNGT)
        return (int) !(a > b);
     if(instruction == OP_ISNLE)
        return (int) !(a <= b);
     if(instruction == OP_ISNLT)
        return (int) !(a < b);
     else
        return 0;
}

double math(long instruction){
     double a,b;
     a = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK,arguments.byte2);
     b = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK,arguments.byte3);
     
     if(instruction == OP_IADD)
        return (long) a + b;
     if(instruction == OP_ISUB)
        return (long) a - b;
     if(instruction == OP_IDIV)
        return (long) a / b;
     if(instruction == OP_IMULT)
        return (long) a * b;
     if(instruction == OP_SADD)
        return (int) a + b;
     if(instruction == OP_SSUB)
        return (int) a - b;
     if(instruction == OP_SDIV)
        return (int) a / b;
     if(instruction == OP_SMULT)
        return (int) a * b;
     if(instruction == OP_DADD)
        return (double) a + b;
     if(instruction == OP_DSUB)
        return (double) a - b;
     if(instruction == OP_DDIV)
        return (double) a / b;
     if(instruction == OP_DMULT)
        return (double) a * b;
     if(instruction == OP_FADD)
        return (float) a + b;
     if(instruction == OP_FSUB)
        return (float) a - b;
     if(instruction == OP_FDIV)
        return (float) a / b;
     if(instruction == OP_FMULT)
        return (float) a * b;
     if(instruction == OP_CADD)
        return (char) a + b;
     if(instruction == OP_CSUB)
        return (char) a - b;
     if(instruction == OP_CDIV)
        return (char) a / b;
     if(instruction == OP_CMULT)
        return (char) a * b;
     else
        return 0;
}

// TODO: Finish implementing the rest of the instructions
void Scorpion_VMExecute(){
  alog.ALOGD("running application.");
      
  long i, g; // our dedicated instruction and igroup
  exe:
    i = gSvm.appholder.getNextInstr();
    g = gSvm.appholder.getGroup();

    if(i == tok_eof){
      alog.ALOGV("fatal error: application attempting to close unexpectingly.");
      segfault();
    }
    else if(i == tok_floating){
      alog.ALOGV("fatal error: attempting to execute unknown instruction.");
      segfault();
    }

    // TODO: Check flags 
    arguments = gSvm.appholder.getAgs();
    // TODO: Debugger run

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
          case OP_NOP: break; // do nothing
          case OP_END: 
              if(gSvm.vm.flags[VFLAG_IFC] > 0 && !gSvm.vm.flags[VFLAG_IF_IGNORE])
                  gSvm.vm.flags[VFLAG_IFC]--;
              if(gSvm.vm.flags[VFLAG_IF_IGNORE] == true){
                 gSvm.vm.flags[VFLAG_IGNORE] = false;
                 gSvm.vm.flags[VFLAG_IF_IGNORE] = false;
              }
              
              // for method ending
              if(gSvm.vm.flags[VFLAG_IGNORE] == 1)
                  gSvm.vm.flags[VFLAG_IGNORE] == 0;
          break;
          case OP_HLT:
               Init_ShutdownScorpionVM();
       } // run each instr
       goto exe;
    group1:
       switch( i ) {
          case OP_RETURN:
               if(arguments.byte1 == 0 && gSvm.vm.flags[VFLAG_MTHDC] == 0)
                  return_main();
               else
                 return_method(arguments.byte1); // TODO: return method
          break;
          case OP_PUSH:
            {
               long stacksz = svmGetBlockAddr(gSvm.env->getBlockTable(), STACK_BLOCK);
      
               if(++gSvm.vm.vStaticRegs[VREG_SP] >= stacksz)
                 Exception("The stack was filled with too much data.", "StackOverfowlException");
               
               svmBlockToAddr(gSvm.env->getBlockTable(), STACK_BLOCK, gSvm.vm.vStaticRegs[VREG_SP], 
                      svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1), "");
            }
          break;
          case OP_POP:
               if((gSvm.vm.vStaticRegs[VREG_SP] - 1) < -1)
                   Exception("Failure to pull data from empty stack.", "StackUnderflowException");
                
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, 
                      svmBlockFromAddr(gSvm.env->getBlockTable(), STACK_BLOCK, gSvm.vm.vStaticRegs[VREG_SP]--),"");    
          break;
          case OP_JMP:
               gSvm.vm.vStaticRegs[VREG_PC] =  svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);
          break;
          case OP_CALL:
               {
                 gSvm.vm.flags[VFLAG_MTHDC]++;
                 long m = Scorpion_InvokeMethod(arguments.byte1);
                 if(m != 0)
                   Exception("Failure to invoke unknown method.", "MethodInvocationFailure");
               }
          break;
          case OP_MTHD: break; // this method does nothing, it was executed during vm init
          case OP_LBL: break;
       } // run each instr
       goto exe;
    group2:
       switch( i ) {
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
                  
                  svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, i, ""); 
               }
          break;
          case OP_JIT:
               if(arguments.byte2 == 1)
                  gSvm.vm.vStaticRegs[VREG_PC] =  svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);
          break;
          case OP_JIF:
               if(arguments.byte2 == 0)
                  gSvm.vm.vStaticRegs[VREG_PC] =  svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);
          break;
          case OP_ICONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (long) arguments.byte2, "");
          break;
          case OP_DCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (double) arguments.byte2, "");
          break;
          case OP_FCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (float) arguments.byte2, "");
          break;
          case OP_SCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (int) arguments.byte2, "");
          break;
          case OP_BCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (bool) arguments.byte2, "");
          break;
          case OP_CCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (char) arguments.byte2, "");
          break;
          case OP_RSHFT:
               {
                 svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, 
                      (((long) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, 
                      arguments.byte1)) >> (long) arguments.byte2), "");
               }
          break;
          case OP_LSHFT:
               {
                 svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, 
                      (((long) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, 
                      arguments.byte1)) << (long) arguments.byte2), "");
               }
          break;
          case OP_PTR:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, arguments.byte2, "");
          break;
       } // run each instr
       goto exe;
    group3:
       switch( i ) {
          default:
               if(i == OP_ISEQ || i == OP_ISNEQ || i == OP_ISLT || i == OP_ISNLT || i == OP_ISLE || i == OP_ISNLE
                  || i == OP_ISGT || i == OP_ISNGT || i == OP_ISGE || i == OP_ISNGE)
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, compare(i), "");
               else if(i == OP_IADD || i == OP_ISUB || i == OP_IMULT || i == OP_IDIV || i == OP_SADD
                  || i == OP_SSUB || i == OP_SMULT || i == OP_SDIV || i == OP_DADD
                  || i == OP_DSUB || i == OP_DMULT || i == OP_DDIV || i == OP_FADD
                  || i == OP_FSUB || i == OP_FMULT || i == OP_FDIV || i == OP_CADD
                  || i == OP_CSUB || i == OP_CMULT || i == OP_CDIV)
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, math(i), "");
          break;
       } // run each instr
       goto exe;
    group4:
       switch( i ) {
          case OP_STRCONST:
               {
                 string output =  gSvm.appholder.getStr();
                 svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, output.size(), "");
                 
                 long addr = arguments.byte1 + 1;
                 for(long i = 0; i < output.size(); i++)
                    svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, addr++, (int) output.at(i), "");
               }
          break;
          case OP_COUT: // output data to the console
            {
              string output =  gSvm.appholder.getStr();
              for(long i = 0; i < output.size(); i++){
                if(output.at(i) != '$'){
                  cout << output.at(i);
                  continue;
                }
                else {
                   if(!((i + 1) < output.size())){
                      cout << (char) 244;
                      break;
                    }
                    
                   
                   char bad_char=(131);  
                   i++;
                   switch ( output.at(i) ){
                       case '$':
                          cout << '$'; break;
                       case 'n':
                          cout << endl; break;
                       case 'b':
                          cout << '\b'; break;
                       case 't':
                          cout << '\t'; break;
                       case 'f':
                          cout << '\f'; break;
                       case 'r':
                          cout << '\r'; break;
                       case '[': // print variable data
                          {
                            i++;
                            if(!((i) < output.size())){
                              cout << bad_char;
                              break;
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
                                     break;
                                  }
                                  cout << "(" << ss.str() << ")";
                                  
                                  long addr = atoi(ss.str().c_str());
                                  if(form == 'c')
                                    cout << (char) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, addr);
                                  else if(form == 'b')
                                    cout << (bool) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, addr);
                                  else if(form == 'f')
                                    cout << (float) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, addr);
                                  else if(form == 'd')
                                    cout << (double) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, addr);
                                  else if(form == 'i')
                                    cout << (long) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, addr);
                                  else if(form == 's')
                                    cout << (int) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, addr);
                                  else //form == v
                                    cout << svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, addr);
                                  break;
                                }
                            }
                            break;
                          }
                       case '#':
                          cout << std::flush; break;
                       case '!': // TODO: change color
                          {
                            break;
                          }
                       default:
                          cout << bad_char; break;
                   }
                }
              }
            }
          break;
       } // run each instr
       goto exe;

}
