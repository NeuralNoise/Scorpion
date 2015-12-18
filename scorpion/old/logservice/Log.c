/*
* Copyright 2015 Braxton Nunnally
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*/
#include <string>
#include "Log.h"
#include "Logger.h"
#include "filter.h"
using namespace std;

const int VERBOSE = 2;

const int DEBUG = 3;

const int INFO = 4;

const int WARN = 5;

const int ERROR = 6;

const int ASSERT = 7;

string Log::a(string tag, string message, string file, bool log, string stackmsg, int p)
{
   return print(ASSERT,tag,message,log,file, stackmsg, p);
}

string Log::e(string tag,string message, string file, bool log, string stackmsg, int p)
{
   return print(ERROR,tag,message,log,file, stackmsg, p);
}

string Log::w(string tag,string message, string file, bool log, string stackmsg, int p)
{
   return print(WARN,tag,message,log,file, stackmsg, p);
}

string Log::i(string tag,string message, string file, bool log, string stackmsg, int p)
{
   return print(INFO,tag,message,log,file, stackmsg, p);
}

string Log::d(string tag,string message, string file, bool log, string stackmsg, int p)
{
   return print(DEBUG,tag,message,log,file, stackmsg, p);
}

string Log::v(string tag,string message, string file, bool log, string stackmsg, int p)
{
   return print(VERBOSE,tag,message,log,file, stackmsg, p);
}
