 
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
 #include "Object.h"
 #include "Array.h"
 #include "../exception.h"
 #include <string>
 #include <stdlib.h> 
 #include <sstream>
 #include <iostream>
 #include <stdint.h>
 
 using namespace std;
 extern float btof(bool b);
  
 ListAdapter<schar> tochararray(string data){
     ListAdapter<schar> hashList;
     hashList._init_();
     hashList.enableprotected(); // Set hash list to protected mode for creation of large strings
       
     for(long i = 0; i < data.size(); i++)
     {
       hashList.add((schar) data.at(i));
       if(hashList._err())
        Exception("String could not be created.", "OutOfMemoryError");
     }
       
     return hashList;
 }
 
 string fromchararray(ListAdapter<schar> &hashList){
     stringstream ss;
     
     for(long i = 0; i < hashList.size(); i++)
       ss << (char) hashList.valueAt(i);
       
     return ss.str();
 }
 
 ListAdapter<schar> arryconcat(ListAdapter<schar> hashList1, ListAdapter<schar> hashList2){
     hashList1.enableprotected(); // Set hash list to protected mode for creation of large strings
     
     for(long i = 0; i < hashList2.size(); i++)
     {
        hashList1.add(hashList2.valueAt(i));
         
        if(hashList1._err())
          Exception("String could not be created.", "OutOfMemoryError");
     }
     hashList2.clear();
     return hashList1;
 }
 
 // TODO: Check array typedef and assign and get accordingly.
 /* Generic array parsing stuff */
 double get(Object &obj, long pos){
     if(!svmObjectIsAlive(obj))
          segfault();
  
     if(pos >= obj.obj->arrayobj->length){
       stringstream ss;
       ss << "Index at(" << pos << ") is not within bounds. Array size[" << length(obj) << "].";
       Exception(ss.str(), "OutOfMemoryError");
     }
     
     if(__typedef(obj) == TYPEDEF_GENERIC_BOOL_ARRAY)
       return btof(obj.obj->arrayobj->pboolean[pos]);
     else if(__typedef(obj) == TYPEDEF_GENERIC_INT_ARRAY)
       return obj.obj->arrayobj->pint[pos];
     else if(__typedef(obj) == TYPEDEF_GENERIC_SHORT_ARRAY)
       return obj.obj->arrayobj->pshort[pos];
     else if(__typedef(obj) == TYPEDEF_GENERIC_BYTE_ARRAY)
       return obj.obj->arrayobj->pbyte[pos];
     else if(__typedef(obj) == TYPEDEF_GENERIC_FLOAT_ARRAY)
       return obj.obj->arrayobj->pfloat[pos];
     else if(__typedef(obj) == TYPEDEF_GENERIC_DOUBLE_ARRAY)
       return obj.obj->arrayobj->pdouble[pos];
     else if(__typedef(obj) == TYPEDEF_GENERIC_CHAR_ARRAY)
       return obj.obj->arrayobj->pchar[pos];
 }

 void arrycpy(Object &obj, Object &obj2)
 {
	 if(!svmObjectIsAlive(obj) || !svmObjectIsAlive(obj2))
          segfault();
          
     if(__typedef(obj) != __typedef(obj2))
        Exception("Cannot assign array objects of different types.", "IllegalTypeException");
        
     if(__typedef(obj) == TYPEDEF_GENERIC_BOOL_ARRAY)
         obj.obj->arrayobj->pboolean = obj2.obj->arrayobj->pboolean;
     else if(__typedef(obj) == TYPEDEF_GENERIC_INT_ARRAY)
       obj.obj->arrayobj->pint = obj2.obj->arrayobj->pint;
     else if(__typedef(obj) == TYPEDEF_GENERIC_SHORT_ARRAY)
       obj.obj->arrayobj->pshort = obj2.obj->arrayobj->pshort;
     else if(__typedef(obj) == TYPEDEF_GENERIC_BYTE_ARRAY)
       obj.obj->arrayobj->pbyte = obj2.obj->arrayobj->pbyte;
     else if(__typedef(obj) == TYPEDEF_GENERIC_FLOAT_ARRAY)
       obj.obj->arrayobj->pfloat = obj2.obj->arrayobj->pfloat;
     else if(__typedef(obj) == TYPEDEF_GENERIC_DOUBLE_ARRAY)
       obj.obj->arrayobj->pdouble = obj2.obj->arrayobj->pdouble;
     else if(__typedef(obj) == TYPEDEF_GENERIC_CHAR_ARRAY)
       obj.obj->arrayobj->pchar = obj2.obj->arrayobj->pchar;
     else if(__typedef(obj) == TYPEDEF_GENERIC_LONG_ARRAY)
       obj.obj->arrayobj->plong = obj2.obj->arrayobj->plong;
 }
 
 void set(Object &obj, long pos, double default_value){
     if(!svmObjectIsAlive(obj))
          segfault();
          
     if(pos >= obj.obj->arrayobj->length){
       stringstream ss;
       ss << "Index at(" << pos << ") is not within bounds. Array size[" << length(obj) << "].";
       Exception(ss.str(), "ArrayIndexOutOfBoundsException");
     }
      
      if(__typedef(obj) == TYPEDEF_GENERIC_BOOL_ARRAY)
         obj.obj->arrayobj->pboolean[pos] = (bool) default_value;
     else if(__typedef(obj) == TYPEDEF_GENERIC_INT_ARRAY)
       obj.obj->arrayobj->pint[pos] = default_value;
     else if(__typedef(obj) == TYPEDEF_GENERIC_SHORT_ARRAY)
       obj.obj->arrayobj->pshort[pos] = default_value;
     else if(__typedef(obj) == TYPEDEF_GENERIC_BYTE_ARRAY)
       obj.obj->arrayobj->pbyte[pos] = default_value;
     else if(__typedef(obj) == TYPEDEF_GENERIC_FLOAT_ARRAY)
       obj.obj->arrayobj->pfloat[pos] = default_value;
     else if(__typedef(obj) == TYPEDEF_GENERIC_DOUBLE_ARRAY)
       obj.obj->arrayobj->pdouble[pos] = default_value;
     else if(__typedef(obj) == TYPEDEF_GENERIC_CHAR_ARRAY)
       obj.obj->arrayobj->pchar[pos] = default_value;
     else if(__typedef(obj) == TYPEDEF_GENERIC_LONG_ARRAY)
       obj.obj->arrayobj->plong[pos] = default_value;
 }
 
 
 long length(Object &obj){
     return obj.obj->arrayobj->length;
 }
 
 
 
