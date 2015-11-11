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
 #include "../exception.h"
 #include <iostream>
 #include <new>
 #include <stdlib.h> 
 
 using namespace std;

bool svmObjectIsDead(Object &obj){
   return (obj.init.byte1 != OBJECT_ALIVE);
}

bool svmObjectHasInstance(Object &obj, int instance){
   return (obj.instanceData.byte1 == instance);
}

void svmInitHeapObject(Object &obj, int _typedef_, u1 objsz_t, int gc_status){
  if(!svmObjectIsDead(obj))
        return;
  
  obj.init.byte1 = OBJECT_ALIVE;
  obj.instanceData.byte1 = _typedef_;
  obj.instanceData.byte2 = gc_status;
  
  if(_typedef_ == TYPEDEF_GENERIC){
   obj.obj = new (nothrow) DataObject[1];
   
   if(obj.obj == nullptr)
      Exception("Object could not be created.", "OutOfMemoryError");
  }
  if(_typedef_ == TYPEDEF_STRING){
   obj.obj = new (nothrow) DataObject[1];
   obj.obj->strobj = new (nothrow) StringObject[1];
   obj.obj->strobj->array = new (nothrow) ArrayObject[1];
   
   if(obj.obj == nullptr || obj.obj->strobj == nullptr 
      || obj.obj->strobj->array == nullptr)
       Exception("String object could not be created.", "OutOfMemoryError");
  }
  if(_typedef_ == TYPEDEF_GENERIC_ARRAY) {
   obj.obj = new (nothrow) DataObject[1];
   obj.obj->arrayobj = new (nothrow) ArrayObject[1];
   
   if(obj.obj == nullptr || obj.obj->arrayobj == nullptr)
       Exception("Array object could not be created.", "OutOfMemoryError");
   
   obj.obj->arrayobj->length = objsz_t.byte1;
   obj.obj->arrayobj->generic = (double*)malloc(objsz_t.byte1);
   
   if(obj.obj->arrayobj->generic == NULL)
       Exception("Array object could not be created.", "OutOfMemoryError");
  }
  if(_typedef_ == TYPEDEF_STRING_ARRAY) {
   obj.obj = new (nothrow) DataObject[1];
   obj.obj->arrayobj = new (nothrow) ArrayObject[1];
   
   if(obj.obj == nullptr || obj.obj->arrayobj == nullptr)
       Exception("String array object could not be created.", "OutOfMemoryError");
    
   obj.obj->arrayobj->length = objsz_t.byte1;
   obj.obj->arrayobj->strobj = (StringObject*)malloc(objsz_t.byte1);
   
   if(obj.obj->arrayobj->strobj == NULL)
       Exception("String array object could not be created.", "OutOfMemoryError");
   
   for(long i = 0; i < objsz_t.byte1; i++){ // initalize all strings
      obj.obj->arrayobj->strobj[i].array = new (nothrow) ArrayObject[1];
      
      if(obj.obj->arrayobj->strobj[i].array == nullptr)
        Exception("String array object could not be created.", "OutOfMemoryError");
   }
      
  }
   
  obj.size_t.byte1 = objsz_t.byte1;
}


long jmpLocation(Method &m){
    return m.ref.byte2;
}

long returnLocation(Method &m){
    return m.ref.byte1;
}
  
void svmSetGenericValue(Object &obj, double value){
    if(svmObjectIsDead(obj))
      Exception("Object was not initalized.", "DeadObjectException");
   
    obj.obj->generic = value;
}

double svmGetGenericValue(Object &obj){
    if(svmObjectIsDead(obj))
      Exception("Object was not initalized.", "DeadObjectException");
   
    return obj.obj->generic;
}


long __typedef(Object &obj){
    return obj.instanceData.byte1;
} 

long __gcstatus(Object &obj){
    return obj.instanceData.byte2;
}

bool isObjArray(Object &obj){
    return (__typedef(obj) == TYPEDEF_GENERIC_ARRAY || __typedef(obj) == TYPEDEF_STRING_ARRAY);
}

void freeObj(Object &obj){
    if(!svmObjectIsDead(obj))
       return;
       
    free(obj.obj);
    obj.instanceData.byte2 = GC_IDLE;
    obj.size_t.byte1 = 0;
    obj.instanceData.byte1 = 0;
    obj.init.byte1 = OBJECT_DEAD;
}

// We perform a shallow delete
void svmDumpObject(Object &obj){
  gc_objc++;
  obj.size_t.byte1 = 0;
  obj.instanceData.byte1 = 0;
  obj.init.byte1 = OBJECT_DEAD;
  obj.instanceData.byte2 = GC_DIRTY;
}
