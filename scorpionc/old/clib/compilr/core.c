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
 * A small portion of the software depicted in this library goes to 
 * the creator of C LTL(C++ Lexer Toolkit Library)
 *
 * (http://www.partow.net/programming/lexertk/index.html) 2001
 * Author - Arash Partow
 */
 #include "compilr.h"
 #include "core.h"
 #include "../arraylist.h"
 #include <stdio.h>
 #include <stdlib.h> 
 #include <string>
 #include <iostream>
 
 using namespace std;

 ListAdapter<cmplr_item> cplrfreelist1;
 
 cmplr_item_2 *cplrfreelist2=0;
 #define sub_item_base_sz1 4
 
 ListAdapter<Object> objects;
 ListAdapter<Method> methods;
 ListAdapter<_namespace> namespaces;
 
 bool init = false;
 
 
 struct cmplr_item new_cmplr_item(u2 init, double* data, string str)
 {
    struct cmplr_item citem;
    citem.size_t.byte1 = init.byte2;
    citem.item.byte1 = init.byte1;
    
    citem.sub_item._init_();
    //citem->str = new string[1];
    for(int i = 0; i < init.byte2; i++)
    {
       struct cmplr_item it;
       it.item.byte1 = data[i];
       citem.sub_item.add( it );
    }
       
    citem.str=str;
    return citem;
 }

 struct cmplr_item_2 *new_cmplr_item2()
 {
     struct cmplr_item_2 *citem2 = new cmplr_item_2[1];
     return citem2;
 }

 void cmplrfree( ListAdapter<cmplr_item> &tmppt )
 {
     tmppt.clear();
 }
 
 void cmplrfree2( struct cmplr_item_2 *tmppt )
 {
    tmppt->c_items.clear();
 }
 
 void cmplr_cleanup(bool fullFlush)
 {
     if(!init)
        return;
     
     cmplrfree2(cplrfreelist2);
     cmplrfree(cplrfreelist1);
     
     if(fullFlush)
     {
         //Flush everything
         objects.clear();
         methods.clear();
         init=false;
     }
 }

/*-----------------------*/
/* Initalize the Compilr library */
/*  This will be called automatically */
/*  upon calling any cmplr routines. */
/*-----------------------*/
 int cmplr_init()
 {
     cplrfreelist2 = new_cmplr_item2();
     init=true;
     return 0;
 }

/*-----------------------*/
/* Add item to a buffer. */
/*-----------------------*/
 int cmplr_add_item( struct cmplr_item b2 )
 {
     cplrfreelist2->c_items.add(b2);
     return 0;
 }
 