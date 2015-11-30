 
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
 #include <string>
 #include <stdlib.h> 
 #include <sstream>
 #include <iostream>
 #include <stdint.h>
 
 using namespace std;
  
 ArrayObject* tochararray(string data){
     ArrayObject* obj = new (nothrow) ArrayObject[1];
     
     if(obj == nullptr)
        return obj;
    
     obj->pchar = new (nothrow) int16_t[data.size()];
     obj->length = data.size();    
     
     if(obj->pchar == nullptr)
        return obj;
     
     if(obj->length > STR_LIMIT)
        Exception("String greater than max limit.", "StringOverloadException");
       
     for(int i = 0; i < data.size(); i++)
       obj->pchar[i] = (int) data.at(i);
       
     return obj;
 }
 
 string fromchararray(ArrayObject &arrayobj){
     stringstream ss;
     
     for(int i = 0; i < arrayobj.length; i++)
       ss << (char) arrayobj.pchar[i];
       
     return ss.str();
 }
 
 ArrayObject* ostr_arraymesh(ArrayObject &arrayobj, ArrayObject &arrayobj2){
     
     stringstream ss;
     ss << fromchararray(arrayobj) << fromchararray(arrayobj2);
     return tochararray(ss.str());
 }
 
 
 
 /* Generic array parsing stuff */
 double get(Object &obj, long pos){
     if(!svmObjectIsAlive(obj))
          segfault();
  
     if(pos >= obj.obj->arrayobj->length){
       stringstream ss;
       ss << "Index at(" << pos << ") is not within bounds. Array size[" << length(obj) << "].";
       Exception(ss.str(), "ArrayIndexOutOfBoundsException");
     }
     
     return obj.obj->arrayobj->pchar[pos];
 }
 
 void set(Object &obj, long pos, double default_value){
     if(!svmObjectIsAlive(obj))
          segfault();
          
     if(pos >= obj.obj->arrayobj->length){
       stringstream ss;
       ss << "Index at(" << pos << ") is not within bounds. Array size[" << length(obj) << "].";
       Exception(ss.str(), "ArrayIndexOutOfBoundsException");
     }
      
      obj.obj->arrayobj->pchar[pos] = default_value;
 }
 
 
 long length(Object &obj){
     return obj.obj->arrayobj->length;
 }
 
 
 
