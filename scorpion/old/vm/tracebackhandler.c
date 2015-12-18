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
#include "traceback.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

int TraceBack::protos = 0;
bool TraceBack::shownative = false;

string trace_protos[20];
string trace_classes[20];
      
extern string getModuleName();
string mod;


void TraceBack::addproto(string proto, string klass, long line, int native)
{
    if(native && shownative){
       stringstream s1;
       s1 << "(native) " << proto;
       proto = s1.str();
    }
    
    
    stringstream linenum;
    linenum << klass << ":" << line;
    protos++;
    bool update = false;
    if(protos >= 19)
     update = true;
     
    if(update){
        for(int i = 0; i < 18; i++){
            if( i + 1 >= 18){
                trace_protos[i] = proto;
                trace_classes[i] = linenum.str();
                protos = 18;
                return;
            }
            else{
                trace_protos[i] = trace_protos[i + 1];
                trace_classes[i] = trace_classes[i + 1];
            }
        }
    }
    else {
        trace_protos[protos - 1] = proto;
        trace_classes[protos - 1] = linenum.str();
    }
}

void TraceBack::dumprotos()
{
    for(int i = 0; i < 20; i++){
        trace_protos[i] = "";
        trace_classes[i] = "";
    }
}

void TraceBack::printStackTrace()
{
    printf("  Traceback (most recent call last):\n");
    for(int i = 0; i < protos; i++){
        printf("       at %s(class::%s)\n", trace_protos[i].c_str(), trace_classes[i].c_str());
    }
}

string TraceBack::getStackTrace()
{
    stringstream ss;
    ss << "  Traceback (most recent call last):\n";
    for(int i = 0; i < protos; i++){
        ss << "       at " << trace_protos[i] << "(class::" << trace_classes[i] << ")\n";
    }
  return ss.str();
}



