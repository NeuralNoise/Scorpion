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
 #ifndef SCORPION_MEMORY_ENVIRONMENT
 #define SCORPION_MEMORY_ENVIRONMENT
 
 #include <string>
 #include <stdint.h>
 #include "allocation_scheme.h"
 #include "gc.h"
 #include "object_container.h"
 #include "block_allocator.h"
 #include "../../clib/arraylist.h"
 
 using namespace std;
 
  #define nullptr ((void *)0)
  
 namespace ScorpionVM
 {
     namespace memory
     {
         namespace environment
         {
             class scorpion_env
             {
                 
                 public:
                   const char* name;
                   ScorpionVM::memory::ObjectContainer* m_heap;
                   int64_t* m_stack;
                   
                   int alloc(uint64_t _alloc_base, uint64_t _alloc_max, uint64_t stack_t)
                   {
                       m_heap = base_allocator.malloc(_alloc_base, _alloc_max);
                       if(m_heap == NULL)
                          return 1;
                       
                       m_stack = base_allocator2.malloc(stack_t, stack_t);
                       if(m_stack == NULL)
                       {
                         base_allocator.free(m_heap);
                         return 1;
                       }
                       return 0;
                   }
                   int destroy_self() // destroy*self()
                   {
                       int r=0;  
                       name = "";
                       r+=base_allocator.free(m_heap);
                       r+=base_allocator2.free(m_stack);
                       return r;
                   }
                   void setName(const char* n)
                   {
                       name = n;
                   }
                   string getName()
                   {
                       stringstream ss;
                       ss << name;
                       return ss.str();
                   }
                   
                   uint64_t sizeinfo(int i, bool heap = true)
                   {
                       return ((heap) ? base_allocator.size_t(i) : base_allocator2.size_t(i));
                   }
                   
                 private:
                    ScorpionVM::memory::BlockAllocator
                       <ScorpionVM::memory::ObjectContainer> base_allocator; // allocators for stack and heap
                    ScorpionVM::memory::BlockAllocator<int64_t> base_allocator2;
             };
         } // end environments
     } // end memory
 } // end ScorpionVM
 
#endif // SCORPION_MEMORY_ENVIRONMENT