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
 #include <stdint.h>
 
 using namespace std;

bool svmObjectIsAlive(Object &obj){
   return (obj.init.byte1 == OBJECT_ALIVE);
}

bool svmObjectHasInstance(Object &obj, int instance){
   return (obj.instanceData.byte1 == instance);
}

// TODO: add err() function to free object before throwing exception
void svmInitHeapObject(Object &obj, int _typedef_, u1 objsz_t, int gc_status){
  
  if(svmObjectIsAlive(obj))
    return;
  
  obj.init.byte1 = OBJECT_ALIVE;
  obj.instanceData.byte1 = _typedef_;
  obj.instanceData.byte2 = gc_status;
  
  if(isgeneric(_typedef_)){
   obj.obj = new (nothrow) DataObject[1];
   
   if(obj.obj == nullptr)
      Exception("Object could not be created.", "OutOfMemoryError");

   if(_typedef_ == TYPEDEF_GENERIC_BYTE){
      obj.obj->pbyte = new (nothrow)  int8_t[1];
      
      if(obj.obj->arrayobj->pbyte == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_SHORT){
      obj.obj->pshort = new (nothrow)  int16_t[1];
      
      if(obj.obj->arrayobj->pshort == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_INT){
      obj.obj->pint = new (nothrow)  int32_t[1];
      
      if(obj.obj->arrayobj->pint == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_LONG){
      obj.obj->plong = new (nothrow)  int64_t[1];
      
      if(obj.obj->arrayobj->plong == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_CHAR){
      obj.obj->pchar = new (nothrow)  int16_t[1];
      
      if(obj.obj->arrayobj->pchar == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_BOOL){
      obj.obj->pboolean = new (nothrow)  bool[1];
      
      if(obj.obj->arrayobj->pboolean == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_FLOAT){
      obj.obj->pfloat = new (nothrow)  float[1];
      
      if(obj.obj->arrayobj->pfloat == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_DOUBLE){
      obj.obj->pdouble = new (nothrow)  double[1];
      
      if(obj.obj->arrayobj->pdouble == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
  }
  if(_typedef_ == TYPEDEF_STRING){
   obj.obj = new (nothrow) DataObject[1];
   obj.obj->strobj = new (nothrow) StringObject[1];
   obj.obj->strobj->array = new (nothrow) ArrayObject[1];
   
   if(obj.obj == nullptr || obj.obj->strobj == nullptr 
      || obj.obj->strobj->array == nullptr)
       Exception("String object could not be created.", "OutOfMemoryError");
  }
  if(isgenericarray(_typedef_)) {
   obj.obj = new (nothrow) DataObject[1];
   obj.obj->arrayobj = new (nothrow) ArrayObject[1];
   
   if(obj.obj == nullptr || obj.obj->arrayobj == nullptr)
       Exception("Array object could not be created.", "OutOfMemoryError");
   
   obj.obj->arrayobj->length = objsz_t.byte1;
   
   if(_typedef_ == TYPEDEF_GENERIC_BYTE_ARRAY){
      obj.obj->arrayobj->pbyte =  (int8_t*)malloc(objsz_t.byte1);
      
      if(obj.obj->arrayobj->pbyte == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_SHORT_ARRAY){
      obj.obj->arrayobj->pshort = (int16_t*)malloc(objsz_t.byte1);
      
      if(obj.obj->arrayobj->pshort == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_INT_ARRAY){
      obj.obj->arrayobj->pint = (int32_t*)malloc(objsz_t.byte1);
      
      if(obj.obj->arrayobj->pint == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_LONG_ARRAY){
      obj.obj->arrayobj->plong = (int64_t*)malloc(objsz_t.byte1);
      
      if(obj.obj->arrayobj->plong == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_CHAR_ARRAY){
      obj.obj->arrayobj->pchar = (int16_t*)malloc(objsz_t.byte1);
      
      if(obj.obj->arrayobj->pchar == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_BOOL_ARRAY){
      obj.obj->arrayobj->pboolean = (bool*)malloc(objsz_t.byte1);
      
      if(obj.obj->arrayobj->pboolean == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_FLOAT_ARRAY){
      obj.obj->arrayobj->pfloat = (float*)malloc(objsz_t.byte1);
      
      if(obj.obj->arrayobj->pfloat == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
   else if(_typedef_ == TYPEDEF_GENERIC_DOUBLE_ARRAY){
      obj.obj->arrayobj->pdouble = (double*)malloc(objsz_t.byte1);
      
      if(obj.obj->arrayobj->pdouble == NULL)
          Exception("Array object could not be created.", "OutOfMemoryError");
   }
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
    if(!svmObjectIsAlive(obj))
      Exception("Object was not initalized.", "DeadObjectException");
   
    if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BYTE)
       obj.obj->pbyte[default_loc] = value;
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_SHORT)
       obj.obj->pshort[default_loc] = value;
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_INT)
       obj.obj->pint[default_loc] = value;
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_LONG)
       obj.obj->plong[default_loc] = value;
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_FLOAT)
       obj.obj->pfloat[default_loc] = value;
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_CHAR)
       obj.obj->pchar[default_loc] = value;
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BOOL)
       obj.obj->pboolean[default_loc] = value;
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_DOUBLE)
       obj.obj->pdouble[default_loc] = value;
}

double svmGetGenericValue(Object &obj){
    if(!svmObjectIsAlive(obj))
      Exception("Object was not initalized.", "DeadObjectException");
   
    
    if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BYTE)
       return (double) obj.obj->pbyte[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_SHORT)
       return (double) obj.obj->pshort[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_INT)
       return (double) obj.obj->pint[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_LONG)
       return (double) obj.obj->plong[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_FLOAT)
       return (double) obj.obj->pfloat[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_CHAR)
       return (double) obj.obj->pchar[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BOOL)
       return (double) obj.obj->pboolean[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_DOUBLE)
       return (double) obj.obj->pdouble[default_loc];
}

void svmIncGenericValue(Object &obj){
    if(!svmObjectIsAlive(obj))
      Exception("Object was not initalized.", "DeadObjectException");
   
    
    if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BYTE)
       ++obj.obj->pbyte[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_SHORT)
       ++obj.obj->pshort[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_INT)
       ++obj.obj->pint[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_LONG)
       ++obj.obj->plong[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_FLOAT)
       ++obj.obj->pfloat[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_CHAR)
       ++obj.obj->pchar[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BOOL)
       ++obj.obj->pboolean[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_DOUBLE)
       ++obj.obj->pdouble[default_loc];
}

double svmDecGenericValue(Object &obj){
    if(!svmObjectIsAlive(obj))
      Exception("Object was not initalized.", "DeadObjectException");
   
    if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BYTE)
       --obj.obj->pbyte[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_SHORT)
       --obj.obj->pshort[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_INT)
       --obj.obj->pint[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_LONG)
       --obj.obj->plong[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_FLOAT)
       --obj.obj->pfloat[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_CHAR)
       --obj.obj->pchar[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_DOUBLE)
       --obj.obj->pdouble[default_loc];
}

long __typedef(Object &obj){
    return obj.instanceData.byte1;
} 

long __gcstatus(Object &obj){
    return obj.instanceData.byte2;
}

bool isObjArray(Object &obj){
    return (isgenericarray(__typedef(obj)) || __typedef(obj) == TYPEDEF_STRING_ARRAY);
}

void freeObj(Object &obj){
    if(!svmObjectIsAlive(obj))
       return;
       
    free(obj.obj);
    obj.instanceData.byte2 = GC_IDLE;
    obj.size_t.byte1 = 0;
    obj.instanceData.byte1 = 0;
    obj.init.byte1 = OBJECT_DEAD;
}

bool isgeneric(int _typedef_)
{
    return (_typedef_ == TYPEDEF_GENERIC_BYTE || _typedef_ == TYPEDEF_GENERIC_SHORT 
     || _typedef_ == TYPEDEF_GENERIC_INT || _typedef_ == TYPEDEF_GENERIC_LONG
     || _typedef_ == TYPEDEF_GENERIC_CHAR || _typedef_ == TYPEDEF_GENERIC_BOOL
     || _typedef_ == TYPEDEF_GENERIC_FLOAT || _typedef_ == TYPEDEF_GENERIC_DOUBLE);
}

bool isgenericarray(int _typedef_)
{
    return (_typedef_ == TYPEDEF_GENERIC_BYTE_ARRAY || _typedef_ == TYPEDEF_GENERIC_SHORT_ARRAY 
     || _typedef_ == TYPEDEF_GENERIC_INT_ARRAY || _typedef_ == TYPEDEF_GENERIC_LONG_ARRAY
     || _typedef_ == TYPEDEF_GENERIC_CHAR_ARRAY || _typedef_ == TYPEDEF_GENERIC_BOOL_ARRAY
     || _typedef_ == TYPEDEF_GENERIC_FLOAT_ARRAY || _typedef_ == TYPEDEF_GENERIC_DOUBLE_ARRAY);
}

// We perform a soft(shallow) delete
void svmDumpObject(Object &obj){
  gc_objc++;
  obj.size_t.byte1 = 0;
  obj.instanceData.byte1 = 0;
  obj.init.byte1 = OBJECT_DEAD;
  obj.instanceData.byte2 = GC_DIRTY;
}
