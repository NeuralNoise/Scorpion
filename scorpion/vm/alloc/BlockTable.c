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
 #include "BlockTable.h"
 #include "../../logservice/alog.h"
 #include "../../libxso/xso.h"
 #include "../scorpion_env.h"
 #include "../exception.h"
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <signal.h>
 #include <assert.h>
 #include <iostream>
 #include <sstream>
 
 using namespace std;
 
 bool BlockTableInitilized;
 
 extern ALog alog;


// TODO: make a TabelParser to control data in&out of heap
// TODO: Create humor.c in this directory for memory crashes as well as check to see if file exists in beginning of boot(if not creat it)
bool dataBlock(BlockTable table)
{ // TODO: create option for string size for setting (*name)
   table.mDataBlock->setSize(table.getBlockSize(), 0); // application data does not require a name
   return table.mDataBlock->blockSuccessful();
}

bool methodBlock(BlockTable table, long methodLimit)
{
   table.mMethodBlock->setSize(methodLimit, methodLimit);  // methods require both name and data
   return table.mMethodBlock->blockSuccessful();
}

bool stackBlock(BlockTable table, long size)
{
   if(size > PROTOTYPE_LIMIT){
     alog.ALOGV("error: requested stack size cannot be bigger thab PROTOTYPE_LIMIT");
     return false;
   }

   table.mVMStackBlock->setSize(size, 0); // stack data does not require a name
   return table.mVMStackBlock->blockSuccessful();
}

// simply apply allocation to all 3 block types
 BlockTable svmBlockTableStartup(BlockTable table, long heapMinimumSize, long growthLimit, long methodLimit, long blocksz, long stacksz)
 {
     Exception::trace.addproto("vm.internal.alloc.svmBlockTableStartup", "BlockTable", 1);

    if(!table.isinitalized())
       methodLimit *= 2;
       
     BlockTableInitilized = true;
     table.mDataBlock = new DataBlock[1];
     table.mMethodBlock = new MethodBlock[1];
     table.mVMStackBlock = new VMStackBlock[1];
     
     table.setMaxGrowthLimit(growthLimit);
     table.setMinimumHeapLimit(heapMinimumSize);
     table.setBlockSize(blocksz);
     
     if(!dataBlock(table) || !methodBlock(table, methodLimit) || !stackBlock(table, stacksz)){
        BlockTableInitilized = false;
        table.setInitalized(false);
        return table;
     }

   table.setInitalized(true);
   long d = table.getBlockSize();
   long v = stacksz;
   long m = methodLimit;
   long x = (d*2) + v + (m*2); // our allocated memory in total(bytes)
   table.setBytes(x);

   stringstream ss;
   ss << "allocated: " << x << " bytes.";

   alog.setClass("BlockTable");
   alog.ALOGD(ss.str());
   return table;
 }

BlockTable svmReallocBlockTable(BlockTable table, long blocksz, long stacksz){
     Exception::trace.addproto("vm.internal.alloc.svmReallocBlockTable", "BlockTable", 1);

     if(!table.isinitalized())
        Exception("Stub!", "BlockTableNotInitalizedException");

     if(blocksz < table.getMinimumHeapLimit() || blocksz > table.getMaxGrowthLimit())
         Exception("Failure to over/underallocate heap memory!", "BlockTable");
     else if(stacksz > PROTOTYPE_LIMIT)
         Exception("Faulure to set stack size greater than the max limit.", "BlockTable");
     
    free(table.mDataBlock);
    free(table.mMethodBlock);
    free(table.mVMStackBlock);
 
    return svmBlockTableStartup(table, table.getMinimumHeapLimit(), table.getMaxGrowthLimit(), table.mMethodBlock->getChildSize(), blocksz, stacksz);
}

/*
 * Tears down the entire BlockTable structure.
 */
void svmBlockTableShutdown(ScorpionEnv env){
    Exception::trace.addproto("vm.internal.alloc.svmBlockTableShutdown", "BlockTable", 1);
    alog.setClass("BlockTable");
    stringstream ss;
    ss << "shutting down environment: " << env.getName();
    alog.ALOGD(ss.str());

    if(!env.getBlockTable().isinitalized())
      Exception("Stub!", "BlockTableNotInitalizedException");
    
    free(env.getBlockTable().mDataBlock);
    free(env.getBlockTable().mMethodBlock);
    free(env.getBlockTable().mVMStackBlock);
    
    env.getBlockTable().setMaxGrowthLimit(0);
    env.getBlockTable().setMinimumHeapLimit(0);
    env.getBlockTable().setInitalized(false);
}
 
/*
 * Resets all of the bytes in the block table to clean.
 */
int svmClearBlockTable(ScorpionEnv env, long minHeap, long maxHeap){
    
    Exception::trace.addproto("vm.internal.alloc.svmClearBlockTable", "BlockTable", 1);
    
    if(!env.getBlockTable().isinitalized())
      Exception("Stub!", "BlockTableNotInitalizedException");

     long stacksz = env.getBlockTable().mVMStackBlock->getChildSize();

    /* De-Allocate block structures */
    free(env.getBlockTable().mDataBlock);
    free(env.getBlockTable().mMethodBlock);
    free(env.getBlockTable().mVMStackBlock);
    
    svmBlockTableStartup(env.getBlockTable(), minHeap, maxHeap, env.getMethodLimit(), env.getBlockTable().getBlockSize(), stacksz);
    
    if(!BlockTableInitilized)
       return -1;
    
    return 0;
}


long svmGetBlockAddr(BlockTable table, long blockId){

  if(!table.isinitalized())
      Exception("Stub!", "BlockTableNotInitalizedException");

  if(blockId == 0)
    return table.mDataBlock->getChildSize() * 2;
  else if(blockId == 1)
    return table.mMethodBlock->getChildSize() * 2;
  else
    return table.mVMStackBlock->getChildSize();
}

std::string proto;
/*
 * Returns the data of the relevent byte in the Block table, given
 * an address on the heap.
 */
double svmBlockFromAddr(BlockTable table, int blockId, long addr){

    if(!table.isinitalized())
      Exception("Stub!", "BlockTableNotInitalizedException"); 

    if(!svmIsValidAddr( table, blockId, addr)){
      stringstream ss;
      ss << "Well this is embarrasing, cannot access address @" << addr << endl 
         << "  note: memory cap set to " << svmGetBlockAddr(table, blockId) << " bytes";
      Exception(ss.str(), "BlockAddressRequestDeniedException");
     }

    switch( blockId ){
        case DATA_BLOCK: // dataBlock
           return table.mDataBlock->getChild(addr);
        case METHOD_BLOCK:
           proto = table.mMethodBlock->getName(addr);
           return table.mMethodBlock->getChild(addr);
        case STACK_BLOCK:
           return table.mVMStackBlock->getChild(addr);
        default:
           return -19098739424;
    }
}


string svmDataBlockGetStr(BlockTable table, long ptr){
    if(!table.isinitalized())
      Exception("Stub!", "BlockTableNotInitalizedException");

     if(!svmIsValidAddr( table, DATA_BLOCK, ptr)){
      stringstream ss;
      ss << "Well this is embarrasing, cannot access address @" << ptr << endl 
         << "  note: memory cap set to " << svmGetBlockAddr(table, DATA_BLOCK) << " bytes";
      Exception(ss.str(), "BlockAddressRequestDeniedException");
     }
     
     return table.mDataBlock->getChildStr(ptr);
}

void svmDataBlockSetStr(BlockTable table, long ptr, string data){
    if(!table.isinitalized())
      Exception("Stub!", "BlockTableNotInitalizedException");

     if(!svmIsValidAddr( table, DATA_BLOCK, ptr)){
      stringstream ss;
      ss << "Well this is embarrasing, cannot access address @" << ptr << endl 
         << "  note: memory cap set to " << svmGetBlockAddr(table, DATA_BLOCK) << " bytes";
      Exception(ss.str(), "BlockAddressRequestDeniedException");
     }
     
     table.mDataBlock->setChildStr(ptr, data);   
}


/*
 * Sets data of the relevent byte in the Block table, given
 * an address on the heap.
 */
void svmBlockToAddr(BlockTable table, int blockId, long addr, double data, string class_n_name){

     if(!table.isinitalized())
      Exception("Stub!", "BlockTableNotInitalizedException");

     if(!svmIsValidAddr( table, blockId, addr)){
      stringstream ss;
      ss << "Well this is embarrasing, cannot access address @" << addr << endl 
         << "  note: memory cap set to " << svmGetBlockAddr(table, blockId) << " bytes";
      Exception(ss.str(), "BlockAddressRequestDeniedException");
     }

     switch( blockId ){
        case DATA_BLOCK: // dataBlock
           table.mDataBlock->setChild(addr, data);
           return;
        case METHOD_BLOCK:
           table.mMethodBlock->setChild(addr, data, class_n_name);
           return;
        case STACK_BLOCK:
           table.mVMStackBlock->setChild(addr, data);
           return;
        default:
           alog.ALOGD("warning: block id unknown, data sot set!");
           return;
    }
}


/*
 * Returns true if addr points to a valid address in a block.
 */
bool svmIsValidAddr(BlockTable table, int blockId, long addr){

 //   if(!table.isinitalized())
 //     Exception("Stub!", "BlockTableNotInitalizedException");

    switch( blockId ){
        case DATA_BLOCK: // dataBlock
           return (addr < table.mDataBlock->getChildSize());
        case METHOD_BLOCK:
           return (addr < table.mMethodBlock->getChildSize());
        case STACK_BLOCK:
           return (addr < table.mVMStackBlock->getChildSize());
        default:
           return false;
    }
}

