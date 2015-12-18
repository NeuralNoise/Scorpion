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
 #ifndef SCORPION_OBJECT_CONTAINER
 #define SCORPION_OBJECT_CONTAINER
 
 #include <string>
 #include <stdint.h>
 #include <limits>
 #include "allocation_scheme.h"
 #include "block_allocator.h"
 #include "gc.h"
 #include "../../clib/arraylist.h"
 
 using namespace std;
 
  #define nullptr ((void *)0)
  
 namespace scorpionvm
 {
     namespace memory
     {
         class ObjectContainer
         {
             public:
               ObjectContainer()
               : init(false),
                 string_(false),
                 array(false),
                 gc_(scorpionvm::memory::gc::gc_idle),
                 size_t(0)
               {
               }
               
               scorpionvm::memory::schema::ObjectSchema* valuetype;
               const char* name; // for debugging objects
               
               uint64_t size_t;
               int8_t gc_; // Garbage collector status
               bool init, string_, array;
               
               int destroy_self() // emplode object
               {
                   if(!init) return 1;
                   ObjectContainer();
                   if(string_)
                   {
                     set_size_t=0;
                     scorpionvm::memory::BlockAllocator
                            <ListAdapter<schar> > schema_allocator;
                            
                     return schema_allocator.free(hash_set);
                   }
                   else
                   {
                      scorpionvm::memory::BlockAllocator
                            <scorpionvm::memory::schema::ObjectSchema> schema_allocator;
                     return schema_allocator.free(valuetype);
                   }
               }
                   
               double schema_value(uint64_t i = 0)
               {
                   if(string_) return std::numeric_limits<double>::max();
                   if(array && (i<0||i>size_t)) return std::numeric_limits<double>::max();
                   switch( valuetype->type )
                   {
                       case scorpionvm::memory::schema::ObjectSchema::SBYTE: 
                              return ((!array) ? valuetype->byte : valuetype[i].byte); break;
                       case scorpionvm::memory::schema::ObjectSchema::SSHORT: 
                              return ((!array) ? valuetype->short_ : valuetype[i].byte); break;
                       case scorpionvm::memory::schema::ObjectSchema::SCHAR: 
                              return ((!array) ? valuetype->char_ : valuetype[i].byte); break;
                       case scorpionvm::memory::schema::ObjectSchema::SINT: 
                              return ((!array) ? valuetype->int_ : valuetype[i].byte); break;
                       case scorpionvm::memory::schema::ObjectSchema::SLONG: 
                              return ((!array) ? valuetype->long_ : valuetype[i].byte); break;
                       case scorpionvm::memory::schema::ObjectSchema::SFLOAT: 
                              return ((!array) ? valuetype->float_ : valuetype[i].byte); break;
                       case scorpionvm::memory::schema::ObjectSchema::SDOUBLE: 
                              return ((!array) ? valuetype->double_ : valuetype[i].byte); break;
                       case scorpionvm::memory::schema::ObjectSchema::SBOOL: 
                              return ((!array) ? valuetype->boolean : valuetype[i].byte); break;
                   }
               }
               
               void set_schema_(double data, uint64_t i = 0)
               {
                   if(string_) return;
                   if(array && (i<0||i>size_t)) return;
                   switch( valuetype->type )
                   {
                       case scorpionvm::memory::schema::ObjectSchema::SBYTE: 
                               if(!array) valuetype->byte = data;
                               else valuetype[i].byte = data; 
                       break;
                       case scorpionvm::memory::schema::ObjectSchema::SSHORT: 
                               if(!array) valuetype->short_ =  data;
                               else valuetype[i].byte = data; 
                       break;
                       case scorpionvm::memory::schema::ObjectSchema::SCHAR: 
                               if(!array) valuetype->char_ = data;
                               else valuetype[i].byte = data; 
                       break;
                       case scorpionvm::memory::schema::ObjectSchema::SINT: 
                               if(!array) valuetype->int_ = data;
                               else valuetype[i].byte = data; 
                       break;
                       case scorpionvm::memory::schema::ObjectSchema::SLONG: 
                               if(!array) valuetype->long_ = data;
                               else valuetype[i].byte = data; 
                       break;
                       case scorpionvm::memory::schema::ObjectSchema::SFLOAT:  
                               if(!array) valuetype->float_ = data;
                               else valuetype[i].byte = data; 
                       break;
                       case scorpionvm::memory::schema::ObjectSchema::SDOUBLE: 
                               if(!array) valuetype->double_ = data;
                               else valuetype[i].byte = data; 
                       break;
                       case scorpionvm::memory::schema::ObjectSchema::SBOOL: 
                               if(!array) valuetype->boolean = data;
                               else valuetype[i].byte = data; 
                       break;
                   }
               }
               
               int string_schema_set_(string data, bool append, uint64_t i = 0)
               {
                   if(!string_) return 1;
                   if(!append)
                   {
                       if(array)
                         hash_set[i].clear();
                       else hash_set->clear();
                   }
                   if(array && (i<0||i>=set_size_t)) return 2;
                   
                   if(array)
                   {
                       for( uint64_t i2=0; i2<data.size(); i2++ )
                       {
                           hash_set[i].add((schar)data.at(i2));
                           if(hash_set[i]._err()) return 1;
                       }
                       return 0;
                   }
                   else 
                   {
                       for( uint64_t i2=0; i2<data.size(); i2++ )
                       {
                           hash_set->add((schar)data.at(i2));
                           if(hash_set->_err()) return 1;
                       }
                       return 0;
                   }
               }
               
               string string_schema_get_(uint64_t i = 0)
               {
                   if(!string_) return "";
                   if(array && (i<0||i>set_size_t)) return "";
                   
                   stringstream ss;
                   if(array)
                   {
                       for( uint64_t i2=0; i2<hash_set[i].size(); i2++ )
                       {
                           ss << (char) hash_set[i].valueAt(i2);
                       }
                       return ss.str();
                   }
                   else 
                   {
                       for( uint64_t i2=0; i2<hash_set->size(); i2++ )
                       {
                           ss << (char) hash_set->valueAt(i2);
                       }
                       return ss.str();
                   }
               }
               
               schar string_schema_at_(uint64_t i, uint64_t id = 0)
               {
                   if(!string_) return 1;
                   if(array && (id<0||id>=set_size_t)) return 2;
                   if(i>hash_set[id].size() || i<0) return 0;
                   return ((array) ? hash_set[id].valueAt(i) : hash_set->valueAt(i));
               }
               
               int string_schema_init(uint64_t i)
               {
                   if(!string_) return 1;
                   scorpionvm::memory::BlockAllocator
                            <ListAdapter<schar> > schema_allocator;
                            
                   hash_set = schema_allocator.malloc(i,0);
                   if(hash_set==NULL) return 1;
                   set_size_t=i;
                   size_t=i;
                   if(i==0)
                   {
                     hash_set->_init_();
                     hash_set->enableprotected(); // set adapter class to protected mode
                   }
                   else
                   {
                     hash_set[i]._init_();
                     hash_set[i].enableprotected(); // set adapter class to protected mode
                   }
                   return 0;
               }
               
             private:
               ListAdapter<schar>* hash_set; // for string objects
               uint64_t set_size_t;
         };
         
         class MethodContainer
         {
           public:
             MethodContainer()
             : clazz(""),
               package(""),
               name(""),
               jmp(0),
               ret(0),
               native(false)
             {
             }
             
             string clazz, package, 
                    name;
             uint64_t jmp, ret;
             bool native;
         };
     } // end memory
 } // end scorpionvm
         
#endif // SCORPION_OBJECT_CONTAINER
         