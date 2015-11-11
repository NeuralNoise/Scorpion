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
 
#include "HeapBitmap.h"
#include "../oo/Object.h"
#include "../exception.h"
#include <string>
#include <new>
#include <iostream>
#include <stdlib.h>

using namespace std;
 
 bool svmHeapBitmapInit(HeapBitmap &bitmap, long _base, long maxLimit, long stack, long bitmapsz_t){
     if(svmBitmapInitalized(bitmap))
        Exception("Cannot re-initalize a previously initalized bitmap.", "IllegalStateExcpetion");
     
     if(stack > stack_limit){
         bitmap.reason.byte1 = ALLOC_STACK_OVERLOAD;
         return false;    
     }
     
     
     bitmap.base = _base;
     bitmap.MaxLimit = maxLimit;
     bitmap.size_t = bitmapsz_t;
     bitmap.stsz_t = stack;
     
     bitmap.objs = (Object*)malloc(_base);
     bitmap.stack = new (nothrow) ArrayObject[1];
         
     if(bitmap.stack == NULL || bitmap.objs == nullptr)
         goto bail;
     
     bitmap.stack->generic = (double*)malloc(stack);
     bitmap.stack->length = stack;
     
     if(bitmap.stack->generic == nullptr)
         goto bail;
     
     bitmap.init.byte1 = BITMAP_ALLOC; // bitmap created successfully
     return true;
     
     bail:
       free(bitmap.objs);
       free(bitmap.stack->generic);
       bitmap.base = 0;
       bitmap.size_t = 0;
       bitmap.stsz_t = 0;
       bitmap.MaxLimit = 0;
       bitmap.init.byte1 = 0;
       bitmap.reason.byte1 = ALLOC_OUT_OF_MEMORY;
       return false;   
 }

 bool svmReallocBitmap(HeapBitmap &bitmap, long bitmapsz_t, long stack){
     if(!svmBitmapInitalized(bitmap))
        Exception("Failed to re-alloc non-initalized bitmap.", "IllegalStateExcpetion");
     
     if(bitmapsz_t < bitmap.base || bitmapsz_t > bitmap.MaxLimit){
        bitmap.reason.byte1 = ALLOC_OUT_OF_BOUNDS;
        return false;
     }
    
       return svmHeapBitmapInit(bitmap, bitmap.base, bitmap.MaxLimit, stack, bitmapsz_t);   
 }
 
 long svmGetBitmapSize(HeapBitmap &bitmap, int dataset){
     if(dataset == dataset_obj)
        return bitmap.size_t;
     else if(dataset == dataset_stack)
        return bitmap.stsz_t;
     else
        return -1939828;
 }
 
 void svmBitmapMemoryShutdown(HeapBitmap &bitmap){
     if(!svmBitmapInitalized(bitmap))
        return;
        
       free(bitmap.objs);
       free(bitmap.stack->generic);
       bitmap.init.byte1 = 0;
       bitmap.base = 0;
       bitmap.MaxLimit = 0;
       bitmap.size_t = 0;
       bitmap.stsz_t = 0;
 }
 
 void svmClearBitmap(HeapBitmap &bitmap) {
     if(!svmBitmapInitalized(bitmap))
        Exception("Failed to clear non-initalized bitmap.", "IllegalStateExcpetion");
  
     free(bitmap.objs);
     free(bitmap.stack->generic);
     bitmap.init.byte1 = 0;
     
     if(!svmHeapBitmapInit(bitmap, bitmap.base, bitmap.MaxLimit, bitmap.stsz_t, bitmap.size_t))
       Exception("Bitmap could not be cleared cleanley.", "MemoryFailureError");
 }
 
 bool svmIsValidAddr(HeapBitmap &bitmap, int dataset, long addr){
     if(dataset == dataset_obj)
       return addr >= bitmap.size_t;
     else
       return addr >= bitmap.stsz_t;
 }
 
 void svmObjectOk(HeapBitmap &bitmap, long pos){
     if(!svmIsValidAddr(bitmap, dataset_obj, pos))
       Exception("Cannot access Object at address " + pos, "IllegalAccessExcpetion");
      
     if(!svmObjectIsDead(bitmap.objs[pos]))
       Exception("Object is null.", "NullPointerException");
 }
 
 void svmObjectAssign(Object &obj, Object &obj2){
     if(!svmObjectIsDead(obj))
       Exception("Object is null.", "NullPointerException");
     obj = obj2; // we assume compiler did object checking
 }
 
 bool svmBitmapInitalized(HeapBitmap &bitmap){
    return (bitmap.init.byte1 == BITMAP_ALLOC);
 }
 
 