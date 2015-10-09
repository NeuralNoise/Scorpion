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
#ifndef SCORPION_VM_IMAGE_HOLDER
#define SCORPION_VM_IMAGE_HOLDER

 #include "scorpionvm.h"
 #include <sstream>
 #include <iostream>
 #include <string>
 using namespace std;
 
 enum Img_Token {
    tok_instr = 0x1ff,  // this should be tyhe only expected thing passed back
    tok_eof = 0xeff0, // end of image
    tok_floating = 0xddf, // you do not want to get this back, this means that the instruction in the image index was not an instruction
                        // a Seg fault will be thrown 
 };


class ImageHolder {
    public:
       int getNextInstr();
       string getStr();
       u4_d getAgs();
       long getOp();
       int getGroup();
};


#endif
