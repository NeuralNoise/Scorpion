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
 #include <stdio.h>
 #include <stdlib.h> 
 #include <string>
 #include <iostream>
 
 using namespace std;

 cmplr_item *cplrfreelist1=0;
 
 cmplr_item_2 *cplrfreelist2=0;
 #define sub_item_base_sz1 4
 
 bool init = false;
 unsigned long cplr_item_buflen=2 * 1048576;
 unsigned long cplr_bitmap_len=4 * 1748576;
 unsigned long cplr_item_sz1=0;
 
 
 struct cmplr_item *new_cmplr_item(u2 init, double* data, string str)
 {
    struct cmplr_item *citem = new cmplr_item[1];
    citem->sub_item = (cmplr_item*)malloc( sub_item_base_sz1 );
    citem->str = new string[1];
    
    if(citem->sub_item == NULL){ cres.reason << "compilr:  error: could not initalize temp internal memory structure.\n"; return NULL;}
    citem->size_t.byte1 = init.byte2;
    citem->item.byte1 = init.byte1;
    
    for(int i = 0; i < sub_item_base_sz1; i++)
       citem->sub_item[i].item.byte1 = data[i];
       
    citem->str[0]=str;
    return citem;
 }
 
 void cmplrfree( struct cmplr_item *tmppt )
 {
     free(tmppt);
 }


 struct cmplr_item_2 *new_cmplr_item2()
 {
     struct cmplr_item_2 *citem2 = new cmplr_item_2[1];
     citem2->c_items = (cmplr_item*)malloc( cplr_item_buflen+1 );
     if(citem2->c_items == NULL){ cres.reason << "compilr:  error: could not initalize internal memory structures.\n"; return NULL;}
     citem2->j=cplr_item_buflen+1;
     
     return citem2;
 }

 void cmplrfree2( struct cmplr_item_2 *tmppt )
 {
     tmppt->j = 0;
     free(tmppt->c_items);
 }
 
 void cmplr_cleanup(bool fullFlush)
 {
     if(!init)
        return;
     
     cplr_item_sz1=0;
     cmplrfree2(cplrfreelist2); // TODO: fix seg fault
     cmplrfree(cplrfreelist1);
     
     if(fullFlush)
     {
         //Flush everything
         init=false;
     }
    cplrfreelist1=NULL;
 }

/*-----------------------*/
/* Initalize the Compilr library */
/*  This will be called automatically */
/*  upon calling any cmplr routines. */
/*-----------------------*/
 int cmplr_init()
 {
     cplrfreelist2 = new cmplr_item_2[1];
     cplrfreelist2->c_items = (cmplr_item*) malloc (cplr_item_buflen+1);
     if(cplrfreelist2->c_items == NULL){ cres.reason << "compilr:  error: could not initalize internal memory structures.\n"; return -1;}
     
     cplrfreelist2->j=cplr_item_buflen+1;
     init=true;
     return 0;
 }

/*-----------------------*/
/* Add item to a buffer. */
/*-----------------------*/
 int cmplr_add_item( struct cmplr_item *b2 )
 {
     if(cplr_item_sz1 > cplr_item_buflen+1){ cres.reason << "compilr:  error: could not add object item to buffer. Out of memory!\n"; return -1; }
     cplrfreelist2->c_items[cplr_item_sz1++]=(*b2);
     
     return 0;
 }
 
 long jmpLocation(Method &m)
 {
     
 }
 
 long returnLocation(Method &m)
 {
     
 }

 bool svmObjectIsDead(Object &obj)
 {
     
 }

 bool svmObjectHasInstance(Object &obj, int instance)
 {
     
 }

 void svmInitHeapObject(Object &obj, int _typedef_, u1 objsz_t, int gc_status)
 {
     
 }

/* debugging */
 void svmDumpObject(Object &obj){
     
 }

 bool isObjArray(Object &obj)
 {
     
 }

 void freeObj(Object &obj)
 {
     
 }
 
 
 
 