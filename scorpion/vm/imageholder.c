 #include "imgholder.h"
 #include "scorpionvm.h"
 #include "Opcodes.h"
 #include "Globals.h"
 #include "../logservice/alog.h"
 #include "exception.h" 
 #include "../clib/u4.h"
 #include <sstream>
 #include <iostream>
 #include <string>
 using namespace std;

extern long streamcount;
extern ALog alog;
unsigned long pos;

 double getb(){
    pos = gSvm.vm.vStaticRegs[VREG_PC]++;
    
    if(pos >= gSvm.bytestream.size())
       return tok_eof;

    return gSvm.bytestream.valueAt(pos);
 }

 u4_d op_ags;
 
 void agsclear(){
     op_ags.byte1 = 0;
     op_ags.byte2 = 0;
     op_ags.byte3 = 0;
     op_ags.byte4 = 0;
 }

 static long CurByte;
 int instrgroup = 5;
 stringstream word;
 static long LastChar = ' ';
 long getByte(){
     LastChar = getb();

     if(LastChar == tok_eof)
     {
       alog.ALOGV("fatal error: application attempting to close unexpectingly.");
       segfault();
     }
     
     if(LastChar < 0 || LastChar > sMaxOpcodeLimit)
     {
       alog.ALOGV("fatal error: attempting to execute unknown instruction.");
       segfault();
     }
        
     if(LastChar == OP_NOP || LastChar == OP_END || LastChar == OP_HLT || LastChar == OP_NO || LastChar == OP_ENDNO){
         instrgroup = 0;
         return LastChar;
     }
     else if(LastChar == OP_RETURN || LastChar == OP_PUSH || LastChar == OP_POP || LastChar == OP_JMP || LastChar == OP_CALL
       || LastChar == OP_MTHD || LastChar == OP_LBL || LastChar == OP_IF || LastChar == OP_INC || LastChar == OP_DEC
       || LastChar == OP_KILL || LastChar == OP_DELETE || LastChar == OP_DELETE_ARRY){ // push 7 or push *x
         instrgroup = 1;
         op_ags.byte1 = getb();
         
         return LastChar;
     }
     else if(LastChar == OP_ICONST || LastChar == OP_DCONST || LastChar == OP_FCONST || 
        LastChar == OP_SCONST || LastChar == OP_BCONST || LastChar == OP_CCONST || LastChar == OP_RSHFT 
        || LastChar == OP_LSHFT || LastChar == OP_CIN || LastChar == OP_STR_APND || LastChar == OP_ASSN 
        || LastChar == OP_JIF || LastChar == OP_JIT || LastChar == OP_THROW || LastChar == OP_ACONST 
        || LastChar == OP_STR_ACONST || LastChar == OP_CAST){ // mthd @9
         instrgroup = 2;
         op_ags.byte1 = getb();
         op_ags.byte2 = getb();
         return LastChar;
     }
     else if(LastChar == OP_ISEQ || LastChar == OP_ISNEQ || LastChar == OP_ISLT || LastChar == OP_ISNLT || LastChar == OP_ISLE || LastChar == OP_ISNLE
       || LastChar == OP_ISGT || LastChar == OP_ISNGT || LastChar == OP_ISGE || LastChar == OP_ISNGE || LastChar == OP_IADD
       || LastChar == OP_ISUB || LastChar == OP_IMULT || LastChar == OP_IDIV || LastChar == OP_SADD
       || LastChar == OP_SSUB || LastChar == OP_SMULT || LastChar == OP_SDIV || LastChar == OP_DADD
       || LastChar == OP_DSUB || LastChar == OP_DMULT || LastChar == OP_DDIV || LastChar == OP_FADD
       || LastChar == OP_FSUB || LastChar == OP_FMULT || LastChar == OP_FDIV || LastChar == OP_CADD
       || LastChar == OP_CSUB || LastChar == OP_CMULT || LastChar == OP_CDIV || LastChar == OP_IMOD
       || LastChar == OP_CMOD || LastChar == OP_SMOD || LastChar == OP_OR || LastChar == OP_AND
       || LastChar == OP_AT || LastChar == OP_ALOAD || LastChar == OP_ASTORE){ //
         instrgroup = 3;
         op_ags.byte1 = getb();
         op_ags.byte2 = getb();
         op_ags.byte3 = getb();
         return LastChar;
     }
     else if(LastChar == OP_COUT){ // string 13 'Hello, World!'
         instrgroup = 4;
         op_ags.byte1 = getb();
         char c;
         word.str("");
         for(int i = 0; i < op_ags.byte1; i++){
             c = (char) getb();
             word << "" << c;
         }
         
         return LastChar;
     }
     else if(LastChar == OP_STRCONST){ // string 13 'Hello, World!'
         instrgroup = 4;
         op_ags.byte1 = getb();
         op_ags.byte2 = getb();
         
         char c;
         word.str("");
         for(int i = 0; i < op_ags.byte2; i++){
             c = (char) getb();
             word << "" << c;
         }
         
         return LastChar;
     }
     
     alog.ALOGV("fatal error: attempting to execute unknown instruction.");
     segfault();
 }


string ImageHolder::getStr(){
     return word.str();
 }

u4_d ImageHolder::getAgs(){
    return op_ags;
}
      
long ImageHolder::getOp(){
    return CurByte;
}

int ImageHolder::getGroup(){
   return instrgroup;
}

 int ImageHolder::getNextInstr(){ //parse this alike the Compilr library
     return getByte();
 }
 
 
 
