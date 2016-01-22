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
#ifndef SCORPION_ARCHIVE_H
#define SCORPION_ARCHIVE_H

#include "clib/filestream.h"
#include "clib/u2.h"
#include "clib/u4.h"
#include "clib/u1.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

struct archiveHeader {
    u4            magic;
    u2            minor_version;
    u2            major_version;
    u1            sourcecount;         // The number of source files there are
    string        *sourcefiles;        // The names of each source file
};

struct FileMap {
    string name;
    string include;
    string contents;
};

class Archive {
    
    public:
      FileMap* fmap;
      archiveHeader header;
      string sourceFiles;
    
    public:
      int extract(const char* file);
      void clean();
      
    private:
      int unpack(string __outbuf__);
      int splitfiles();
};

#endif // SCORPION_ARCHIVE_H
