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
#ifndef SCORPION_ENVIRONMENT
#define SCORPION_ENVIRONMENT

#include "../clib/u4.h"
#include "../clib/u2.h"
#include "../clib/u1.h"
#include "exception.h"
#include "scorpionvm.h"
#include "../logservice/alog.h"
#include "../libxso/xso.h"
 #include "alloc/HeapBitmap.h"
#include <sstream>
#include <iostream>
#include <string>
using namespace std;
 


  /*
  * Scorpion Runtime Envirnment (SRE)
  *
  * This class will represent the environment interface 
  * that the Scorpion Virtual Machine will interact with.
  *
  * An environment is scorpion is much more flexible than 
  * the scorpion vm itsself. Each virtual machine can have multiple
  * environments, but each environment cannot have multiple virtual
  * machines.
  *
  * There can only be a single virtual machine runnning at any given time 
  * per processee.
  */
  class ScorpionEnv {
        string name;
        long methodLimit;
        
        HeapBitmap memBitmap;
   //     BlockTable table; // every environment has its own tabel
        
      #define nullptr ((void *)0)
      
      public:      
        int InitEnvironmentSetup(string env_name, XSO f, long minHeap, long maxHeap, long blocksz, long stacksz){
            Exception::trace.addproto("vm.internal.system.InitEnvironmentSetup", "ScorpionEnv", 1);
            setName(env_name);
            
            alog.setClass("ScorpionEnv");
            stringstream ss;
            ss << "New Scorpion environment: " << env_name << ".";
            alog.ALOGD(ss.str());
            alog.setClass("ScorpionEnv");
            alog.ALOGV("Setting up new environment.");

            //table = svmBlockTableStartup(table, minHeap, maxHeap, f.headerInf.method_size.byte1, blocksz, stacksz);
            
        /*   if(!BlockTableInitilized){
               alog.setClass("ScorpionEnv");
               alog.ALOGV("Environment was not created successfully. Have you requested too much memory?");
               return -1;
           }*/
          
       //   svmBlockToAddr(table, 0, 12, 55, "");
       //   cout << table.mDataBlock[0].getChild(12) << endl;
          
          alog.setClass("ScorpionEnv");
          alog.ALOGV("Environment was created successfully.");
           return 0;
        }
        string getName(){
          Exception::trace.addproto("vm.internal.system.getName", "ScorpionEnv", 1);
          return name;
        }
        void setName(string n){
          Exception::trace.addproto("vm.internal.system.setName", "ScorpionEnv", 1);
          name = n;
        }
        long getMethodLimit(){
          Exception::trace.addproto("vm.internal.system.getMethodLimit", "ScorpionEnv", 1);
          return methodLimit;
        }
        void setMethodLimit(long l){
          Exception::trace.addproto("vm.internal.system.setMethodLimit", "ScorpionEnv", 1);
          methodLimit = l;
        }
        HeapBitmap getHeapBitmap(){
          return memBitmap;
        }
  };

#endif // SCORPION_ENVIRONMENT


