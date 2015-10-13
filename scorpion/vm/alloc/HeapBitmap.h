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
 #ifndef SCORPION_HEAP_BITMAP
 #define SCORPION_HEAP_BITMAP
 
 struct Object;
 struct StringObject;
 struct ArrayObject;
 
 #include "../oo/Object.h"
 
 class ScorpionEnv;
 #define BITMAP_ALLOC (0x23)
 
 struct HeapBitmap {
     /* We set up some standard bitmap flags */
     unsigned long size_t;
     unsigned long base, MaxLimit;
     
     /*
     * Have we initalized the Bitmap yet?
     */
     u1 init;
     
     Object* objs;
 };
 
 // TODO: apply comments
 // TODO: implement methods
 bool svmHeapBitmapStartup(HeapBitmap &bitmap, long base, long maxLimit, long methodLimit, long bitmapsz_t, long stacksz);

 bool svmReallocBitmap(HeapBitmap &bitmap, long bitmapsz_t, long stacksz);
 
 long svmGetBitmapSize(HeapBitmap bitmap, int dataset);

 void svmBitmapMemoryShutdown(ScorpionEnv env);
 
 void svmClearBitmap(HeapBitmap bitmap);
 
 bool svmIsValidAddr(HeapBitmap bitmap, int dataset, long addr);

 #endif // SCORPION_HEAP_BITMAP