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
 #ifndef SCORPION_XSO_FILE
 #define SCORPION_XSO_FILE
 
 #include <string>
 #include <stdint.h>
 #include "../clib/u1.h"
 #include "../clib/u2.h"
 #include "../clib/u4.h"
 
 using namespace std;
 
 namespace scorpionvm
 {
     namespace _xso
     {
         struct header {
            u4            magic;
            u2            minor_version;
            u2            major_version;
            u1            target_dev_vers;
            u1            minimum_dev_vers;
            string        version_number;
            u1            debug;
            u1            logging;
            u1            log_precedence;
            string        log_file;
            string        application_id;
            string        permissions;
            u1            filesize;            // The full size of the image section in bytes
            string        name;
            u1            method_size;
        };
        
        enum 
        {
            prototype_limit=(64 + 16 *(132 * 31)+2),
            xso_max_buf_len=((64 * 1020 * 156) * 8)
        };
        
        class xso
        {
            public:
              header xso_header;
        };
     } // _xso
 } // end scorpionvm
 
 #endif // SCORPION_XSO_FILE