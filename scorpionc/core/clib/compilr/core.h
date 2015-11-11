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
 * A small portion of the software depicted in this library goes to 
 * the creator of C LTL(C++ Lexer Toolkit Library)
 *
 * (http://www.partow.net/programming/lexertk/index.html) 2001
 * Author - Arash Partow
 */
 #ifndef _COMPILR_CORE
 #define _COMPILR_CORE
 
 #include "../u1.h"
 #include "../u2.h"
 #include "../u4.h"
 #include <string>
 #include <sstream>
 
 using namespace std;
 
 #define nil NULL
 
 extern bool init;
 
 extern unsigned long cplr_item_buflen;
 extern unsigned long cplr_bitmap_len;
 
 // TODO: create standard memory structure variables
 
 struct cmplr_item { /* Data structure for holding buffer items. */
     u1_d item;
     
     u1 size_t;
     
     string* str;
     
     struct cmplr_item* sub_item;
 } ;
 
 struct cmplr_item_2 { /* Data structure for holding buffer items with index. */
     int j;
     cmplr_item* c_items;
 } ;
 
 extern struct cmplr_item *new_cmplr_item(u2 init, double* data, string str);
 
 void cmplrfree( struct cmplr_item *tmppt );


 extern struct cmplr_item_2 *new_cmplr_item2();

 void cmplrfree2( struct cmplr_item_2 *tmppt );
 
 void cmplr_cleanup(bool fullFlush);


 extern struct cmplr_item *cplrfreelist1;
 
 extern struct cmplr_item_2 *cplrfreelist2;

/*-----------------------*/
/* Initalize the Compilr library */
/*  This will be called automatically */
/*  upon calling any cmplr routines. */
/*-----------------------*/
 int cmplr_init();

/*-----------------------*/
/* Add item to a buffer. */
/*-----------------------*/
 int cmplr_add_item( struct cmplr_item *b2 );
 
 /*--------------------------------*/
 /* Scorpion memory structure nodes                          */
 /*  These struct nodes will be used for initalizing         */
 /*  and keeping tract of all data in the object file.       */
 /*  The memory structures closely mimmic the                */
 /*  memory structure found in the Scorpion vitual machine.  */
 /*--------------------------------*/
 
 struct StringObject;
 struct ArrayObject;
 struct Method;
 
 #include "../u1.h"
 #include "../u2.h"
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
   
   string symbol; // the name of our object
   
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
 
 Object findObjectByDescriptor(string desc);
 
/*
 * Properly initialize an Object.
 * void SVM_OBJECT_INIT(Object** obj, long location, int _typedef, u1 sz)
 */
 #define SVM_OBJECT_INIT(obj, _tdef, sz) \
     svmInitHeapObject(obj, _tdef, sz, GC_CLEAN)

/* debugging */
 void svmDumpObject(Object &obj); // wait for GC cleanup
 // We still free this object after calling the routine since the compiler has no GC

 bool isObjArray(Object &obj);

 void freeObj(Object &obj);
 
 #endif // _COMPILR_CORE
 