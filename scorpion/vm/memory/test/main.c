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
 /*
 * Command line invocation of the Scorpion VM
 */
 #include <stdlib.h>
 #include <stdio.h>
 #include <assert.h>
 #include <iostream>
 #include <sstream>
 #include "../allocation_scheme.h"
 #include "../block_allocator.h"
 #include "../object_container.h"
 #include "../object_scheme_controller.h"
 #include "../vm_env.h"
 
 using namespace std;
 
 int main(){
    ScorpionVM::memory::BlockAllocator
             <ScorpionVM::memory::environment::scorpion_env> base_allocator;
    ScorpionVM::memory::environment::scorpion_env* env;
    env = base_allocator.calloc(1,0);
    
    if(env == NULL)
    {
       cout << "Could not allocate environment memory.";
       return 1;   
    }
    
    uint64_t base_heap_size = 10000000;
    uint64_t max_heap_size =  10000000;
    
    if(env->alloc(base_heap_size, max_heap_size, 0xfff) != 0)
    {
       cout << "Could not allocate env->m_heap memory.";
       return 1;   
    }
    // ---------------------------------------------------------
    int r = 
       ScorpionVM::memory::schema::object_scheme_controller::create_object
         (env->m_heap[base_heap_size-1], ScorpionVM::memory::schema::ObjectSchema::SINT, 0);
    
    if(r != ScorpionVM::memory::schema::object_scheme_controller::object_alloc_ok && r != ScorpionVM::memory::schema::object_scheme_controller::object_alreay_created)
    {
       cout << "Could not allocate object in memory.";
       return 1;   
    }
    env->m_heap[base_heap_size-1].set_schema_(9);
    assert(env->m_heap[base_heap_size-1].schema_value() == 9);
    
    
    r = 
       ScorpionVM::memory::schema::object_scheme_controller::create_object
         (env->m_heap[base_heap_size-2], ScorpionVM::memory::schema::ObjectSchema::SSTRING, 0);
    
    if(r != ScorpionVM::memory::schema::object_scheme_controller::object_alloc_ok && r != ScorpionVM::memory::schema::object_scheme_controller::object_alreay_created)
    {
       cout << "Could not allocate object in memory.";
       return 1;   
    }
    
    env->m_heap[base_heap_size-2].string_schema_set_("Hello, World!", false);
    assert(env->m_heap[base_heap_size-2].string_schema_get_() == "Hello, World!");
    //cout << "str " << env->m_heap[base_heap_size-2].string_schema_get_() << endl;
    
    env->m_heap[base_heap_size-2].string_schema_set_(" This is cool", true);
    assert(env->m_heap[base_heap_size-2].string_schema_get_() == "Hello, World! This is cool");
    
    assert(env->m_heap[base_heap_size-2].string_schema_at_(0) == 'H');
    
    r = 
       ScorpionVM::memory::schema::object_scheme_controller::create_object
         (env->m_heap[base_heap_size-3], ScorpionVM::memory::schema::ObjectSchema::SLONG, 5);
    
    if(r != ScorpionVM::memory::schema::object_scheme_controller::object_alloc_ok && r != ScorpionVM::memory::schema::object_scheme_controller::object_alreay_created)
    {
       cout << "Could not allocate object in memory.";
       return 1;   
    }
    
    env->m_heap[base_heap_size-1].set_schema_(9, 0);
    assert(env->m_heap[base_heap_size-1].schema_value(0) == 9);

    r = 
       ScorpionVM::memory::schema::object_scheme_controller::create_object
         (env->m_heap[base_heap_size-4], ScorpionVM::memory::schema::ObjectSchema::SSTRING, 3);
    
    if(r != ScorpionVM::memory::schema::object_scheme_controller::object_alloc_ok && r != ScorpionVM::memory::schema::object_scheme_controller::object_alreay_created)
    {
       cout << "Could not allocate object in memory.";
       return 1;   
    }
    
    env->m_heap[base_heap_size-4].string_schema_set_("Hello, World!", false, 1);
    assert(env->m_heap[base_heap_size-4].string_schema_get_(1) == "Hello, World!");
    
    env->m_heap[base_heap_size-4].string_schema_set_(" This is cool", true, 1);
    assert(env->m_heap[base_heap_size-4].string_schema_get_(1) == "Hello, World! This is cool");
    
    assert(env->m_heap[base_heap_size-4].string_schema_at_(0, 1) == 'H');

    env->m_heap[base_heap_size-4].destroy_self();

    env->destroy_self();
    cout << "done.\n"; 
    return 0;
 }
 
 
 
 
 