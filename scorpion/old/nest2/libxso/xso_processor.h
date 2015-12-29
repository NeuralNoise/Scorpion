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
 #ifndef SCORPION_XSO_PROCESSOR
 #define SCORPION_XSO_PROCESSOR
 
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <signal.h>
 #include <assert.h>
 #include <iostream>
 #include <sstream>
 #include <string>
 #include <stdint.h>
 #include "xso.h"
 
 using namespace std;
 
 // *********************[[ Compiler flag id's ]]**********************
 #define method (0x7f)
 #define instr (0x5a)
 #define string_instr (0x3b)
 #define out_instr (0x2c)

 class xso_processor
 {
     public:
       xso_processor()
       : i(0),
         byte(0),
         img_t(0),
         _eof(false),
         image("")
       {
       }
       bool _eof;
       uint64_t i, img_t;
       std::string image;
       char byte;
       
       char read_byte();
       int preprocess();
       string xso_text();
       
       void process_method();
       void setbyte(double byte);
       void process_instruction();
       void process_str_instruction();
 };
 
 #endif // SCORPION_XSO_PROCESSOR