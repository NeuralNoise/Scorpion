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
 #ifndef SCORPION_ALLOCATION_SCHEME
 #define SCORPION_ALLOCATION_SCHEME
 
 #include "../../clib/u1.h"
 #include "../../clib/u2.h"
 #include "../../clib/arraylist.h"
 #include <string>
 #include <stdint.h>
 
 using namespace std;
  
  /**
  * 8-Bit signed integer
  *   This data type has a minimum value of -128 and a maximum value of 127
  *   Using this data type is great for saving space upon creation of huge array 
  *   and data structures that occupy large pieces of memory
  */
  typedef int8_t sbyte;  
  
  /**
  * 16-Bit signed integer
  *  This data type is a "little" integer, containing a minimum value of -32,768
  *  and a max value of 32,767. This is also a nother good data type to use in large 
  *  arrays to save memory consumption.
  */
  typedef int16_t sshort;
  
  /**
  * 16-Bit signed integer
  *  This data type is very similar to the type short, except this 
  *  data type is used for text creation and manipulation. This data
  *  type contains a minimum value of -32,768 and a max value of 32,767.
  */
  typedef int16_t schar;
  
  /**
  * 32-Bit signed integer
  *  This data type is a "medium" integer, containing a minimum value of -2^31 and
  *  a maximum value of 2^31. This data type is probably the most used type of data 
  *  because its range is in the "middle" and can be used for most data processing
  */
  typedef int32_t sint;
  
  /**
  * 64-Bit signed integer
  *  This data type is a "big" integer, containing a mimimum value of -2^63 and
  *  a maximum value of 2^63. This data type is the largest integer type that 
  *  hold data in Scorpion. Use this data type when you need a range of values 
  *  much wider than those provided by int
  */
  typedef int64_t slong;

  /**
  * 32-bit signed decimal
  *  This data type is used for small and mid sized decimal calculations.
  *  Use this when you wand to parform fractional data calculations. This 
  *  data type has a decimal precision(digits behind the decimal number)
  *  of up to 7 digits.
  */
  typedef float sfloat;
  
  /**
  * 64-Bit signed decimal
  *  This data type is float's counterpar in performing fractional calculations.
  *  Use this data type when you need more percise decimal calculations. This 
  *  data type has a decimal precision(digits behind the decimal number)
  *  of up to 16 digits.
  */
  typedef double sdouble;
  
  /**
  * Undefined-Bit value
  *  This data type is a very trivial data type seen in almost all programming languages.
  *  It operates as a single bit that can only hold the values true or false (i.e 1 or 0)
  *  Its size is left undefined.
  */
  typedef bool sbool;
  
 namespace scorpionvm
 {
     namespace memory
     {
         namespace schema
         {
             template <class T>
             class AllocationScheme
             {
                 public:
                  T* malloc(unsigned long N, unsigned long size_t);
                  T* calloc(unsigned long N, unsigned long size_t);
                  int free(T* ptr);
             };
    
             struct ObjectSchema
             {
                 enum t {SBYTE=0, SSHORT=1, SCHAR=2, SINT=3, 
                      SLONG=4, SFLOAT=5, SDOUBLE=6, SBOOL=7, SSTRING=8} type;
                 union
                 {
                     sbyte   byte;  
                     sshort  short_;
                     schar   char_;
                     sint    int_;
                     slong   long_;
                     sfloat  float_;
                     sdouble double_;
                     sbool   boolean;
                 };
             };
         } // end schemea
     } // end memory
 } // end scorpionvm
 
 #endif // SCORPION_ALLOCATION_SCHEME
 