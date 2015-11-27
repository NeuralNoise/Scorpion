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
 #include "../arraylist.h"
 #include <string>
 #include <sstream>
 
 using namespace std;
 
 #define nil NULL
 #define obj_offset 50 // The offset address of all Object data sets
 
 #define var_return 0x0
 #define var_null   0x1
 #define var_true   0x2
 #define var_false  0x3
 
 extern bool init;
 
 extern unsigned long cplr_item_buflen;
 extern unsigned long cplr_bitmap_len;
 
 // TODO: create standard memory structure variables
 
 struct cmplr_item { /* Data structure for holding buffer items. */
     u1_d item;
     
     u1 size_t;
     
     string* str;
     
     struct u1_d* sub_item;
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

 extern unsigned long cplr_item_sz1;

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
 
 
 // *********************[[ Compiler flag id's ]]**********************
 #define cplr_method (0x7f)
 #define cplr_instr (0x5a)
 #define cplr_string_instr (0x3b)
 #define cplr_out_instr (0x2c)
 
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
   * initalized & its address.
   */
   u2 init;  
     
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
   
   string symbol, m, klazz, stype; // the name of our object and the module it belongs to
   
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
    
    
    ListAdapter<string> varlist;
    ListAdapter<int> vartypes;
    ListAdapter<int> pointertype;
    
    /*
    * Wether or not this method is
    * native
    */
    bool native;
    
    u1 address;
  
 };
 
  /*
 * Scorpion Garbage Collector
 *
 * The GC Marks Objects with 1 of 3 status
 *
 * GC_DIRTY :       This is when an Object has has a "shallow delete".
 *
 * GC_IDLE :        This is when an Object has been removed by the GC and is waiting to be re-instantiated;
 *
 * GC_CLEAN :       This is when an object is currently in use and should NOT be touched by the GC.
 *
 * The GC gets activated only after the number of Objects with the status GC_DIRTY passes the limit of
 * dirty objects. Once the GC activated, each dirty Object is invalidated.
 *
 * GC Terms
 *
 * - Shallow Delete
 *       A "shallow delete" is when an Object has been identified non-initalized, but 
 *     hasn't been offically deleted yet. This is done to not slow down the overall 
 *     performance of the virtual machine and to allow the GC to remove it later.
 */
 #define GC_DIRTY (0x472)
 #define GC_CLEAN (0x482)
 #define GC_IDLE (0x302)
 
 bool svmObjectIsAlive(Object obj);

 bool svmObjectHasInstance(Object obj, int instance);

 void svmInitHeapObject(int _typedef_, u1 objsz_t, string name, string m, string klazz, string stype, int gc_status);
 
 bool findObjectByDescriptor(string desc);
 bool findObjectByClassDescriptor(string desc, string klazz);
 bool findSpecificObjectByDescriptor(string module, string desc);
 bool findSpecificMethodByDescriptor(string module, string desc);
 unsigned long findSpecificObjectAddressByDescriptor(string module, string desc);
 unsigned long findActualObjectAddressByDescriptor(string module, string desc);
 unsigned long findActualMethodAddressByDescriptor(string module, string desc);
 unsigned long findSpecificMethodAddressByDescriptor(string module, string desc);
 
/*
 * Properly initialize an Object.
 * void SVM_OBJECT_INIT(int _typedef, u1 sz, string name, string module, string class)
 */
 #define SVM_OBJECT_INIT(_tdef, sz, name, m, k, s_type) \
     svmInitHeapObject(_tdef, sz, name, m, k, s_type, GC_CLEAN)

 bool isObjArray(Object obj);

 void freeObj(long adr);
 
 /* Dynamic memory */
 extern ListAdapter<Object> memoryObjs;
 extern ListAdapter<Method> staticMethods;
 
 #endif // _COMPILR_CORE
 