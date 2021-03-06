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
#ifndef LOG
#define LOG

#include <string>
using namespace std;

extern const int VERBOSE;

extern const int DEBUG;

extern const int INFO;

extern const int WARN;

extern const int ERROR;

extern const int ASSERT;

 class Log {
   public:
     static string a(string tag,string message, string file, bool log, string stackmsg, int p);
     static string e(string tag,string message, string file, bool log, string stackmsg, int p);
     static string w(string tag,string message, string file, bool log, string stackmsg, int p);
     static string i(string tag,string message, string file, bool log, string stackmsg, int p);
     static string d(string tag,string message, string file, bool log, string stackmsg, int p);
     static string v(string tag,string message, string file, bool log, string stackmsg, int p);
 };

#endif
