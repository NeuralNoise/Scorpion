import macros.System;

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
* Scorpion Open Source Project
* This file contains classes and methods that will be used as
* library resources for the Scorpion programming language.
*
* LogManager is responsible for controlling the log that is 
* being sent to '/usr/share/scorpion/log.txt'.
*/
 
@ Manage logs
public class LogManager:
   public LogManager(): end
   
   static const int VERBOSE = 2;

   static const int DEBUG = 3;

   static const int INFO = 4;

   static const int WARN = 5;

   static const int ERROR = 6;

   static const int ASSERT = 7;

   private static bool isLogEnabled = true;
   private static int priorityStackLvl = VERBOSE;

   function enable():
     @ Enable log
     if(!isLogEnabled):
       
     end
     else:
       System.console.println("Log is already enabled!");
     end
   end

   function disable():

   end

   static function bool isEnabled():
     return isLogEnabled;
   end

   static function int getPriority():
     return priorityStackLvl;
   end

   function setPriorityLevel(int priority):
     
   end

endclass
