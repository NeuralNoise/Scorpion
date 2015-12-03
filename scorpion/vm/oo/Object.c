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

void init_err(Object &obj, string message)
{
    freeObj(obj);
    Exception(message, "OutOfMemoryError");
}

void svmInitHeapObject(Object &obj, int _typedef_, u1 objsz_t, int gc_status){
  if(svmObjectIsAlive(obj))
    return;
  
  obj.init.byte1 = OBJECT_ALIVE;
  obj.instanceData.byte1 = _typedef_;
  obj.instanceData.byte2 = gc_status;
  obj.size_t = objsz_t;
  
  if(isgeneric(_typedef_)){
       obj.obj = new (nothrow) DataObject[1];
       
       if(obj.obj == nullptr)
          init_err(obj, "Object could not be created.");
    
       if(_typedef_ == TYPEDEF_GENERIC_BYTE){
          obj.obj->pbyte = new (nothrow)  sbyte[1];
          
          if(obj.obj->pbyte == nullptr)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_SHORT){
          obj.obj->pshort = new (nothrow)  sshort[1];
          
          if(obj.obj->pshort == nullptr)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_INT){
          obj.obj->pint = new (nothrow)  sint[1];
          
          if(obj.obj->pint == nullptr)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_LONG){
          obj.obj->plong = new (nothrow)  slong[1];
          
          if(obj.obj->plong == nullptr)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_CHAR){
          obj.obj->pchar = new (nothrow)  schar[1];
          
          if(obj.obj->pchar == nullptr)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_BOOL){
          obj.obj->pboolean = new (nothrow)  sbool[1];
          
          if(obj.obj->pboolean == nullptr)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_FLOAT){
          obj.obj->pfloat = new (nothrow)  sfloat[1];
          
          if(obj.obj->pfloat == nullptr)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_DOUBLE){
          obj.obj->pdouble = new (nothrow)  sdouble[1];
          
          if(obj.obj->pdouble == nullptr)
              init_err(obj, "Array object could not be created.");
       }
       return;
  }
  if(_typedef_ == TYPEDEF_STRING){
       obj.obj = new (nothrow) DataObject[1];
       obj.obj->strobj = new (nothrow) StringObject[1];
       obj.obj->strobj->array = new (nothrow) ArrayObject[1];
       
       if(obj.obj == nullptr || obj.obj->strobj == nullptr 
          || obj.obj->strobj->array == nullptr)
           init_err(obj, "String object could not be created.");
     return;
  }
  if(isgenericarray(_typedef_)) {
       obj.obj = new (nothrow) DataObject[1];
       obj.obj->arrayobj = new (nothrow) ArrayObject[1];
       
       if(obj.obj == nullptr || obj.obj->arrayobj == nullptr)
           init_err(obj, "Array object could not be created.");
       
       obj.obj->arrayobj->length = objsz_t.byte1;
       
       if(_typedef_ == TYPEDEF_GENERIC_BYTE_ARRAY){
          obj.obj->arrayobj->pbyte =  (sbyte*)malloc(objsz_t.byte1);
          
          if(obj.obj->arrayobj->pbyte == NULL)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_SHORT_ARRAY){
          obj.obj->arrayobj->pshort = (sshort*)malloc(objsz_t.byte1);
          
          if(obj.obj->arrayobj->pshort == NULL)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_INT_ARRAY){
          obj.obj->arrayobj->pint = (sint*)malloc(objsz_t.byte1);
          
          if(obj.obj->arrayobj->pint == NULL)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_LONG_ARRAY){
          obj.obj->arrayobj->plong = (slong*)malloc(objsz_t.byte1);
          
          if(obj.obj->arrayobj->plong == NULL)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_CHAR_ARRAY){
          obj.obj->arrayobj->pchar = (schar*)malloc(objsz_t.byte1);
          
          if(obj.obj->arrayobj->pchar == NULL)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_BOOL_ARRAY){
          obj.obj->arrayobj->pboolean = (sbool*)malloc(objsz_t.byte1);
          
          if(obj.obj->arrayobj->pboolean == NULL)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_FLOAT_ARRAY){
          obj.obj->arrayobj->pfloat = (sfloat*)malloc(objsz_t.byte1);
          
          if(obj.obj->arrayobj->pfloat == NULL)
              init_err(obj, "Array object could not be created.");
       }
       else if(_typedef_ == TYPEDEF_GENERIC_DOUBLE_ARRAY){
          obj.obj->arrayobj->pdouble = (sdouble*)malloc(objsz_t.byte1);
          
          if(obj.obj->arrayobj->pdouble == NULL)
              init_err(obj, "Array object could not be created.");
       }
       return;
  }
  if(_typedef_ == TYPEDEF_STRING_ARRAY) {
       obj.obj = new (nothrow) DataObject[1];
       obj.obj->arrayobj = new (nothrow) ArrayObject[1];
       
       if(obj.obj == nullptr || obj.obj->arrayobj == nullptr)
           init_err(obj, "String array object could not be created.");
        
       obj.obj->arrayobj->length = objsz_t.byte1;
       obj.obj->arrayobj->strobj = (StringObject*)malloc(objsz_t.byte1);
       
       if(obj.obj->arrayobj->strobj == NULL)
           init_err(obj, "String array object could not be created.");
       
       for(long i = 0; i < objsz_t.byte1; i++){ // initalize all strings
          obj.obj->arrayobj->strobj[i].array = new (nothrow) ArrayObject[1];
          
          if(obj.obj->arrayobj->strobj[i].array == nullptr)
            init_err(obj, "String array object could not be created.");
       }
      return;
  }
}


long jmpLocation(Method &m){
    return m.ref.byte2;
}

long returnLocation(Method &m){
    return m.ref.byte1;
}

float  btof(bool b)
{
    if(b)
      return 1;
    else return 0;
}

void svmSetGenericValue(Object &obj, double value){
    if(!svmObjectIsAlive(obj))
      Exception("Object was not initalized.", "DeadObjectException");
   
    if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BYTE)
       { obj.obj->pbyte[default_loc] = value; return; }
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_SHORT)
       { obj.obj->pshort[default_loc] = value; return; }
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_INT)
       { obj.obj->pint[default_loc] = value; return; }
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_LONG)
       { obj.obj->plong[default_loc] = value; return; }
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_FLOAT)
       { obj.obj->pfloat[default_loc] = value; return; }
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_CHAR)
       { obj.obj->pchar[default_loc] = value; return; }
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BOOL)
       { obj.obj->pboolean[default_loc] = (bool) value; return; }
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_DOUBLE)
       { obj.obj->pdouble[default_loc] = value; return; }
}

double svmGetGenericValue(Object &obj){
    if(!svmObjectIsAlive(obj))
      Exception("Object was not initalized.", "DeadObjectException");
   
    
    if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BYTE)
       return obj.obj->pbyte[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_SHORT)
       return obj.obj->pshort[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_INT)
       return obj.obj->pint[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_LONG)
       return obj.obj->plong[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_FLOAT)
       return obj.obj->pfloat[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_CHAR)
       return obj.obj->pchar[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BOOL)
       return obj.obj->pboolean[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_DOUBLE)
       return obj.obj->pdouble[default_loc];
}

void svmIncGenericValue(Object &obj){
    if(!svmObjectIsAlive(obj))
      Exception("Object was not initalized.", "DeadObjectException");
   
    
    if(obj.instanceData.byte1 == TYPEDEF_GENERIC_BYTE)
       ++obj.obj->pbyte[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_SHORT)
       ++obj.obj->pshort[default_loc];
    else if(obj.instanceData.byte1 == TYPEDEF_GENERIC_INT)
       { ++obj.obj->pint[default_loc]; return; }
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
