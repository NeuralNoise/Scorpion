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
 
 long str_location = 0;
 // TODO: look at instance data
 unsigned int at(Object &obj, long pos){
     if(svmObjectIsDead(obj)){}
        //Exception("String Object has not been created!", "DeadObjectException");
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING))
       return obj.obj->strobj[0].array->generic[pos];
     else
       return obj.obj->arrayobj->strobj[str_location].array->generic[pos];
 }
 
 void assign(Object &obj, string data){
     if(svmObjectIsDead(obj)){}
        //Exception("String Object has not been created!", "DeadObjectException");
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING)){
       obj.obj->strobj[0].array = tochararray(data);
       obj.obj->strobj[0].length = obj.obj->strobj[0].array->length;
     }
     else {
       obj.obj->arrayobj->strobj[str_location].array = tochararray(data);
       obj.obj->arrayobj->strobj[str_location].length = obj.obj->arrayobj->strobj[str_location].array->length;
     }
 }
 
 unsigned int size(Object &obj){
     if(svmObjectIsDead(obj)){}
        //Exception("String Object has not been created!", "DeadObjectException");
        
     if(svmObjectHasInstance(obj, TYPEDEF_STRING))
       return obj.obj->strobj[0].length;
     else
       return obj.obj->arrayobj->strobj[str_location].length;
 }
 
 void concat(Object &obj, string data){
      
     if(svmObjectIsDead(obj)){}
        //Exception("String Object has not been created!", "DeadObjectException");
        
     ArrayObject* aobj = new ArrayObject[1];
     aobj = tochararray(data);  
     
     if(svmObjectHasInstance(obj, TYPEDEF_STRING)){  
 
       obj.obj->strobj[default_loc].array = ostr_arraymesh(obj.obj->strobj[default_loc].array[default_loc], aobj[default_loc]);
       obj.obj->strobj[default_loc].length = obj.obj->strobj[default_loc].array->length;
     }
     else {
       obj.obj->arrayobj->strobj[str_location].array = 
          ostr_arraymesh(obj.obj->arrayobj->strobj[str_location].array[default_loc], aobj[default_loc]);
       obj.obj->arrayobj->strobj[str_location].length = obj.obj->arrayobj->strobj[str_location].array->length;
     }
 }
 
 