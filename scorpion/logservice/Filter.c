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
#include "Logger.h"
using namespace std;

string print(int priority, string tag, string message, bool log, string file, string stackmsg, int p)
{
     if(!log)
         return stackmsg;
     else if(log){
         if(priority <= p)
             return println(priority,tag,message, file, stackmsg);
         else
             return stackmsg;
     }
}
