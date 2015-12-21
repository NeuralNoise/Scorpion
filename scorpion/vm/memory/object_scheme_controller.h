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
 #ifndef SCORPION_OBJECT_CONTROLLER
 #define SCORPION_OBJECT_CONTROLLER
 
 #include <string>
 #include <stdint.h>
 #include "allocation_scheme.h"
 #include "gc.h"
 #include "object_container.h"
 #include "block_allocator.h"
 #include "scorpion_assert.h"
 #include "../../clib/arraylist.h"
 
 using namespace std;
 using namespace scorpionvm::memory;
 using namespace scorpionvm::memory::schema;
 
  #define nullptr ((void *)0)
  
 namespace scorpionvm
 {
     namespace memory
     {
         namespace schema
         {
             namespace object_scheme_controller
             {
                 enum response_codes
                 {
                     object_alreay_created=0x000,
                     object_dirty=0x00001,
                     object_alloc_ok=0x00002,
                     object_alloc_failed=0xfff
                 };
                 
                 bool isnumber(ObjectSchema::t type)
                 {
                     return (type == ObjectSchema::SBOOL || type == ObjectSchema::SCHAR 
                         || type == ObjectSchema::SINT || type == ObjectSchema::SBYTE
                         || type == ObjectSchema::SLONG || type == ObjectSchema::SFLOAT 
                         || type == ObjectSchema::SDOUBLE || type == ObjectSchema::SSHORT);
                 }
                 
                 int create_object(ObjectContainer &container, 
                     ObjectSchema::t type, uint64_t array)
                 {
                     scorpionvm::scorpion_assert(container.gc_ == gc::gc_idle ||
                               container.gc_ == 0, "container.gc_ == gc::gc_idle ||\n\tcontainer.gc_ == 0");
                               
                     if(container.init)
                       return object_alreay_created;
                     else if(container.gc_ == gc::gc_dirty)
                       return object_dirty;
                       
                     scorpionvm::memory::BlockAllocator
                            <ObjectSchema> schema_allocator;
                       
                     container.init = true;
                     if(isnumber(type))
                     {
                        container.string_=false;
                        container.valuetype = schema_allocator.malloc(((array==0) ? 1 : array),0); // add if statement to check if array is == 0
                        if(container.valuetype==NULL)
                        {
                            return object_alloc_failed;
                        }
                        
                        container.valuetype->type = type;
                        container.size_t=1;
                        container.init=true;
                        container.array = ((array == 0) ? false : true);
                        container.gc_ = scorpionvm::memory::gc::gc_clean;
                        
                        return object_alloc_ok;
                     }
                     else // object is a string
                     {
                        container.string_=true;
                        if(container.string_schema_init(((array == 0) ? 1 : array)) != 0)
                          return object_alloc_failed;
                        container.size_t=0;
                        container.init=true;
                        container.array = ((array == 0) ? false : true);
                        container.gc_ = gc::gc_clean;
                        
                        return object_alloc_ok;
                     }
                 }
                 
                 int kill_object(ObjectContainer &container)
                 {
                     
                 }
                 
                 int delete_object(ObjectContainer &container)
                 {
                     return container.destroy_self();
                 }
             } // end object__scheme_controller
         } // end schema
     } // end memory
 } // end scorpionvm
 
 #endif // SCORPION_OBJECT_CONTROLLER
