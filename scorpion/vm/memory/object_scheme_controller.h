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
                 
                 bool isnumber(ObjectSchema::t type);
                 
                 bool isnumber(ObjectContainer &container);
                 
                 void arrycpy(ObjectContainer &container, ObjectContainer &container2);
                 
                 int create_object(ObjectContainer &container, 
                     ObjectSchema::t type, uint64_t array);
                 
                 int kill_object(ObjectContainer &container);
                 
                 int delete_object(ObjectContainer &container);
             } // end object__scheme_controller
         } // end schema
     } // end memory
 } // end scorpionvm
 
 #endif // SCORPION_OBJECT_CONTROLLER
