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
    return Scorpion_InvokeMethod(svm_main, vm);
}

int Scorpion_InvokeMethod(long ptrValue, ScorpionVmState* vm){
    
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

int instrSize(ScorpionVmState* vm)
{
    unsigned long i = vm->k;
    if(i == OP_NOP || i == OP_END || i == OP_HLT || i == OP_NO || i == OP_ENDNO)
         return 0;
     else if(i == OP_RETURN || i == OP_PUSH || i == OP_POP || i == OP_JMP || i == OP_CALL
       || i == OP_MTHD || i == OP_LBL || i == OP_IF || i == OP_INC || i == OP_DEC
       || i == OP_KILL || i == OP_DELETE) // push 7 or push *x
         return 1;
     else if(i == OP_ICONST || i == OP_DCONST || i == OP_FCONST || 
        i == OP_SCONST || i == OP_BCONST || i == OP_CCONST || i == OP_RSHFT 
        || i == OP_LSHFT || i == OP_CIN || i == OP_STR_APND || i == OP_ASSN 
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

#define GETARG_SIZE(vmstate)\
         instrSize(vmstate);

u4_d arguments; // our dedicated instruction arguments
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
    cout << (char) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 'b' || gSvm.env->getBitmap().objs[addr].instanceData.byte1 == TYPEDEF_GENERIC_BOOL)
    cout << (((sbool) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]) == 1) ? "true" : "false");
  else if(form == 'f' || gSvm.env->getBitmap().objs[addr].instanceData.byte1 == TYPEDEF_GENERIC_FLOAT)
    cout << std::setprecision((g_max_precision/2) - 1) << (sfloat) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else if(form == 'd' || gSvm.env->getBitmap().objs[addr].instanceData.byte1 == TYPEDEF_GENERIC_DOUBLE)
    cout << std::setprecision(g_max_precision) << (double) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
  else
    cout << (double) svmGetGenericValue(gSvm.env->getBitmap().objs[addr]);
}

void _cout_(string output)
{
   // cout << "outputting " << output << endl;
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
                      break;
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

string getStr(ScorpionVmState* vm, long len)
{
     char c;
     stringstream ss;
     for(int i = 0; i < len; i++){
         c = (char) vm->bytestream.valueAt(vm->k++);
         ss << "" << c;
     }
     return ss.str();
}

void Protect(ScorpionVmState* vm)
{
    if(vm->k>=vm->m)
      alog.ALOGV("Attempting to close program unexpectingly.");
    else if(vm->k>sMaxOpcodeLimit || vm->k<0)
    {
        stringstream ss;
        ss << "Attempting to execute invalid instruction (" << vm->k << ").";
        alog.ALOGV(ss.str());
    }
    
    segfault();
}


// TODO: Finish implementing the rest of the instructions
// TODO: Have interpreter utilize Scorpion primitive types during data transfer
// TODO: implement GC check in instruction checkGC(gSvm.env->bitmap);      
void Scorpion_VMExecute(ScorpionVmState* vmstate){
  alog.ALOGD("running application.");
  vmstate->methodcount = 0;
    
  exe:
    if(vmstate->k>=vmstate->m||vmstate->k<0)
      Protect(vmstate);
      
    vmstate->i = vmstate->bytestream.valueAt(vmstate->k++);
    
    if(vmstate->i>sMaxOpcodeLimit) Protect(vmstate);
   // cout << "i=" << vmstate->i << endl;
    if((vmstate->status == vm_status_no_run && vmstate->i != OP_ENDNO) || 
       (vmstate->status == vm_status_if_ignore && !(vmstate->i == OP_END || vmstate->i == OP_IF))){ // do not run
        vmstate->k+=GETARG_SIZE(vmstate);
        goto exe;
    }
        
    // TODO: Debugger run
   // if(gSvm.Debug)
      // do debug stuff

       switch( vmstate->i ) {
          case OP_NOP: goto exe; // do nothing
          case OP_END: 
              if(vmstate->ifcount > 0)
                  vmstate->ifcount--;
              
              if(vmstate->status == vm_status_if_ignore && (vmstate->ifcount == vmstate->ifdepth)){
                 vmstate->status = vm_status_normal;
                 vmstate->ifdepth = 0;
              }
                 
          goto exe;
          case OP_NO:
               vmstate->status = vm_status_no_run; goto exe;
          case OP_ENDNO:
               vmstate->status = vm_status_normal; goto exe;
          case OP_HLT:
               gSvm.vmstate = vmstate;
               Init_ShutdownScorpionVM();
               goto exe;
       }
       op_ags.byte1=vmstate->bytestream.valueAt(vmstate->k++);
    
       switch( vmstate->i ) {
          case OP_RETURN:
               --vmstate->methodcount;
               if(((long) op_ags.byte1 == 0) && (vmstate->methodcount <= 0))
               {
                  gSvm.vmstate = vmstate;
                  return_main();
               }
               else
                 return_method((long) op_ags.byte1, vmstate);
          goto exe;
          case OP_COUT: // output data to the console;
              _cout_(getStr(vmstate, op_ags.byte1));
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
                if((vmstate->status == vm_status_normal) && 
                 (svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]) == 0))
                       vmstate->status = vm_status_if_ignore;
                       
               if(vmstate->status == vm_status_if_ignore)
                  vmstate->ifdepth = vmstate->ifcount;
                  
               vmstate->ifcount++;
               
          goto exe;
          case OP_PUSH:
            {
               long stacksz = gSvm.env->getBitmap().stack->length;
      
               if(++vmstate->sp >= stacksz) 
                 Exception("The stack has overflowed with too much data.", "StackOverfowlException");
               //cout << "push @" << vmstate->sp << " ->" << (long) op_ags.byte1 << endl;
               gSvm.env->getBitmap().stack->plong[vmstate->sp] = (long) op_ags.byte1;
            }
          goto exe;
          case OP_POP:
               if(vmstate->sp < 0)
                   Exception("Failure to pull data from empty stack.", "StackUnderflowException");
                
               gSvm.env->getBitmap().objs[(long) op_ags.byte1] =
                      gSvm.env->getBitmap().objs[gSvm.env->getBitmap().stack->plong[vmstate->sp--]];
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
               vmstate->k =  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
          goto exe;
          case OP_CALL:
               {
                 vmstate->methodcount++;
                 Scorpion_InvokeMethod((long) op_ags.byte1, vmstate);
               }
          goto exe;
          case OP_MTHD: goto exe; // this instruction does nothing, it was executed during vm init
          case OP_LBL: goto exe;  // this instruction does nothing, it was executed during vm 
          case OP_DELETE:
               if(!svmObjectIsAlive(gSvm.env->getBitmap().objs[(long) op_ags.byte1]))
                         goto exe;
                         
                 freeObj(gSvm.env->getBitmap().objs[(long) op_ags.byte1]); // arrays and generic objects can be freed the same way
          goto exe;
       } 
       op_ags.byte2=vmstate->bytestream.valueAt(vmstate->k++);
       
       switch( vmstate->i ) {
          case OP_ICONST:
               {
                    //cout << "iconst @" << (long) op_ags.byte1 << " ->" << (long) op_ags.byte2 << endl;
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->bitmap.objs[(long) op_ags.byte1], TYPEDEF_GENERIC_INT, sz);
                    gSvm.env->getBitmap().objs[(long) op_ags.byte1].obj->pint[default_loc] = (sint) op_ags.byte2;
                     //cout << "i is " << vmstate->k << endl;
               }
          goto exe;
          case OP_STRCONST:
               {
                 u1 sz;
                 sz.byte1 = 1;
                 SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_STRING, sz);
                 assign(gSvm.env->getBitmap().objs[(long) op_ags.byte1], getStr(vmstate,op_ags.byte2));
               }
          goto exe;
          case OP_DCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->bitmap.objs[(long) op_ags.byte1], TYPEDEF_GENERIC_DOUBLE, sz);
                    gSvm.env->getBitmap().objs[(long) op_ags.byte1].obj->pdouble[default_loc] = (sdouble) op_ags.byte2;
               }
          goto exe;
          case OP_FCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_FLOAT, sz);
                    gSvm.env->getBitmap().objs[(long) op_ags.byte1].obj->pfloat[default_loc] = (sfloat) op_ags.byte2;
               }
          goto exe;
          case OP_SCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_SHORT, sz);
                    gSvm.env->getBitmap().objs[(long) op_ags.byte1].obj->pshort[default_loc] = (sshort) op_ags.byte2;
               }
          goto exe;
          case OP_BCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_BOOL, sz);
                    gSvm.env->getBitmap().objs[(long) op_ags.byte1].obj->pboolean[default_loc] = (sbool) op_ags.byte2;
               }
          goto exe;
          case OP_CCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_CHAR, sz);
                    gSvm.env->getBitmap().objs[(long) op_ags.byte1].obj->pchar[default_loc] = (schar) op_ags.byte2;
               }
          goto exe;
          case OP_BYTE_CONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_BYTE, sz);
                    gSvm.env->getBitmap().objs[(long) op_ags.byte1].obj->pbyte[default_loc] = (sbyte) op_ags.byte2;
               }
          goto exe;
          case OP_LCONST:
               {
                    u1 sz;
                    sz.byte1 = 1;
                    SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) op_ags.byte1], TYPEDEF_GENERIC_LONG, sz);
                    gSvm.env->getBitmap().objs[(long) op_ags.byte1].obj->plong[default_loc] = (slong) op_ags.byte2;
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
                  vmstate->k =  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
          goto exe;
          case OP_JIF:
               if((bool) svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2]) == 0)
                  vmstate->k =  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1]);
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
       op_ags.byte3=vmstate->bytestream.valueAt(vmstate->k++);
    
       switch( vmstate->i ) {
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
                  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])<svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]));
          goto exe;
          case OP_ISGT:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])>svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]));
          goto exe;
          case OP_ISLE:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])<=svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]));
          goto exe;
          case OP_ISGE:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])>=svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]));
          goto exe;
          case OP_OR:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  (bool) ((bool)svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])||(bool)svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3])));
          goto exe;
          case OP_AND:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  (bool) ((bool)svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])&&(bool)svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3])));
          goto exe;
          case OP_ADD:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])+svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]));
          goto exe;
          case OP_MULT:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])*svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]));
          goto exe;
          case OP_SUB:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])-svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]));
          goto exe;
          case OP_DIV:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])/svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]));
          goto exe;
          case OP_MOD:
             svmSetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte1],
                  (slong)svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte2])%(slong)svmGetGenericValue(gSvm.env->getBitmap().objs[(long) op_ags.byte3]));
          goto exe;
       }
       goto exe;

}
