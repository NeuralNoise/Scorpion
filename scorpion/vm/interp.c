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

double math(long instruction){
     double a,b;
     a = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK,arguments.byte2);
     b = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK,arguments.byte3);
     
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

// TODO: Finish implementing the rest of the instructions
// TODO: Make the engine faster
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

    if(gSvm.vm.flags[VFLAG_NO] == 1 && i != OP_ENDNO) // do not run
        goto exe;
        
    if(gSvm.vm.flags[VFLAG_IF_IGNORE] == 1 && (i != OP_END || i != OP_IF)) // do not run
        goto exe; 
        
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
               if(arguments.byte1 == 0 && gSvm.vm.flags[VFLAG_MTHDC] == 0)
                  return_main();
               else
                 return_method(arguments.byte1);
          goto exe;
          case OP_INC:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, 
                      svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1) + 1, "");
          goto exe;
          case OP_DEC:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, 
                      svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1) - 1, "");
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
               
               if(!gSvm.vm.flags[VFLAG_IF_IGNORE] && !svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1)){
                   gSvm.vm.flags[VFLAG_IF_IGNORE] = 1;
                   gSvm.vm.flags[VFLAG_IGNORE] = 1;
               }
          goto exe;
          case OP_PUSH:
            {
               long stacksz = svmGetBlockAddr(gSvm.env->getBlockTable(), STACK_BLOCK);
      
               if(++gSvm.vm.vStaticRegs[VREG_SP] >= stacksz)
                 Exception("The stack was filled with too much data.", "StackOverfowlException");
               
               svmBlockToAddr(gSvm.env->getBlockTable(), STACK_BLOCK, gSvm.vm.vStaticRegs[VREG_SP], 
                      svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1), "");
            }
          goto exe;
          case OP_POP:
               if((gSvm.vm.vStaticRegs[VREG_SP] - 1) < -1)
                   Exception("Failure to pull data from empty stack.", "StackUnderflowException");
                
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, 
                      svmBlockFromAddr(gSvm.env->getBlockTable(), STACK_BLOCK, gSvm.vm.vStaticRegs[VREG_SP]--),"");    
          goto exe;
          case OP_JMP:
               gSvm.vm.vStaticRegs[VREG_PC] =  svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);
          goto exe;
          case OP_CALL:
               {
                 gSvm.vm.flags[VFLAG_MTHDC]++;
                 long m = Scorpion_InvokeMethod(arguments.byte1);
                 if(m != 0)
                   Exception("Failure to invoke unknown method.", "MethodInvocationFailure");
               }
          goto exe;
          case OP_SLP:
                {
                    long time = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);
                    sleep(time);
                }
          goto exe;
          case OP_USLP:
               {
                   long time = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);
                   usleep(time);
               }
          goto exe;
          case OP_MTHD: goto exe; // this instruction does nothing, it was executed during vm init
          case OP_LBL: goto exe;  // this instruction does nothing, it was executed during vm init
       } // run each instr
       goto exe;
    group2:
       switch( i ) {
          case OP_ICONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (long) arguments.byte2, "");
          goto exe;
          case OP_DCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (double) arguments.byte2, "");
          goto exe;
          case OP_FCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (float) arguments.byte2, "");
          goto exe;
          case OP_SCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (int) arguments.byte2, "");
          goto exe;
          case OP_BCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (bool) arguments.byte2, "");
          goto exe;
          case OP_CCONST:
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, (char) arguments.byte2, "");
          goto exe;
          case OP_JIT:
               if(arguments.byte2 == 1)
                  gSvm.vm.vStaticRegs[VREG_PC] =  svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);
          goto exe;
          case OP_JIF:
               if(arguments.byte2 == 0)
                  gSvm.vm.vStaticRegs[VREG_PC] =  svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);
          goto exe;
          case OP_RSHFT:
               {
                 svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, 
                      (((long) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, 
                      arguments.byte1)) >> (long) arguments.byte2), "");
               }
          goto exe;
          case OP_LSHFT:
               {
                 svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, 
                      (((long) svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, 
                      arguments.byte1)) << (long) arguments.byte2), "");
               }
          goto exe;
          case OP_THROW:
               {
                   stringstream clause, message;
                   long clength = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1);
                   long mlength = svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte2);
                   long clause_address = arguments.byte1 + 1;
                   long message_address = arguments.byte1 + 1;
                   
                   for(long i = 0; i < clength; i++)
                       clause << (char)  svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, clause_address++);
                   
                   for(long i = 0; i < mlength; i++)
                       message << (char)  svmBlockFromAddr(gSvm.env->getBlockTable(), DATA_BLOCK, message_address++);
                       
                   // TODO: Implement $ string processing in Exception()       
                   Exception(message.str(), clause.str());
               }
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
                  
                  svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, i, ""); 
               }
          goto exe;
       } // run each instr
       goto exe;
    group3:
       switch( i ) {
          default:
               if(i == OP_ISEQ || i == OP_ISNEQ || i == OP_ISLT || i == OP_ISNLT || i == OP_ISLE || i == OP_ISNLE
                  || i == OP_ISGT || i == OP_ISNGT || i == OP_ISGE || i == OP_ISNGE || i == OP_OR || i == OP_AND)
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, compare(i), "");
               else if(i == OP_IADD || i == OP_ISUB || i == OP_IMULT || i == OP_IDIV || i == OP_SADD
                  || i == OP_SSUB || i == OP_SMULT || i == OP_SDIV || i == OP_DADD
                  || i == OP_DSUB || i == OP_DMULT || i == OP_DDIV || i == OP_FADD
                  || i == OP_FSUB || i == OP_FMULT || i == OP_FDIV || i == OP_CADD
                  || i == OP_CSUB || i == OP_CMULT || i == OP_CDIV || i == OP_IMOD 
                  || i == OP_SMOD || i == OP_CMOD)
               svmBlockToAddr(gSvm.env->getBlockTable(), DATA_BLOCK, arguments.byte1, math(i), "");
          goto exe;
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
          goto exe;
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
                      goto exe;
                    }
                    
                   
                   char bad_char=(131);  
                   i++;
                   switch ( output.at(i) ){
                       case '$':
                          cout << '$'; goto exe;
                       case 'n':
                          cout << endl; goto exe;
                       case 'b':
                          cout << '\b'; goto exe;
                       case 't':
                          cout << '\t'; goto exe;
                       case 'f':
                          cout << '\f'; goto exe;
                       case 'r':
                          cout << '\r'; goto exe;
                       case '[': // print variable data example:  $[v483|
                          {
                            i++;
                            if(!((i) < output.size())){
                              cout << bad_char;
                              goto exe;
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
                                     goto exe;
                                  }
                                  
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
                                  goto exe;
                                }
                            }
                            break;
                          }
                       case '#':
                          cout << std::flush; goto exe;
                       case '!': // TODO: change color
                          {
                            goto exe;
                          }
                       default:
                          cout << bad_char; goto exe;
                   }
                }
              }
            }
          goto exe;
       } // run each instr
       goto exe;

}
