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
 #ifndef SCORPION_BLOCK_ALLOCATOR
 #define SCORPION_BLOCK_ALLOCATOR
 
 #include <string>
 #include <stdint.h>
 #include <stdio.h>
 #include <iostream>
 #include <cstdlib>
 #include "allocation_scheme.h"
 
 using namespace std;
 
  #define nullptr ((void *)0)
  
 namespace scorpionvm
 {
     namespace memory
     {
         template <class T>
         class BlockAllocator : public scorpionvm::memory::schema::AllocationScheme<T>
         {
              public:
                 BlockAllocator()
                 : base_size(0),
                   m_total_size(0)
                 {
                 }
    
                 T* malloc(unsigned long N, unsigned long size_t)
                 {
                     void* ptr = nullptr;
                     
                     ptr = std::malloc(sizeof(T)*N+1);
                     if(ptr == NULL)
                     {
                         T* p=NULL;
                         return p;
                     }
                     
                     T* mem = reinterpret_cast<T*>(ptr);
                     base_size=N;
                     m_total_size=size_t;
                     return mem;
                 }
                 
                 T* calloc(unsigned long N, unsigned long size_t)
                 {
                     void* ptr = nullptr;
                     
                     ptr = std::calloc(N,sizeof(T));
                     if(ptr == NULL)
                     {
                         T* p=NULL;
                         return p;
                     }
                     
                     T* mem = reinterpret_cast<T*>(ptr);
                     base_size=N;
                     m_total_size=size_t;
                     return mem;
                 }
                 
                 int free(T* ptr)
                 {
                     if(ptr==NULL||ptr==nullptr)
                       return 1;
                     
                     std::free(ptr); 
                     base_size=0;
                     m_total_size=0;
                     return 0;
                 }

                 uint64_t size_t(int s)
                 {
                     if(s==0)
                       return base_size;
                     else return m_total_size;
                 }
                 
              private:
                uint64_t base_size;
                uint64_t m_total_size;
         };
     } // end memory
 } // end scorpionvm
 
 #endif // SCORPION_BLOCK_ALLOCATOR