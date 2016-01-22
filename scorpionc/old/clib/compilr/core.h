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
 
 #define var_return      0x0
 #define var_null        0x1
 #define var_true        0x2
 #define var_false       0x3
 #define var_dummy_data  0x4
 
 extern bool init;
 
 // TODO: create standard memory structure variables
 
 struct cmplr_item { /* Data structure for holding buffer items. */
     u1_d item;
     
     u1 size_t;
     
     string str;
     
     ListAdapter<struct cmplr_item> sub_item;
 } ;
 
 struct cmplr_item_2 { /* Data structure for holding buffer items with index. */
     ListAdapter<cmplr_item> c_items;
 } ;
 
 extern struct cmplr_item new_cmplr_item(u2 init, double* data, string str);
 
 void cmplrfree( ListAdapter<cmplr_item> &tmppt );


 extern struct cmplr_item_2 *new_cmplr_item2();

 void cmplrfree2( struct cmplr_item_2 *tmppt );
 
 void cmplr_cleanup(bool fullFlush);


 extern ListAdapter<cmplr_item> cplrfreelist1;
 
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
 int cmplr_add_item( struct cmplr_item b2 );
 
 
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
 
 /**
  * Compiler Access specifiers
  * 
  */
 #define access_public 0x3
 #define access_private 0x2
 #define access_protected 0x1
 
 /**
  * Compiler Object types
  * 
  */
 #define typedef_bit    0x9
 #define typedef_byte   0x8
 #define typedef_short  0x7
 #define typedef_char   0x6
 #define typedef_int    0x5
 #define typedef_long   0x4
 #define typedef_float  0x3
 #define typedef_double 0x2
 #define typedef_string 0x1
 #define typedef_boolean 0x0
 #define typedef_class  (-0x1)
 #define typedef_node   (-0x2)
 #define typedef_function (-0x3)
 #define typedef_label (-0x4)
 #define typedef_unknown (-0x5)
 #define typedef_unused_object (-0x6)
 
 struct ClassObject;
 
 struct Object {
     int type;
     u1 size_t, eadr;
     
     bool isStatic, isarray, isConst;
     int access, scopeLevel;
     
     std::string name, package;
     std::string _namespace;
     std::string parentclass;
     
     struct ClassObject *C;
     
     bool _static(){ return isStatic; }
     bool array(){ return isarray; }
     bool _const(){ return isConst; }
     unsigned long size(){ return size_t.byte1; }
     bool _private(){ return (access == access_private); }
     bool _public(){ return (access == access_public); }
     bool _protected(){ return (access == access_protected); }
     
 };
 
 struct ClassObject {
     ListAdapter<Object> classObjects;
     ListAdapter<ClassObject> superclass;
     ListAdapter<cmplr_item> iqueue;
     
     ListAdapter<Object> classItems;
     std::string name; // used for class nesting
 };
 
 struct Method {
     u1 eadr;
     
     std::string name, package;
     
     std::string _namespace;
     std::string parentclass;
     
     bool isStatic, isnative;
     int access;
     
     ListAdapter<Object> args;
     
     bool _static(){ return isStatic; }
     bool _private(){ return (access == access_private); }
     bool _public(){ return (access == access_public); }
     bool _protected(){ return (access == access_protected); }
     
 };
 
 struct _namespace
 {
     std::string name;
     std::string parent;
     
     ListAdapter<_namespace> subnamespace;
 };
 
 /* Dynamic memory */
 extern ListAdapter<Object> objects;
 extern ListAdapter<Method> methods;
 extern ListAdapter<_namespace> namespaces;
 
 #endif // _COMPILR_CORE
 
