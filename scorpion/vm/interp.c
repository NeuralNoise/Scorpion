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
        
        stringstream s;
        s << gSvm.mtds[ptrValue].module << "." << gSvm.mtds[ptrValue].name;

        Exception::trace.addproto(s.str(), gSvm.mtds[ptrValue].name, gSvm.mtds[ptrValue].native);
    }
    else
      includep = true;
    
    gSvm.mtds[ptrValue].ref.byte1 = gSvm.vm.vStaticRegs[VREG_PC];
    gSvm.vm.vStaticRegs[VREG_PC] = jmpLocation(gSvm.mtds[ptrValue]);
    
     return 0;
}

void return_main() // this is simple lol
{
   Init_ShutdownScorpionVM();
}
 
void return_method(long ptrValue){
    if(ptrValue >= gSvm.methodc){
       stringstream ss;
       ss << "pointer to method: " << ptrValue << " is out of allocated method range.";
       Exception(ss.str(), "MethodNotFoundException");
    }
    
    gSvm.vm.vStaticRegs[VREG_PC] = returnLocation(gSvm.mtds[ptrValue]);
}

int instrSize(int i, unsigned long k)
{
    if(i == OP_NOP || i == OP_END || i == OP_HLT || i == OP_NO || i == OP_ENDNO)
         return 0;
     else if(i == OP_RETURN || i == OP_PUSH || i == OP_POP || i == OP_JMP || i == OP_CALL
       || i == OP_MTHD || i == OP_LBL || i == OP_IF || i == OP_INC || i == OP_DEC
       || i == OP_KILL || i == OP_DELETE || i == OP_DELETE_ARRY) // push 7 or push *x
         return 1;
     else if(i == OP_ICONST || i == OP_DCONST || i == OP_FCONST || 
        i == OP_SCONST || i == OP_BCONST || i == OP_CCONST || i == OP_RSHFT 
        || i == OP_LSHFT || i == OP_CIN || i == OP_STR_APND || i == OP_ASSN 
        || i == OP_JIF || i == OP_JIT || i == OP_THROW
        || i == OP_STR_ACONST || i == OP_CAST || i == OP_BYTE_CONST || i == OP_LCONST
        || i == OP_DACONST || i == OP_IACONST || i == OP_FACONST || i == OP_CACONST 
        || i == OP_BACONST || i == OP_BYTE_ACONST || i == OP_SACONST || i == OP_LACONST) // mthd @9
         return 2;
     else if(i == OP_ISEQ || i == OP_ISNEQ || i == OP_ISLT || i == OP_ISNLT || i == OP_ISLE || i == OP_ISNLE
       || i == OP_ISGT || i == OP_ISNGT || i == OP_ISGE || i == OP_ISNGE || i == OP_IADD
       || i == OP_ISUB || i == OP_IMULT || i == OP_IDIV || i == OP_SADD
       || i == OP_SSUB || i == OP_SMULT || i == OP_SDIV || i == OP_DADD
       || i == OP_DSUB || i == OP_DMULT || i == OP_DDIV || i == OP_FADD
       || i == OP_FSUB || i == OP_FMULT || i == OP_FDIV || i == OP_CADD
       || i == OP_CSUB || i == OP_CMULT || i == OP_CDIV || i == OP_IMOD
       || i == OP_CMOD || i == OP_SMOD || i == OP_OR || i == OP_AND
       || i == OP_AT || i == OP_ALOAD || i == OP_ASTORE) //
         return 3;
     else if(i == OP_COUT)
         return gSvm.bytestream.valueAt(k++);
     else if(i == OP_STRCONST){ // string 13 'Hello, World!'
         k++;
         return gSvm.bytestream.valueAt(k++);
     }
}

#define GETARG_SIZE(i,k)\
         instrSize(i,k);

u4_d arguments; // our dedicated instruction arguments
long compare(long instruction){
     double a,b;
     a = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
     b = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]);
     
     if(instruction == OP_ISLT)
        return a < b;
     if(instruction == OP_ISEQ)
        return a == b;
     if(instruction == OP_ISGE)
        return a >= b;
     if(instruction == OP_ISGT)
        return a > b;
     if(instruction == OP_ISLE)
        return a <= b;
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
        return ((bool) a || (bool) b);
     if(instruction == OP_AND)
        return ((bool) a && (bool) b);
     else
        return 0;
}

int _getch_() {
    int ch;
    struct termios t_old, t_new;

    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

    ch = getchar();
    cout << "\b \b" << std::flush;
    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}

void printf_obj_content(long addr, char form)
{
   if(isgenericarray(gSvm.env->getBitmap().objs[addr].instanceData.byte1))
   {
     printf("%#x", (unsigned int) addr);
     return;
   }
  
  if(form == 'i')
    cout << (sdouble) ((sint) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]));
  else if(form == 's')
    cout << (sdouble) ((sshort) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]));
  else if(form == 'S')
    _cout_(getstr(gSvm.env->getBitmap().objs[addr]));
  else if(form == 'B')
    cout << (sdouble) ((sbyte) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]));
  else if(form == 'l')
    cout << (sdouble) ((slong) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]));
  else if(form == 'p')
    printf("%#x", (unsigned int) addr);
  else if(form == 'c' || gSvm.env->getBitmap().objs[addr].instanceData.byte1 == TYPEDEF_GENERIC_CHAR)
    cout << (schar) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 'b' || gSvm.env->getBitmap().objs[addr].instanceData.byte1 == TYPEDEF_GENERIC_BOOL)
    cout << (((sbool) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]) == 1) ? "true" : "false");
  else if(form == 'f' || gSvm.env->getBitmap().objs[addr].instanceData.byte1 == TYPEDEF_GENERIC_FLOAT)
    cout << std::setprecision((g_max_precision/2) - 1) << (sfloat) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 'd' || gSvm.env->getBitmap().objs[addr].instanceData.byte1 == TYPEDEF_GENERIC_DOUBLE)
    cout << std::setprecision(g_max_precision) << (double) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else
    cout << (double) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
}

double math(long instruction){
     double a,b;
     a = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
     b = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]);
     
     if(instruction == OP_IADD)
        return (sint) (a + b);
     if(instruction == OP_ISUB)
        return (sint) (a - b);
     if(instruction == OP_IDIV)
        return (sint) (a / b);
     if(instruction == OP_IMULT)
        return (sint) (a * b);
     if(instruction == OP_SADD)
        return (sshort) (a + b);
     if(instruction == OP_SSUB)
        return (sint) (a - b);
     if(instruction == OP_SDIV)
        return (sshort) (a / b);
     if(instruction == OP_SMULT)
        return (sshort) (a * b);
     if(instruction == OP_DADD)
        return (sdouble) (a + b);
     if(instruction == OP_DSUB)
        return (sdouble) (a - b);
     if(instruction == OP_DDIV)
        return (sdouble) (a / b);
     if(instruction == OP_DMULT)
        return (sdouble) (a * b);
     if(instruction == OP_FADD)
        return (sfloat) (a + b);
     if(instruction == OP_FSUB)
        return (sfloat) (a - b);
     if(instruction == OP_FDIV)
        return (sfloat) (a / b);
     if(instruction == OP_FMULT)
        return (sfloat) (a * b);
     if(instruction == OP_CADD)
        return (schar) (a + b);
     if(instruction == OP_CSUB)
        return (schar) (a - b);
     if(instruction == OP_CDIV)
        return (schar) (a / b);
     if(instruction == OP_CMULT)
        return (schar) (a * b);
     if(instruction == OP_IMOD)
        return (sint) a % (long) b;
     if(instruction == OP_SMOD)
        return (sshort) a % (int) b;
     if(instruction == OP_CMOD)
        return (schar) a % (char) b;
     else
        return 21303029493;
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
                         continue;
                      }
                      
                      long addr = atoi(ss.str().c_str());
                      printf_obj_content(addr, form);
                      continue;
                    }
                }
                continue;
              }
           case '#':
              cout << std::flush; continue;
           case '!': // TODO: change color
              {
                continue;
              }
           default:
              cout << bad_char; continue;
       }
    }
  }
}

string getStr(unsigned long &k, unsigned long m, long len)
{
     char c;
     stringstream ss;
     for(int i = 0; i < len; i++){
         c = (char) gSvm.bytestream.valueAt(k++);
         ss << "" << c;
     }
     return ss.str();
}

void Protect(long k, long m)
{
    if(k>=m)
      alog.ALOGV("Attempting to close program unexpectingly.");
    else if(k>sMaxOpcodeLimit || k<0)
      alog.ALOGV("Attempting to execute invalid instruction.");
    
    segfault();
}


// TODO: Finish implementing the rest of the instructions
// TODO: Have interpreter utilize Scorpion primitive types during data transfer
// TODO: implement GC check in instruction checkGC(gSvm.env->bitmap);      
void Scorpion_VMExecute(){
  alog.ALOGD("running application.");
  
  unsigned long i, k=gSvm.vm.vStaticRegs[VREG_PC], m=gSvm.bytestream.size();
  exe:
    if(k>=m||k>sMaxOpcodeLimit||k<0)
      Protect(k,m);
      
    i = gSvm.bytestream.valueAt(k++);
  //  cout << "i=" << i << endl;
    if((gSvm.vm.flags[VFLAG_NO] == 1 && i != OP_ENDNO) || 
       (gSvm.vm.flags[VFLAG_IF_IGNORE] == 1 && !(i == OP_END || i == OP_IF))){ // do not run
        k+=GETARG_SIZE(i,k);
        goto exe;
    }
        
    // TODO: Debugger run
   // if(gSvm.Debug)
      // do debug stuff

       switch( i ) {
          case OP_NOP: goto exe; // do nothing
          case OP_END: 
              if(gSvm.vm.flags[VFLAG_IFC] > 0)
                  gSvm.vm.flags[VFLAG_IFC]--;
              
              if(gSvm.vm.flags[VFLAG_IF_IGNORE] == 1 && (gSvm.vm.flags[VFLAG_IFC] == gSvm.vm.flags[VFLAG_IF_DEPTH])){
                 gSvm.vm.flags[VFLAG_IF_IGNORE] = 0;
                 gSvm.vm.flags[VFLAG_IF_DEPTH] = 0;
              }
                 
          goto exe;
          case OP_NO:
               gSvm.vm.flags[VFLAG_NO] = 1; goto exe;
          case OP_ENDNO:
               gSvm.vm.flags[VFLAG_NO] = 0; goto exe;
          case OP_HLT:
               Init_ShutdownScorpionVM();
       }
       op_ags.byte1=gSvm.bytestream.valueAt(k++);
    
       switch( i ) {
          case OP_RETURN:
               --gSvm.vm.flags[VFLAG_MTHDC];
               if(((long) op_ags.byte1 == 0) && (gSvm.vm.flags[VFLAG_MTHDC] <= 0))
                  return_main();
               else
                 return_method((long) op_ags.byte1);
          goto exe;
          case OP_COUT: // output data to the console;
              _cout_(getStr(k,m,op_ags.byte1));
          goto exe;
          case OP_INC:
               svmIncGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
          goto exe;
          case OP_DEC:
               svmDecGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
          goto exe;
          case OP_IF:
               
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
                if((gSvm.vm.flags[VFLAG_IF_IGNORE] == 0) && 
                 (svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]) == 0))
                       gSvm.vm.flags[VFLAG_IF_IGNORE] = 1;
                       
               if(gSvm.vm.flags[VFLAG_IF_IGNORE] == 1)
                  gSvm.vm.flags[VFLAG_IF_DEPTH] = gSvm.vm.flags[VFLAG_IFC];
                  
               gSvm.vm.flags[VFLAG_IFC]++;
               
          goto exe;
          case OP_PUSH:
            {
               long stacksz = gSvm.env->getBitmap().stack->length;
      
               if(++gSvm.vm.vStaticRegs[VREG_SP] >= stacksz) 
                 Exception("The stack has overflowed with too much data.", "StackOverfowlException");
            //   cout << "push @" << gSvm.vm.vStaticRegs[VREG_SP] << " ->" << (long) op_ags.byte1 << endl;
               gSvm.env->getBitmap().stack->plong[gSvm.vm.vStaticRegs[VREG_SP]] = (long) op_ags.byte1;
            }
          goto exe;
          case OP_POP:
               if(gSvm.vm.vStaticRegs[VREG_SP] < 0)
                   Exception("Failure to pull data from empty stack.", "StackUnderflowException");
                
               svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], 
                      gSvm.env->getBitmap().stack->plong[gSvm.vm.vStaticRegs[VREG_SP]--]);
          goto exe;
          case OP_KILL:
               {
                   if(!svmObjectIsAlive(gSvm.env->getBitmap().objs[(long) op_ags.byte1]))
                     goto exe;
                     
                   svmDumpObject(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
                   checkGC(gSvm.env->bitmap);
               }
          goto exe;
          case OP_JMP:
               k =  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
          goto exe;
          case OP_CALL:
               {
                 gSvm.vm.flags[VFLAG_MTHDC]++;
                 Scorpion_InvokeMethod((long) op_ags.byte1);
               }
          goto exe;
          case OP_MTHD: goto exe; // this instruction does nothing, it was executed during vm init
          case OP_LBL: goto exe;  // this instruction does nothing, it was executed during vm init
          default:
             if(i == OP_DELETE || i == OP_DELETE_ARRY){
                 if(!svmObjectIsAlive(gSvm.env->getBitmap().objs[(long) op_ags.byte1]))
                         goto exe;
                         
                   freeObj(gSvm.env->getBitmap().objs[(long) op_ags.byte1]); // arrays and generic objects can be freed the same way
                   goto exe;
             }
          break;
       } 
       op_ags.byte2=gSvm.bytestream.valueAt(k++);
       
       switch( i ) {
          case OP_ICONST:
               {
                   // cout << "iconst @" << (long) op_ags.byte1 << " ->" << (long) op_ags.byte2 << endl;
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->bitmap.objs[(long) op_ags.byte1], TYPEDEF_GENERIC_INT, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], (sint) op_ags.byte2);
                   //  cout << "i is " << k << endl;
               }
          goto exe;
          case OP_STRCONST:
               {
                 u1 sz;
                 sz.byte1 = 1;
                 SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_STRING, sz);
                 assign(gSvm.env->getBitmap().objs[(long) op_ags.byte1], getStr(k,m,op_ags.byte2));
               }
          goto exe;
          case OP_DCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->bitmap.objs[(long) op_ags.byte1], TYPEDEF_GENERIC_DOUBLE, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], op_ags.byte2);
               }
          goto exe;
          case OP_FCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_FLOAT, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], (sfloat) op_ags.byte2);
               }
          goto exe;
          case OP_SCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_SHORT, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], (sshort) op_ags.byte2);
               }
          goto exe;
          case OP_BCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_BOOL, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], op_ags.byte2);
               }
          goto exe;
          case OP_CCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_CHAR, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], (schar) op_ags.byte2);
               }
          goto exe;
          case OP_BYTE_CONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_BYTE, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], (sbyte) op_ags.byte2);
               }
          goto exe;
          case OP_LCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_LONG, sz);
                    svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], (slong) op_ags.byte2);
               }
          goto exe;
          case OP_IACONST:
               {
                u1 sz;
                sz.byte1 = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_INT_ARRAY, sz);
               }
          goto exe;
          case OP_SACONST:
               {
                u1 sz;
                sz.byte1 = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_SHORT_ARRAY, sz);
               }
          goto exe;
          case OP_BYTE_ACONST:
               {
                u1 sz;
                sz.byte1 = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_BYTE_ARRAY, sz);
               }
          goto exe;
          case OP_BACONST:
               {
                u1 sz;
                sz.byte1 = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_BOOL_ARRAY, sz);
               }
          goto exe;
          case OP_FACONST:
               {
                u1 sz;
                sz.byte1 = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_FLOAT_ARRAY, sz);
               }
          goto exe;
          case OP_DACONST:
               {
                u1 sz;
                sz.byte1 = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_DOUBLE_ARRAY, sz);
               }
          goto exe;
          case OP_LACONST:
               {
                u1 sz;
                sz.byte1 = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_LONG_ARRAY, sz);
               }
          goto exe;
          case OP_STR_ACONST:
               {
                u1 sz;
                sz.byte1 = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_STRING_ARRAY, sz);
               }
          goto exe;
          case OP_CAST:
               {
                   sdouble newData, data = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
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
                   
                   svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2], newData);
               }
          goto exe;
          case OP_JIT:
               if((bool) svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]) == 1)
                  k =  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
          goto exe;
          case OP_JIF:
               if((bool) svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]) == 0)
                  k =  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
          goto exe;
          case OP_RSHFT:
               {
                 long num = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
                 num >>= (long) svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                 svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], num);
               }
          goto exe;
          case OP_LSHFT:
               {
                 long num = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
                 num <<= (long) svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                 svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], num);
               }
          goto exe;
          case OP_THROW: 
                   Exception(getstr(gSvm.env->getBitmap().objs[(long) op_ags.byte1]), 
                             getstr(gSvm.env->getBitmap().objs[(long) op_ags.byte2]));
          goto exe;
          case OP_CIN:
               {
                  int i;
                  if(op_ags.byte2 == 0){ // do not print char to screen
                    i = _getch_();
                  }
                  else
                    i = _getch_();
                  
                  svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], i);
               }
          goto exe;
          case OP_STR_APND:
                 concat(gSvm.env->getBitmap().objs[(long) op_ags.byte1], 
                        getstr(gSvm.env->getBitmap().objs[(long) op_ags.byte2]));
          goto exe;
          case OP_ASSN:
                 gSvm.env->getBitmap().objs[(long) op_ags.byte1] = gSvm.env->getBitmap().objs[(long) op_ags.byte2];
          goto exe;
       }
       op_ags.byte3=gSvm.bytestream.valueAt(k++);
    
       switch( i ) {
          case OP_AT:
               {
                   svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], 
                          at(gSvm.env->getBitmap().objs[(long) op_ags.byte2], 
                             svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3])));
               }
          goto exe;
          case OP_ALOAD:
               {
                   long pos = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]);
                   if(__typedef(gSvm.env->getBitmap().objs[(long) op_ags.byte2]) == TYPEDEF_GENERIC_INT_ARRAY){
                       svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], 
                           get(gSvm.env->getBitmap().objs[(long) op_ags.byte2], pos));
                   }
                   else if(__typedef(gSvm.env->getBitmap().objs[(long) op_ags.byte2]) == TYPEDEF_STRING_ARRAY){
                       str_location = pos;
                       
                       assign(gSvm.env->getBitmap().objs[(long) op_ags.byte1], 
                            getstr(gSvm.env->getBitmap().objs[(long) op_ags.byte2]));
                   }
                   else
                     Exception("Object is not an array type.", "IncompatibleTypeException");
               }
          goto exe;
          case OP_ASTORE:
               {
                   long pos = svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]);
                   if(isgenericarray(__typedef(gSvm.env->getBitmap().objs[(long) op_ags.byte1]))){
                       set(gSvm.env->getBitmap().objs[(long) op_ags.byte1], pos,
                           svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]));
                   }
                   else if(__typedef(gSvm.env->getBitmap().objs[(long) op_ags.byte1]) == TYPEDEF_STRING_ARRAY){
                       str_location = pos;
                       
                       string data = getstr(gSvm.env->getBitmap().objs[(long) op_ags.byte2]);
                       assign(gSvm.env->getBitmap().objs[(long) op_ags.byte1], data);
                   }
                   else
                     Exception("Object is not an array type.", "IncompatibleTypeException");
               }
          goto exe;
          case OP_ISLT:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])<svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3])); goto exe;
          break;
          default:
               if(i == OP_ISEQ || i == OP_ISNEQ || i == OP_ISLT || i == OP_ISNLT || i == OP_ISLE || i == OP_ISNLE
                  || i == OP_ISGT || i == OP_ISNGT || i == OP_ISGE || i == OP_ISNGE || i == OP_OR || i == OP_AND)
                  {
                     svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], compare(i)); goto exe;
                  }
               else if(i == OP_IADD || i == OP_ISUB || i == OP_IMULT || i == OP_IDIV || i == OP_SADD
                  || i == OP_SSUB || i == OP_SMULT || i == OP_SDIV || i == OP_DADD
                  || i == OP_DSUB || i == OP_DMULT || i == OP_DDIV || i == OP_FADD
                  || i == OP_FSUB || i == OP_FMULT || i == OP_FDIV || i == OP_CADD
                  || i == OP_CSUB || i == OP_CMULT || i == OP_CDIV || i == OP_IMOD 
                  || i == OP_SMOD || i == OP_CMOD){
                     svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1], math(i)); goto exe;
                  }
          break;
       }
       goto exe;

}
