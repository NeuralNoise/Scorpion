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
#include "clib/compilr/compilr.h"
#include "compiler.h"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
using namespace std;

Archive archive;
Compilr kernal;

extern Zlib zlib;

int Compiler::compile(){
    stringstream ss;
    
    for(int i = 0; i < size_t; i++)
    {
        int res=archive.extract(libraries[i].c_str());
    
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
        
        
        stringstream _ostream_buf__;
        kernal.Compile_Buffer2Buffer(archive, _ostream_buf__);
        
        if(res != 0)
           return res;
        
        archive.clean();
    }
    
    return 0;
}

