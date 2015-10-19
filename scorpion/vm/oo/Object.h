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
 #ifndef SCORPION_NATIVE_OBJECT
 #define SCORPION_NATIVE_OBJECT
 
 struct StringObject;
 struct ArrayObject;
 struct Method;
 
 #include "../alloc/gc.h"
 #include "../../clib/u1.h"
 #include "../../clib/u2.h"
 #include "../alloc/HeapBitmap.h"
 #include "../alloc/gc.h"
 #include <string>
 
 using namespace std;
 
  #define nullptr ((void *)0)
  
 /*
 * we use this for assigning 
 * values with default locations
 */
 #define default_loc (0)
 
 /*
 * Instance data variables
 *
 * When creating a new instance of data,
 * we need to set both the (typedef) and the (gc_self)
 * data sets to provide the vm with information on how 
 * to preocess each object.
 *
 */
 #define _typedef (0x0)
 #define gc_self (0x1)
 
 /*
 * Init values
 * 
 * OBJECT_DEAD 0x0      when an object has not been instantiated
 * OBJECT_ALIVE 0x1     when an object has been instantiated
 */
 #define OBJECT_DEAD (0x0)
 #define OBJECT_ALIVE (0x129)
 
 /*
 * Typedef values
 * 
 * These are used for object instantiation
 */
 #define TYPEDEF_STRING (0x12)
 #define TYPEDEF_GENERIC_ARRAY (0x13)
 #define TYPEDEF_STRING_ARRAY (0x14)
 #define TYPEDEF_GENERIC (0x15)
 
 struct DataObject {
 
     /* These are our special object types */    
     StringObject* strobj;
     ArrayObject* arrayobj;
     
     /*
     * Generic data (int, float, etc..)
     */
     double generic;
 };
 
 /*
 * In Scorpion, all object must be instantiated before
 * use.
 *
 * Objects in Scorpion down to its lowest possible entity
 * are all double. Objects are abstract containers that 
 * can hold 1 of 4 different typedefs at any given time.
 *
 * A typedef in Scorpion is the primive "type" of a defined object.
 * Each typedef has its own rules that must be followed to avoid an exception.
 * There are 4 main types of typedefs:
 * 
 * typedef-string ::=   A string object
 *                        - String Objects are simply a char[] of data which 
 *                          represents the string.
 *
 * typedef-generic ::=  A "generic" object
 *                        - Generic data in Scorpion represents any non-specific piece of 
 *                          data that is used in the Scorpion.
 *
 * typedef-generic-array ::=  A "generic" array object
 *                              - An array og generic data
 *
 * typedef-string-array  ::=  A string array object
 *                              - An array of string objects
 *
 */
 struct Object {
  
   /*
   * Wether or not the object is 
   * initalized.
   */
   u1 init;  
     
   /*
   * This will hold the important information 
   *
   * byte1 = _typedef_
   * byte2 = gc_status
   */
   u2 instanceData;
    
   /*
   * This is the actual data 
   */
   DataObject* obj;    
   
   /* The size of our object */
   u1 size_t;
 };
 
 #define STR_LIMIT (65535)
 
 /*
 * Strings are used frequently enough that we may want to give them their
 * own unique type.
 *
 * Currently this is just equal to a array of DataObjects, and we pull the fields out
 * like we do for any other object.
 * 
 * Creating this type prevents data from stepping over each others toes, while still keeping 
 * the memory access time to similar or equal speeds.
 */
 struct StringObject {
   
    /** Returns this string's length in characters. */
    unsigned int length;


    /** Returns this string's char[] as an ArrayObject. */
    ArrayObject* array;
 };
 
 struct ArrayObject {
   
   /* The length of our object array */
   unsigned long length;
   
   /* These are our special object types */    
   StringObject* strobj;
     
   /*
   * Generic data (**int, **float, etc..)
   */
   double *generic;
 };

 /*
 * Methods in Scorpion are forms of runnable objects
 * methods and lables are the only things that can directly
 * modify memory placement.
 * 
 * This maintains the structure and flow of the program
 */
 struct Method {
    string clazz, module, name;
  
    /* 
    * This holds 2 key data dets
    *
    * 1. The return location
    * 2. The jump location
    *
    * When calling a method, we jump to the 
    * preprocessed location to which the method
    * has been assigned. 
    *
    * The return location well also be set for 
    * returning back to the position it was called.
    */
    u2 ref;
    
    /*
    * Wether or not this method is
    * native
    */
    bool native;
  
 };

  long jmpLocation(Method &m);
  long returnLocation(Method &m);

bool svmObjectIsDead(Object &obj);

bool svmObjectHasInstance(Object &obj, int instance);

void svmInitHeapObject(Object &obj, int _typedef_, u1 objsz_t, int gc_status);
 
/*
 * Properly initialize an Object.
 * void SVM_OBJECT_INIT(Object** obj, long location, int _typedef, u1 sz)
 */
#define SVM_OBJECT_INIT(obj, _tdef, sz) \
    svmInitHeapObject(obj, _tdef, sz, GC_CLEAN)

/* debugging */
void svmDumpObject(Object &obj); // wait for GC cleanup

void svmSetGenericValue(Object &obj, double value);

double svmGetGenericValue(Object &obj);

long __typedef(Object &obj);

long __gcstatus(Object &obj);
 
#endif // SCORPION_NATIVE_OBJECT