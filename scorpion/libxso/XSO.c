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
 #include "xso.h"
 #include "../vm/exception.h"
 #include "../clib/u4.h"
 #include "../clib/u2.h"
 #include "../clib/zlib/zlib.h"
 #include "../clib/u1.h"
 #include "../clib/binary.h"
 #include "../logservice/alog.h"
 #include "../vm/Globals.h"
 #include "../vm/oo/Object.h"
 #include "../vm/Opcodes.h"
 #include <sstream>
 #include <stdio.h>
 #include <stdlib.h>     
 #include <iostream>
 #include <string>
 
 using namespace std;

bool eof = false;

 string XSO_F;
 long soIdx = 0;
 
 char getch()
 {
     if(!(soIdx < XSO_F.size())){ // protect against a index out of bounds exception
       eof = true;
       return (char) EOF;
     }
       
     return XSO_F.at(soIdx++);
 }

u4 getmagic()
{
    u4 m;
    m.byte1 = (int) getch();
    m.byte2 = (int) getch();
    m.byte3 = (int) getch();
    m.byte4 = (int) getch();
    return m;
}

u2 getmajor()
{
    u2 m;
    m.byte1 = (int) getch();
    m.byte2 = (int) getch();
    return m;   
}

int ctoi(char c)
{
    switch( c ){
        case '9':
            return 9;
        case '8':
            return 8;
        case '7':
            return 7;
        case '6':
            return 6;
        case '5':
            return 5;
        case '4':
            return 4;
        case '3':
            return 3;
        case '2':
            return 2;
        case '1':
            return 1;
        case '0':
            return 0;
        default:
            return 0;
    }
}

bool equals(char c, long x)
{
    return c == ((char) x);
}

bool decode = true;

string getheadertxt()
{
    char c;
    stringstream ss;
    for(int i = soIdx; i < XSO_F.size(); i++){
         c = getch();
         if(eof){
            eof = false;
            Exception("Unexpected end of file while reading application.", "UnexpectedEventException");
         }
            
         if(c == 0)
           break;
         else
           ss << c;
     }
    if(decode){    
      stringstream stream_buf;
      Binary::decode_str(ss.str(), stream_buf);
      return stream_buf.str();
    }
    else 
      return ss.str();
}

string getimage()
{
    Exception::trace.addproto("vm.internal.system.getimage", "XSO", 1);
    stringstream ss;
    for(int i = soIdx; i < XSO_F.size(); i++){
            ss << XSO_F.at(i) << "";
    }
   return ss.str();
}

string rmnln(string data){
    stringstream ss;
    for(long i = 0; i < data.size(); i++){
        if(data.at(i) == '\n'){ }
        else
          ss << data.at(i);
    }
   return ss.str();
}

 void XSO::process(string data)
 {
     Exception::trace.addproto("vm.internal.system.process", "XSO", 1);
     XSO_F = data;
     soIdx = 0;
     
     u4 magic;
     magic.byte1 = 0xdf;
     magic.byte2 = 0x4e;
     magic.byte3 = 0xfa;
     magic.byte4 = 0x2b;
     
     if(!(equals(getch(), 0xdf) && equals(getch(), 0x4e) && 
           equals(getch(), 0xfa) && equals(getch(), 0x2b)))
         Exception("XSO file format Error, invalid magic number!", "XSOFileFormatException");
         
     
     headerInf.magic = magic;
     getch(); // eat offset

     if(!equals(getch(), 0x7a))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
        
     decode = false;    
     headerInf.name = getheadertxt();
     decode = true;
     getch();
     getch();
     
     if(!equals(getch(), 0xd9))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
        
     headerInf.method_size.byte1 = atoi(getheadertxt().c_str());
     
     if(!equals(getch(), 0xAF))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
     
     headerInf.minor_version.byte1 = (int) getch();
     headerInf.minor_version.byte2 = (int) getch();
     getch(); // eat offset
     
     if(!equals(getch(), 0xCE))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
        
     u2 major_version;
     major_version.byte1 = 0x7;
     major_version.byte2 = 0x10;
     
     if(!(equals(getch(), 0x7) && equals(getch(), 0x10)))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
        
     getch(); // eat offset
     
     headerInf.major_version = major_version;
     
     if(!equals(getch(), 0xEF))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
    
     headerInf.filesize.byte1 = atoi(getheadertxt().c_str());
     
     if(!equals(getch(), 0xBC))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
        
     headerInf.target_dev_vers.byte1 = (int) getch();
     getch();
     
     if(!equals(getch(), 0xFD))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
     
     headerInf.minimum_dev_vers.byte1 = (int) getch();
     getch();
     
     if(!equals(getch(), 0xCE))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
     
     headerInf.version_number = getheadertxt();
     decode=false;
    
     
     if(!equals(getch(), 0xEC))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
     
     headerInf.debug.byte1 = ctoi(getch());
     getch();  
     
     if(!equals(getch(), 0xFC))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
     
     headerInf.logging.byte1 = ctoi(getch());
     getch();  
     
     if(!equals(getch(), 0xFA))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
     
     headerInf.log_precedence.byte1 = atoi(getheadertxt().c_str());
     decode = true;
     
     if(!equals(getch(), 0x3C))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
        
     headerInf.log_file = getheadertxt();
     
     if(!equals(getch(), 0xb5))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
        
     headerInf.application_id = getheadertxt();
     getch();   // eat \n
     
     if(!equals(getch(), 0xc1))
        Exception("Invalid flag found in header.", "XSOFileFormatException");
        
     headerInf.permissions = getheadertxt();
    
     // get image file here
     if(headerInf.filesize.byte1 == 0)
        Exception("Image section is empty!", "XSOFileFormatException");
     
     for(int i = 0; i < (12 + 1 + 20); i++)
        getch();  // eat all null values
        
     alog.setClass("XSOImg");
     alog.ALOGI("XSO processing successful. Getting XSO Image.");
     Zlib zlib;
     stringstream __img_buf__;
     
     zlib.Decompress_Buffer2Buffer(getimage(), __img_buf__);
     if(zres.response == ZLIB_FAILURE){
         cout << "\n" << zres.reason.str() << "Shutting down.\n";
         zlib.Cleanup();
         Init_ShutdownScorpionVM();
     }
     else if(zres._warnings_.str() != "")
         cout << zres._warnings_.str();
     
     gSvm.image = rmnln(__img_buf__.str());
     __img_buf__.str("");
     zlib.Cleanup();
 }
 
 
 void preexecute_err(){
       alog.setClass("XSO");
       alog.ALOGV("Failed to properly preprocess the XSO image. Have you compiled the image correctly?");
       segfault();
 }

 
 // *********************[[ Compiler flag id's ]]**********************
 #define method (0x7f)
 #define instr (0x5a)
 #define string_instr (0x3b)
 #define out_instr (0x2c)
 
long streamcount = 0;
static int LastChar = ' ';
 static int getTok(){
    return LastChar = getch();
 }
 
 static long CurTok;
 static int getNextToken(){
     return CurTok = getTok();
 }
 
 void setbyte(double byte){
     if(streamcount > gSvm.appheader.filesize.byte1){
        alog.setClass("XSO");
        alog.ALOGV("Image size bigger than specified. Try recompiling your application.");
        preexecute_err();
     }
    
     streamcount++;
     gSvm.bytestream.add(byte);
 }
 
 void svmInitMethod(long pos, string name, long jmp_adr){
     if(pos >= gSvm.methodc){
         alog.setClass("XSO");
         alog.ALOGV("There are more methods than the expected ammount. Have you compiled your application correctly?");
         preexecute_err();
     }
     
     stringstream mthdname;
     stringstream classname;
     stringstream modulename;
     
     
     mthdname.str("");
     classname.str("");
     modulename.str("");
     if(name == ""){
         alog.setClass("XSO");
         alog.ALOGV("Method name cannot be blank. Have you compiled your application correctly?");
         preexecute_err();
     }
       
    int index = 0;
    bool isnative = false;
    if(name.at(0) == '~'){
       index = 1;
       isnative = true;
    }
    
    for(int i = index; i < name.size(); i++){
        if(name.at(i) == '&'){
            index++;
            break;
        }
        else {
          mthdname << "" << name.at(i);
        }
        index++;
    }
    
    for(int i = index; i < name.size(); i++){
        if(name.at(i) == '&'){
            index++;
            break;
        }
        else
          classname << "" << name.at(i);
        index++;
    }
    
    for(int i = index; i < name.size(); i++)
          modulename << "" << name.at(i);
          
    if(mthdname.str() == "" || modulename.str() == "" || classname.str() == ""){
         alog.setClass("XSO");
         alog.ALOGV("Method name was missing required information. Have you compiled your application correctly?");
         preexecute_err();
    }
     
     gSvm.mtds[pos].name = mthdname.str();
     gSvm.mtds[pos].clazz = classname.str();
     gSvm.mtds[pos].module = modulename.str();
     gSvm.mtds[pos].native = isnative;
     gSvm.mtds[pos].ref.byte2 = jmp_adr;
 }
 
 #define InitMthd(l, name, jmp) \
      svmInitMethod(l, name, jmp);
 
 void HandleMethod(){ // @^name0location0
     CurTok=atof(getheadertxt().c_str());
     if(CurTok != OP_MTHD)
         preexecute_err();

     string methodname = getheadertxt(); // get text from image
     long l = atoi(getheadertxt().c_str());
     
     setbyte(OP_MTHD);
     setbyte(l);
     
    // cout << "setting method " << methodname << " at location " << l << endl;
     InitMthd(l, methodname,streamcount);
     
     getNextToken();
 }
 
 void HandleStringInstruction(){
     CurTok=atof(getheadertxt().c_str());
     if(CurTok == OP_COUT){
         string msg = getheadertxt();
         long arg1 = msg.size();
         
         setbyte(CurTok);
         setbyte(arg1);
         for(int i = 0; i < arg1; i++)
             setbyte((int) msg.at(i));
             
         getNextToken();
     }
     else if(CurTok == OP_STRCONST){
         long arg1 = atoi(getheadertxt().c_str());
         string msg = getheadertxt();
         long arg2 = msg.size();
         
         setbyte(CurTok);
         setbyte(arg1);
         setbyte(arg2);
         for(int i = 0; i < arg2; i++)
             setbyte((int) msg.at(i));
             
         getNextToken();
     }
     else
        preexecute_err();
 }
 
 // TODO: pre assign const values (i_const ect.)
 void HandleInstruction(){ // ^180200290
     CurTok=atof(getheadertxt().c_str());
     if(!(CurTok > 0) && !(CurTok <= sMaxOpcodeLimit))
         preexecute_err();
     
     if(CurTok == OP_NOP || CurTok == OP_END || CurTok == OP_HLT || CurTok == OP_NO || CurTok == OP_ENDNO){  
         setbyte(CurTok);
         getNextToken();
         return;
     }
     else if(CurTok == OP_RETURN  || CurTok == OP_PUSH || CurTok == OP_POP || CurTok == OP_JMP || CurTok == OP_CALL
        || CurTok == OP_MTHD || CurTok == OP_LBL || CurTok == OP_IF || CurTok == OP_INC || CurTok == OP_DEC 
        || CurTok == OP_KILL || CurTok == OP_DELETE || CurTok == OP_DELETE_ARRY){
        double arg1 = atof(getheadertxt().c_str());
        setbyte(CurTok);
        setbyte(arg1);
        
        if(CurTok == OP_LBL) {// pre assign label locations
            u1 sz;
            sz.byte1 = 1;
            SVM_OBJECT_INIT(gSvm.env->getBitmap().objs[(long) arg1], TYPEDEF_GENERIC_LONG, sz);
            svmSetGenericValue(gSvm.env->getBitmap().objs[(long) arg1], streamcount);
        }
        
        getNextToken();
        return;
     }
     else if(CurTok == OP_SCONST || CurTok == OP_BCONST || CurTok == OP_CCONST || CurTok == OP_RSHFT 
       || CurTok == OP_LSHFT || CurTok == OP_CIN || CurTok == OP_JIF 
       || CurTok == OP_JIT || CurTok == OP_ICONST || CurTok == OP_DCONST || CurTok == OP_FCONST 
       || CurTok == OP_THROW || CurTok == OP_STR_APND || CurTok == OP_ASSN
       || CurTok == OP_STR_ACONST || CurTok == OP_CAST || CurTok == OP_BYTE_CONST || CurTok == OP_LCONST
       || CurTok == OP_DACONST || CurTok == OP_IACONST || CurTok == OP_FACONST || CurTok == OP_CACONST 
       || CurTok == OP_BACONST || CurTok == OP_BYTE_ACONST || CurTok == OP_SACONST || CurTok == OP_LACONST){
        double arg1 = atof(getheadertxt().c_str());
        double arg2 = atof(getheadertxt().c_str());
        setbyte(CurTok);
        setbyte(arg1);
        setbyte(arg2);
        
        getNextToken();
        return;
     }
     else if(CurTok == OP_IADD || CurTok == OP_ISEQ || CurTok == OP_ISNEQ
       || CurTok == OP_ISLT || CurTok == OP_ISNLT || CurTok == OP_ISLE || CurTok == OP_ISNLE
       || CurTok == OP_ISGT || CurTok == OP_ISNGT || CurTok == OP_ISGE || CurTok == OP_ISNGE
       || CurTok == OP_ISUB || CurTok == OP_IMULT || CurTok == OP_IDIV || CurTok == OP_SADD
       || CurTok == OP_SSUB || CurTok == OP_SMULT || CurTok == OP_SDIV || CurTok == OP_DADD
       || CurTok == OP_DSUB || CurTok == OP_DMULT || CurTok == OP_DDIV || CurTok == OP_FADD
       || CurTok == OP_FSUB || CurTok == OP_FMULT || CurTok == OP_FDIV || CurTok == OP_CADD
       || CurTok == OP_CSUB || CurTok == OP_CMULT || CurTok == OP_CDIV || CurTok == OP_IMOD
       || CurTok == OP_CMOD || CurTok == OP_SMOD || CurTok == OP_OR || CurTok == OP_AND
       || CurTok == OP_AT || CurTok == OP_ALOAD || CurTok == OP_ASTORE){
        double arg1 = atof(getheadertxt().c_str());
        double arg2 = atof(getheadertxt().c_str());
        double arg3 = atof(getheadertxt().c_str());
        setbyte(CurTok);
        setbyte(arg1);
        setbyte(arg2);
        setbyte(arg3);
        
        getNextToken();
        return;
     }
     else
       preexecute_err();
 }
 
 void XSO::preprocess(string data) // very simple image preprocessor
 {
     Exception::trace.addproto("vm.internal.system.preprocess", "XSO", 1);
     
     alog.setClass("XSO");
     alog.ALOGI("preprocessing XSO image.");
     
     XSO_F = data;
     decode = false;    
     soIdx = 0;
     if(gSvm.appheader.filesize.byte1 > XSO_MAX_BUF_LEN){
         alog.setClass("XSO");
         alog.ALOGV("error: could not create byte stream for image file processing.");
         stringstream ss;
         ss << "Image size too large, size: " << gSvm.appheader.filesize.byte1 << " > (" << XSO_MAX_BUF_LEN << ")";
         Exception(ss.str(), "MemoryOverloadErr");
     }
     
    getNextToken();
     while( 1 ){
         switch( CurTok ){
             case 0:
                 getNextToken();
             break;
             case method:
                 HandleMethod(); // mthd [MainActivity]main cout 'Hello, World!' return main 0 call main hlt
             break;
             case instr:
                 HandleInstruction();
             break;
             case string_instr:
                  HandleStringInstruction();
             break;
             case out_instr: // cout 'Hello World from \[375'
                  HandleStringInstruction();
             break;
             default:
                if(eof){
                  eof = false;
                  gSvm.image = "";
                  if(streamcount != gSvm.appheader.filesize.byte1){
                      alog.setClass("XSO");
                      alog.ALOGV("Image size does not match specified length. Try recompiling your application.");
                      preexecute_err();
                  }
                  return;
                }
                preexecute_err();
             break;
         }
     }
     
 }
 
 void XSO::printfullheaderinfo()
 {
     printf("magic:  0x%x 0x%x 0x%x 0x%x\n", (int) headerInf.magic.byte1, (int) headerInf.magic.byte2, 
                                                (int) headerInf.magic.byte3, (int) headerInf.magic.byte4);
     printf("name:   %s\n", headerInf.name.c_str());
     printf("application id:   %s\n", headerInf.application_id.c_str());
     printf("target dev version:  %lu\n", headerInf.target_dev_vers.byte1);
     printf("minumum dev version:  %lu\n", headerInf.minimum_dev_vers.byte1);
     printf("version number:   %s\n", headerInf.version_number.c_str());
     printf("logging:  %lu\n", headerInf.logging.byte1);
     printf("log file:   %s\n", headerInf.log_file.c_str());
     printf("minor version:  %lu.%lu\n", headerInf.minor_version.byte1, headerInf.minor_version.byte2);
     printf("major version:  %lu.%lu\n", headerInf.major_version.byte1, headerInf.major_version.byte2);
     printf("debug:  %li\n", headerInf.debug.byte1);
     printf("method size:  %lu\n", headerInf.method_size.byte1);
     printf("permissions:   %s\n", headerInf.permissions.c_str());
     printf("log precedence:  %lu\n", headerInf.log_precedence.byte1);
     printf("image size:  %lu bytes\n\n", headerInf.filesize.byte1);
 }

