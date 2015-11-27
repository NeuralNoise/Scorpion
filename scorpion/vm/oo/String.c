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
 using namespace std;
 
 
 long str_location = 0;
 
 bool isnull(Object &obj){
    
    if(svmObjectHasInstance(obj, TYPEDEF_STRING))
       return (obj.obj->strobj[0].array == nullptr || obj.obj->strobj[0].array == NULL);
    else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY))
       return (obj.obj->arrayobj->strobj[str_location].array == nullptr || 
              obj.obj->arrayobj->strobj[str_location].array == NULL);
    else
       Exception("The requested object is not a string type.", "InvalidObjectException");
 }
 
 unsigned int at(Object &obj, long pos){
     if(!svmObjectIsAlive(obj))
        Exception("String Object has not been created!", "DeadObjectException");
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING))
       return obj.obj->strobj[0].array->generic[pos];
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY))
       return obj.obj->arrayobj->strobj[str_location].array->generic[pos];
     else
        Exception("The requested object is not a string type.", "InvalidObjectException");
 }
 
  void assign(Object &obj, string data){
     if(!svmObjectIsAlive(obj))
        Exception("String Object has not been created!", "DeadObjectException");
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING)){
       obj.obj->strobj[0].array = tochararray(data);
       obj.obj->strobj[0].length = obj.obj->strobj[0].array->length;
     }
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY)) {
       obj.obj->arrayobj->strobj[str_location].array = tochararray(data);
       obj.obj->arrayobj->strobj[str_location].length = obj.obj->arrayobj->strobj[str_location].array->length;
     }
     else 
     
     if(isnull(obj))
      Exception("String Object failed to be reassigned.", "NullpointerException");
 }
 
 string getstr(Object &obj){
    if(!svmObjectIsAlive(obj))
        Exception("String Object has not been created!", "DeadObjectException");
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING))
       return fromchararray(obj.obj->strobj[0].array[0]);
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY)) {
       if(str_location >= obj.obj->arrayobj->length){
           stringstream ss;
           ss << "Index " << str_location << " is not within bounds. Array size[" << length(obj) << "].";
           Exception(ss.str(), "ArrayIndexOutOfBoundsException");
       }
       return fromchararray(obj.obj->strobj[str_location].array[0]);
     }
     else
        Exception("The requested object is not a string type.", "InvalidObjectException");
 }
 
 unsigned int size(Object &obj){
     if(!svmObjectIsAlive(obj))
        Exception("String Object has not been created!", "DeadObjectException");
        
     if(svmObjectHasInstance(obj, TYPEDEF_STRING))
       return obj.obj->strobj[0].length;
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY))
       return obj.obj->arrayobj->strobj[str_location].length;
     else
        Exception("The requested object is not a string type.", "InvalidObjectException");
 }
 
 void concat(Object &obj, string data){
      
     if(isnull(obj))
        Exception("String Object is null!", "NullPointerException");
        
     ArrayObject* aobj = tochararray(data);  
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING)){  
 
       obj.obj->strobj[default_loc].array = ostr_arraymesh(obj.obj->strobj[default_loc].array[default_loc], aobj[default_loc]);
       obj.obj->strobj[default_loc].length = obj.obj->strobj[default_loc].array->length;
     }
     else if(svmObjectHasInstance(obj, TYPEDEF_STRING_ARRAY)) {
       obj.obj->arrayobj->strobj[str_location].array = 
          ostr_arraymesh(obj.obj->arrayobj->strobj[str_location].array[default_loc], aobj[default_loc]);
       obj.obj->arrayobj->strobj[str_location].length = obj.obj->arrayobj->strobj[str_location].array->length;
     }
     else
        Exception("The requested object is not a string type.", "InvalidObjectException");
     
     if(isnull(obj))
      Exception("String Object failed to be reassigned.", "NullpointerException");
 }
 
 