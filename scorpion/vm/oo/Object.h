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
 #include <stdint.h>
 
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
 
 /**
 * Init values
 * 
 * OBJECT_DEAD 0x0      when an object has not been instantiated
 * OBJECT_ALIVE 0x1     when an object has been instantiated
 */
 #define OBJECT_DEAD (0x0)
 #define OBJECT_ALIVE (0x129)
 
 /**
 * Typedef values
 * 
 * These are used for object instantiation
 */
 #define TYPEDEF_STRING (0x12)
 #define TYPEDEF_STRING_ARRAY (0x14)
 
  typedef int8_t  sbyte;  
  typedef int16_t sshort;
  typedef int16_t schar;
  typedef int32_t sint;
  typedef int64_t slong;
  typedef float   sfloat;
  typedef double  sdouble;
  typedef bool    sbool;
 
 /**
 * 8-Bit signed integer
 *   This data type has a minimum value of -128 and a maximum value of 127
 *   Using this data type is great for saving space upon creation of huge array 
 *   and data structures that occupy large pieces of memory
 */
 #define TYPEDEF_GENERIC_BYTE (0x32)
 
 /**
 * 16-Bit signed integer
 *  This data type is a "little" integer, containing a minimum value of -32,768
 *  and a max value of 32,767. This is also a nother good data type to use in large 
 *  arrays to save memory consumption.
 */
 #define TYPEDEF_GENERIC_SHORT (0x34)
 
 /**
 * 32-Bit signed integer
 *  This data type is a "medium" integer, containing a minimum value of -2^31 and
 *  a maximum value of 2^31. This data type is probably the most used type of data 
 *  because its range is in the "middle" and can be used for most data processing
 */
 #define TYPEDEF_GENERIC_INT (0x36)
 
 /**
  * 64-Bit signed integer
  *  This data type is a "big" integer, containing a mimimum value of -2^63 and
  *  a maximum value of 2^63. This data type is the largest integer type that 
  *  hold data in Scorpion. Use this data type when you need a range of values 
  *  much wider than those provided by int
  */
 #define TYPEDEF_GENERIC_LONG (0x38)
 
 /**
  * 32-bit signed decimal
  *  This data type is used for small and mid sized decimal calculations.
  *  Use this when you wand to parform fractional data calculations. This 
  *  data type has a decimal precision(digits behind the decimal number)
  *  of up to 7 digits.
  */
 #define TYPEDEF_GENERIC_FLOAT (0x40)
 
 /**
  * 64-Bit signed decimal
  *  This data type is float's counterpar in performing fractional calculations.
  *  Use this data type when you need more percise decimal calculations. This 
  *  data type has a decimal precision(digits behind the decimal number)
  *  of up to 16 digits.
  */
 #define TYPEDEF_GENERIC_DOUBLE (0x42)
 
 /**
  * 16-Bit signed integer
  *  This data type is very similar to the type short, except this 
  *  data type is used for text creation and manipulation. This data
  *  type contains a minimum value of -32,768 and a max value of 32,767.
  */
 #define TYPEDEF_GENERIC_CHAR (0x46)
 
 /**
  * Undefined-Bit value
  *  This data type is a very trivial data type seen in almost all programming languages.
  *  It operates as a single bit that can only hold the values true or false (i.e 1 or 0)
  *  Its size is left undefined.
  */
 #define TYPEDEF_GENERIC_BOOL (0x48)
 
 /*
  * Scorpion primitive type array type
  * definitions
  */
 #define TYPEDEF_GENERIC_BYTE_ARRAY (0x50)
 #define TYPEDEF_GENERIC_SHORT_ARRAY (0x52)
 #define TYPEDEF_GENERIC_INT_ARRAY (0x54)
 #define TYPEDEF_GENERIC_LONG_ARRAY (0x56)
 #define TYPEDEF_GENERIC_FLOAT_ARRAY (0x58)
 #define TYPEDEF_GENERIC_DOUBLE_ARRAY (0x60)
 #define TYPEDEF_GENERIC_BOOL_ARRAY (0x62)
 #define TYPEDEF_GENERIC_CHAR_ARRAY (0x64)

 struct DataObject {
 
     /* These are our special object types */    
     StringObject* strobj;
     ArrayObject* arrayobj;
     
     /*
     * native Scorpion primitive types
     */
     sbyte   *pbyte;  
     sshort  *pshort;
     schar   *pchar;
     sint    *pint;
     slong   *plong;
     sfloat  *pfloat;
     sdouble *pdouble;
     sbool   *pboolean;
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
 *                              - An array of generic data
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
   
   /* This is a special type of object, it simply holds an array of nodes */
   Object* nodeobj;
     
   /*
    * native Scorpion primitive types
    */
     sbyte   *pbyte;  
     sshort  *pshort;
     schar   *pchar;
     sint    *pint;
     slong   *plong;
     sfloat  *pfloat;
     sdouble *pdouble;
     sbool   *pboolean;
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

bool svmObjectIsAlive(Object &obj);

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

void svmIncGenericValue(Object &obj);

double svmDecGenericValue(Object &obj);

long __typedef(Object &obj);

long __gcstatus(Object &obj);

bool isObjArray(Object &obj);

bool isgeneric(int _typedef_);
bool isgenericarray(int _typedef_);

void freeObj(Object &obj);
 
#endif // SCORPION_NATIVE_OBJECT
