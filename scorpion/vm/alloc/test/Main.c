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
 #include "../HeapBitmap.h"
 #include "../../oo/Array.h"
 #include "../../oo/String.h"
 
 using namespace std;
 
 HeapBitmap bitmap;
 
 void Init(){
     bitmap.size_t = 10; // 10 objects
     bitmap.base = 10;
     bitmap.MaxLimit = 100;
     
     bitmap.init.byte1 = BITMAP_ALLOC;
     
     bitmap.objs = new (nothrow) Object[10]; 
 }
 
 extern long str_location;
 // TODO: Test all newly created methods and apply all comments
 int main(){
    cout << "------------------------------------------------\n";
    cout << "     Scorpion Bitmap Memory Structure Test\n";
    cout << "------------------------------------------------\n\n";
  
    svmHeapBitmapInit(bitmap, 10, 100, 10, 10);
    cout << "Bitmap mapped with size " << svmGetBitmapSize(bitmap, dataset_obj) << endl;
    cout << "Testing structure..  " << std::flush;
    
    
    u1 sz;
    sz.byte1 = 1;
    SVM_OBJECT_INIT(bitmap.objs[default_loc], TYPEDEF_GENERIC, sz);
    
    svmSetGenericValue(bitmap.objs[default_loc], 32);
    assert(svmGetGenericValue(bitmap.objs[default_loc]) == 32);
    
    SVM_OBJECT_INIT(bitmap.objs[5], TYPEDEF_GENERIC_ARRAY, sz); // int *a = new int[1];
    
    set(bitmap.objs[5], 0, 34);
    assert(get(bitmap.objs[5], 0) == 34);
    assert(length(bitmap.objs[5]) == 1);
    
    SVM_OBJECT_INIT(bitmap.objs[2], TYPEDEF_STRING, sz);
    
    assign(bitmap.objs[2], "hello");
    assert(size(bitmap.objs[2]) == 5);
    assert(fromchararray(bitmap.objs[2].obj[default_loc].strobj->array[default_loc]) == "hello");
    
    concat(bitmap.objs[2], " world");
    assert(size(bitmap.objs[2]) == 11);
    assert(fromchararray(bitmap.objs[2].obj[default_loc].strobj->array[default_loc]) == "hello world");
    
    
    SVM_OBJECT_INIT(bitmap.objs[7], TYPEDEF_STRING_ARRAY,  sz);
    assert(length(bitmap.objs[7]) == 1);
    str_location = 0;
    
    assign(bitmap.objs[7], "hello");
    assert(size(bitmap.objs[7]) == 5);
    assert(fromchararray(bitmap.objs[7].obj->arrayobj->strobj[default_loc].array[default_loc]) == "hello");
    
    concat(bitmap.objs[7], " world");
    assert(size(bitmap.objs[7]) == 11);
    assert(fromchararray(bitmap.objs[7].obj->arrayobj->strobj[default_loc].array[default_loc]) == "hello world");
    
    svmBitmapMemoryShutdown(bitmap);
    
    cout << "done.\n"; 
    return 0;
 }
 
 
 
 