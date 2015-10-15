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
 #ifndef SCORPION_GARBAGE_COLLECTOR
 #define SCORPION_GARBAGE_COLLECTOR
 
 #include "../oo/Object.h"
 #include "HeapBitmap.h"
 
 struct HeapBitmap;
 
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
 
 /*
 * This is our max limit of Objects allowed before 
 * the GC is activated.
 */
 #define GC_LIMIT ((64 * 16) * 12)
 
 
 // The total count of all our Objects       
 extern long gc_objc;
 
 /* GC object control methods */      
 void setObjAvailable(Object &obj); // sets Object to Idle state
 
 void invalidateObjs(HeapBitmap &bitmap);
 
 bool objIsDirty(Object &obj);

 
 /*
 * Check the obj count
 * if(true) invalidateObjects();
 */
 #define checkGC(bitmap) \
       invalidateObjs(bitmap);
 
 #endif // SCORPION_GARBAGE_COLLECTOR