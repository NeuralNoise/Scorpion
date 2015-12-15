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
 #include "Object.h"
 #include "Array.h"
 #include "../exception.h"
 #include <sstream>
 #include <iostream>
 #include <stdlib.h>
 #include <stdint.h>
 using namespace std;
 
 
 long str_location = 0;
 
 unsigned int at(Object &obj, long pos){
     if(!svmObjectIsAlive(obj))
        Exception("String Object has not been created!", "DeadObjectException");
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING)){
       if(pos >= obj.obj->strobj->hash.size()){
        stringstream ss;
        ss << "string::at(" << pos << ") >= string::length[" << obj.obj->strobj->hash.size() << "]";
        Exception(ss.str(), "StringIndexOutOfBoundsException");
       }
        
       return obj.obj->strobj->hash.valueAt(pos);
     }
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY)){
       if(pos >= obj.obj->arrayobj->strobj[str_location].hash.size()){
        stringstream ss;
        ss << "string::at(" << pos << ") >= string::length[" << obj.obj->arrayobj->strobj[str_location].hash.size() << "]";
        Exception(ss.str(), "StringIndexOutOfBoundsException");
       }
        
       return obj.obj->arrayobj->strobj[str_location].hash.valueAt(pos);
     }
     else
        Exception("The requested object is not a string type.", "InvalidObjectException");
 }
 
  void assign(Object &obj, string data){
     if(!svmObjectIsAlive(obj))
        Exception("String Object has not been created!", "DeadObjectException");
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING))
       obj.obj->strobj->hash = tochararray(data);
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY))
       obj.obj->arrayobj->strobj[str_location].hash = tochararray(data);
     else 
        Exception("The requested object is not a string type.", "InvalidObjectException");
 }
 
 string getstr(Object &obj){
    if(!svmObjectIsAlive(obj))
        Exception("String Object has not been created!", "DeadObjectException");
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING))
       return fromchararray(obj.obj->strobj->hash);
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY)) {
       if(str_location >= obj.obj->strobj->hash.size()){
           stringstream ss;
           ss << "Index " << str_location << " is not within bounds. Array size[" << length(obj) << "].";
           Exception(ss.str(), "ArrayIndexOutOfBoundsException");
       }
       return fromchararray(obj.obj->strobj[str_location].hash);
     }
     else
        Exception("The requested object is not a string type.", "InvalidObjectException");
 }
 
 unsigned int size(Object &obj){
     if(!svmObjectIsAlive(obj))
        Exception("String Object has not been created!", "DeadObjectException");
        
     if(svmObjectHasInstance(obj, TYPEDEF_STRING))
       return obj.obj->strobj->hash.size();
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY))
       return obj.obj->arrayobj->strobj[str_location].hash.size();
     else
        Exception("The requested object is not a string type.", "InvalidObjectException");
 }
 
 void concat(Object &obj, string data){
        
     if(svmObjectHasInstance(obj, TYPEDEF_STRING))
         obj.obj->strobj->hash = arryconcat(obj.obj->strobj->hash, tochararray(data));
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY)) {
       arryconcat(obj.obj->arrayobj->strobj[str_location].hash, tochararray(data));
     }
     else
        Exception("The requested object is not a string type.", "InvalidObjectException");
 }
 
 