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
 #include "../../logservice/ctime.h"
 #include "../../logservice/alog.h"
 #include "../Globals.h"
 #include <stdlib.h>
 #include <sstream>
 #include <string>
 
 #include "gc.h"
 
 using namespace std;
      
 long gc_objc = 0;
 extern ALog alog;
 
 void setObjAvailable(Object &obj){
     obj.instanceData.byte2 = GC_IDLE;
 }
 
 double getper(long val1, long val2){
    double a = 1.0, b = 1.0, c;
    a = a*val1;
    b = b*val2;
    
    c = (a/b) * 100;
    return (int) c;
 }
 
 void invalidateObjs(HeapBitmap &bitmap){
     if(gc_objc < GC_LIMIT)
        return;
     
     long bytes_free = 0;
     u2 time;
     time.byte1 = getWallTimeInUsec();
     
     gc_objc = 0;
     for(long i = 0; i < bitmap.size_t; i++){
         if(!svmObjectIsAlive(bitmap.objs[i]) && !objIsDirty(bitmap.objs[i]))
             continue;
             
         free(bitmap.objs[i].obj);
         setObjAvailable(bitmap.objs[i]);
     }
     
     time.byte2 = getWallTimeInUsec();
     if((time.byte2 - time.byte1) < 0){
        time.byte1 = 1000 + (time.byte2 - time.byte1);
     }
     else
       time.byte1 = (time.byte2 - time.byte1);
     
     stringstream ss;
     ss << "GC_ALLOC freed " << bytes_free << " Objects, " << getper(bytes_free, bitmap.size_t) 
           << "% free " << bytes_free << "Objs/" << bitmap.size_t << "Objs, paused " << time.byte1 << "ms"; 
     
     alog.setClass("scorpionvm");
     alog.ALOGV(ss.str());
     
     gSvm.dlog.DLOGV("scorpionvm", ss.str());
 }
 
 bool objIsDirty(Object &obj){
     return (__gcstatus(obj) == GC_DIRTY);
 }
 
 
 
 