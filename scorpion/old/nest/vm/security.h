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
#ifndef SCORPION_VM_SECURITY
#define SCORPION_VM_SECURITY

    #include "permission.h"
    #include "../logservice/alog.h"
    #include <stdio.h>
    #include <iostream>
    #include <sstream>
    #include <string>
    using namespace std;


 class AccessController {
     protected:
       string* permissionList;
       int access_size_t;
     
     public:
       void setupAccesPermissions(){
           access_size_t = 4;
           permissionList = new string[access_size_t];
           
           permissionList[0] = "scorpion.permission.READ_INTERNAL_STORAGE";
           permissionList[1] = "scorpion.permission.WRITE_INTERNAL_STORAGE";
           permissionList[2] = "scorpion.permission.GPIO"; // for raspberry pi only
           permissionList[3] = "scorpion.permission.REBOOT";
       }
       bool permissionExists(Permission perm){
           for(int i = 0; i < access_size_t; i++){
               if(perm.getPermission() == permissionList[i])
                 return true;
           }
          return false;
       }
       int getPermissionSize(){
           return access_size_t;
       }
       Permission valueAt(int idx){
           
           Permission p;
           p.setPermission("null");
           if(idx < access_size_t){
               p.setPermission(permissionList[idx]);
               return p;
           }
           else
             return p;
       }
 };

/*
* This is a very simple security framework for the scorpion programming language
*/
 class SecurityManager {
      Permission *permissions;
      int pcount, var2;
      
      public:
          AccessController controller;
          void setup(){
              permissions = new Permission[controller.getPermissionSize()];
              pcount = controller.getPermissionSize();
              var2 = 0;
          }
          void add(Permission perm){
              var2++;
              if(var2 > pcount){
                  alog.setClass("SecurityManager");
                  alog.ALOGV("warning:  max ammount of possible permissions added!.");
              }
              
              permissions[var2 - 1] = perm;
          }
          Permission valueAt(int idx){
               Permission p;
               p.setPermission("null");
               if(idx < pcount){
                   p.setPermission(permissions[idx].getPermission());
                   return p;
               }
               else
                 return p;
          }
          bool hasPermission(Permission perm){
              for(int i = 0; i < pcount; i++){
                  if(perm.getPermission() == permissions[i].getPermission())
                        return true;
              }
            return false;
          }
          bool checkPermission(Permission perm){
              return controller.permissionExists(perm);
          }
 };

#endif

