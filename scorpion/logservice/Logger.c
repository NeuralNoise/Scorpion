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
#include <iostream>
#include <fstream>
#include "ctime.h"
#include "alog.h"
#include "../vm/Globals.h"
#include "Logger.h"
#include "../clib/filestream.h"

using namespace std;

string t_ch(int priority)
{
   switch( priority ){
      case 2:
        return "V";
      case 3:
        return "D";
      case 4:
        return "I";
      case 5:
        return "W";
      case 6:
        return "E";
      case 7:
        return "A";
      default:
        return "?";
   }
}


string getModuleName(){
    return "vm.internal.system";
}

string getAppId(string file)
{
   if (file == alog.getFile())
      return getModuleName();
   else
     return gSvm.appheader.application_id;
}


string println(int priority,string tag,string message, string file, string stackmsg)
{
     stackmsg += currtime() + " " + getAppId(file) + " " + t_ch(priority) + "/" + tag + ": " + message + "\n";

     FileStream::out(file.c_str(), stackmsg);
     return stackmsg;
}

