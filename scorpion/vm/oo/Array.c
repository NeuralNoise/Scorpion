 
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
 #include <string>
 #include <sstream>
 
 using namespace std;
  
 ArrayObject* tochararray(string data){
     ArrayObject* obj = new ArrayObject[0];
     obj->generic = new double[data.size()];
     
     obj->length = data.size();
     
     if(obj->length > STR_LIMIT){}
       // Exception("Scring greater than max limit.", "StringOverloadException");
       
     for(int i = 0; i < data.size(); i++)
       obj->generic[i] = (int) data.at(i);
       
     return obj;
 }
 
 string fromchararray(ArrayObject &arrayobj){
     stringstream ss;
     
     for(int i = 0; i < arrayobj.length; i++)
       ss << (char) arrayobj.generic[i];
       
     return ss.str();
 }
 
 ArrayObject* ostr_arraymesh(ArrayObject &arrayobj, ArrayObject &arrayobj2){
     ArrayObject* obj = new ArrayObject[0];
     
     stringstream ss;
     ss << fromchararray(arrayobj) << fromchararray(arrayobj2);
     
     obj = tochararray(ss.str());
     if(obj->length > STR_LIMIT){}
       // Exception("Scring greater than max limit.", "StringOverloadException");
       
     return obj;
 }
 
 
 
 /* Generic array parsing stuff */
 double get(Object &obj, long pos){
     if(pos >= obj.obj->arrayobj->length){}
      // Exception("Index " + pos + " is not within bounds. Array size[" + arrayobj.length + "].", "ArrayIndexOutOfBoundsException");
     
     return obj.obj->arrayobj->generic[pos];
 }
 
 void set(Object &obj, long pos, double default_value){
     if(pos >= obj.obj->arrayobj->length){}
      // Exception("Index " + pos + " is not within bounds. Array size[" + arrayobj.length + "].", "ArrayIndexOutOfBoundsException");
      
      obj.obj->arrayobj->generic[pos] = default_value;
 }
 
 
 long length(Object &obj){
     return obj.obj->arrayobj->length;
 }
 
 
 
