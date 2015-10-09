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
#ifndef SCORPION_ALLOC_BLOCKTABLE_H_
#define SCORPION_ALLOC_BLOCKTABLE_H_


  #define nullptr ((void *)0)
  
  // Block table constants
  #define STANDARD_TABLE_SIZE 1
  #define DATA_BLOCK 0
  #define METHOD_BLOCK 1
  #define STACK_BLOCK 2
  
  #include <new>          // std::nothrow
  #include <string>
  #include <iostream>
 
  using namespace std;
  
  
  class BlockTable;
  class ScorpionEnv;
  
/*
 * Initializes the block tabel; must be called before any other
 * svmBlockTable() functions.
 *
 * BlockTabel supports on 3 main types of block formats
 * - Standard Data Block
 * - Data Stack Block
 * - Standard Method Block
 *
 */
BlockTable svmBlockTableStartup(BlockTable table, long heapMinimumSize, long growthLimit, long methodLimit, long blocksz, long stacksz);


/*
* Reallocate a block table
*/
BlockTable svmReallocBlockTable(BlockTable table, long blocksz, long stacksz);

/*
* Get the allocated memory for a specific block type
*/
long svmGetBlockAddr(BlockTable table, long blockId);

/*
 * Tears down the entire BlockTable structure.
 */
void svmBlockTableShutdown(ScorpionEnv env);

/*
 * Resets all of the bytes in the block table to clean.
 */
void svmClearBlockTable(BlockTable table);

extern std::string proto;
/*
 * Returns the data of the relevent byte in the Block table, given
 * an address on the heap.
 */
double svmBlockFromAddr(BlockTable table, int blockId, long addr);

/*
 * Sets data of the relevent byte in the Block table, given
 * an address on the heap.
 */
void svmBlockToAddr(BlockTable table,int blockId, long addr, double data, string str);

/*
 * Returns true if addr points to a valid address in a block.
 */
bool svmIsValidAddr(BlockTable table, int blockId, long addr);

/*
* Class Block
* 
* This class is the base class for the scorpion heap 
* structure.
*
*     [     Data Block     ][ VM Stack Block ][  Method Block  ]
*
* DataBlock being the biggest of all the memory acquired by the Virtual machine.
*/
// Base class for data blocks
class Block {
    protected:
       string *name;
       double *child;
       long child_t, name_t;
       bool blockSuccess;
       
    public:
       void setSize(long child_sz, long name_sz){
          child = new (nothrow) double[child_sz];
          if(child == nullptr){
             blockSuccess = false;
             return;
          }
          
          
          name = new (nothrow) string[name_sz];
          if(name == nullptr){
             blockSuccess = false;
             return;
          }
          
          child_t = child_sz;
          name_t = name_sz;
          blockSuccess = true;
       }
};

/*
* Class DataBlock
*
* This class is the main area block of data that 
*/
class DataBlock: public Block {
     public:
        long getChildSize(){
           return child_t;
        }
        void setChild(long index, double data_in){
          child[index] = data_in;
        }
        double getChild(long index){
           return child[index];
        }
        bool blockSuccessful(){
           return blockSuccess;
        }
};

class MethodBlock: public Block {
     public:
        long getChildSize(){ // name size would be the same
           return child_t;
        }
        void setChild(long index, double data_in, string class_and_name){
           child[index] = data_in;
           name[index] = class_and_name; // getTime%Timer
        }
        double getChild(long index){
           return  child[index];
        }
        string getName(long index){
           return  name[index];
        }
        bool blockSuccessful(){
           return blockSuccess;
        }
};

class VMStackBlock: public Block {
    public:
        long getChildSize(){
           return child_t;
        }
        void setChild(long index, double data_in){
           child[index] = data_in;
        }
        double getChild(long index){
           return  child[index];
        }
        bool blockSuccessful(){
           return blockSuccess;
        }
};

class BlockTable {
    
    protected:
       long growthLimit;
       long minimumHeapLimit;
       long blockSize;
       bool initalized;
       long allocbytes;
       
    public:
      BlockTable(){ }
      DataBlock *mDataBlock;
      MethodBlock *mMethodBlock;
      VMStackBlock *mVMStackBlock;
      void setMaxGrowthLimit(long limit){
          growthLimit = limit;
      }
      void setMinimumHeapLimit(long limit){
          minimumHeapLimit = limit;
      }
      long getMaxGrowthLimit(){
          return growthLimit;
      }
      long getMinimumHeapLimit(){
          return minimumHeapLimit;
      }
      void setBlockSize(long sz){
          blockSize = sz;
      }
      long getBlockSize(){
          return blockSize;
      }
      bool isinitalized(){
          return initalized;
      }
      void setInitalized(bool status){
          initalized = status;
      }
      void setBytes(long var){
          allocbytes = var;
      }
      long getAllocBytes(){
         return allocbytes;
      }
};


#endif  // SCORPION_ALLOC_BLOCKTABLE_H_




