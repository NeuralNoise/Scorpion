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
 #include "xso.h"
 #include "img.h"
 #include "../clib/u4.h"
 #include "../clib/u2.h"
 #include "../clib/u1.h"
 #include "../vm/Globals.h"
 #include "../clib/binary.h"
 #include <sstream>
 #include <stdio.h>
 #include <stdlib.h>     
 #include <iostream>
 #include <string>
 
 using namespace std;
 
 long getpsize(XSO x) // get permission size
 { 
     int count = 0;
     for(int i = 0; i < x.headerInf.permissions.size(); i++){
         if(x.headerInf.permissions.at(i) == ';')
            count++;
     }
     return count;
 }
 
 void setPermissions(XSO x)
 {
     int permissionsize_t = getpsize(x);
     
     gSvm.psize_t = permissionsize_t;
     gSvm.permissions = new Permission[permissionsize_t]; 
     int pcount = 0;
     
     for(int i = 0; i < x.headerInf.permissions.size(); i++){
         stringstream ss;
         for(int xx = i; xx < x.headerInf.permissions.size(); xx++){
             if(x.headerInf.permissions.at(xx) != ';')
                ss << x.headerInf.permissions.at(xx) << "";
             else
               break;
             i++;
         }
         gSvm.permissions[pcount++].setPermission(ss.str());
     }
 }
 
 
 