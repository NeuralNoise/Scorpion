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
#include "scorpionc.h"
#include "clib/filestream.h"
#include "clib/zlib/zlib.h"
#include "Archive.h"
#include "SyntaxAnalyzer.h"
#include "compiler.h"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
using namespace std;

Archive archive;

extern Zlib zlib;

#define std_token_threshold (64*32*1020)

int Compiler::compile(){
    stringstream ss;
    cout << "compiling " << libraries[0] << endl;
    
    int res=archive.extract(libraries[0].c_str());
    
    switch( res ){
        case -3:
            cout << "Error: lib file: " << libraries[0] << " invalid magic number!" << endl;
            return res;
        break;
        case -2:
            cout << "Error: lib file: " << libraries[0] << " unexpected end of file!" << endl;
            return res;
        break;
        case 0: break;
        case -4: break;
        default:
            cout << "Error: lib file: " << libraries[0] << " sar file format error!" << endl;
            return res;
        break;
    }
    
    // TODO: Analyze each file
    for(int i = 0; i < archive.header.sourcecount.byte1; i++){
        cout << "compiling " << archive.fmap[i].name << endl;
        SyntaxAnalyzer analyzer;
        Tokenizer tokenizer;
        
        if(tokenizer.tokens.set(std_token_threshold) != 0){
            cout << "err\n";
            break;
        }
        
        tokenizer.p=0;
        tokenizer.line=0;
        tokenizer.eof = false;
        tokenizer.tokens.p=0;
        LastChar = ' ';
        cout << tokenizer.tokens.p << endl;
        
        //TODO: decompress contents
        // archive.fmap[i].contents
        analyzer.setFile(archive.fmap[i].name);
        
        stringstream __fout_buf__;
        zlib.Decompress_Buffer2Buffer(archive.fmap[i].contents, __fout_buf__);
        
        if(zres._warnings_.str() != "")
            cout << zres._warnings_.str();
                               
        if(zres.response == ZLIB_FAILURE){
            cout << "\n" << zres.reason.str() << "Error is not recoverable, exiting with exit status: -1.\n";
            zlib.Cleanup();
            exit(-1);
        }
        
        tokenizer.setStream(__fout_buf__.str());
            
        res+=analyzer.analyze(archive.fmap[i].name, tokenizer);
        
        tokenizer.clean();
    }
    
    if(res != 0)
       return res;
    
    return 0;
}

