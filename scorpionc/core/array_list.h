/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Array list 
* 
*/
#ifndef array_list_h
#define array_list_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string>
#include <iostream>
#include <sstream>
using namespace std;

void core_exit(int e);

template<class T>
class ArrayList
{
   
   public:
     void list_start()
     {
         size_t = 0;
         set = NULL;
     }
     void add(T item)
     {
         size_t++;
         T* temp_set = NULL;
         temp_set = (T*)malloc(size_t*sizeof(T));
         
         if(temp_set == NULL)
         {
             cout << "std:  array_out_of_memory:\n\tadd() size: " << size_t << endl;
             core_exit(1);
         }
         
         for(unsigned long i = 0; i < size_t - 1; i++)
             temp_set[i] = set[i];
         temp_set[size_t-1]=item;
         
         if(set != NULL) free( set );
         set = &temp_set[0];
     }
     void replace(unsigned long ndx, T item)
     {
         if(ndx >= size_t)
         {
             cout << "std:  array_out_of_bounds:\n\treplace()[" << ndx << "] size: " << size_t << endl;
             core_exit(1);
         }
         set[ ndx ] = item;
     }
     bool contains(T item)
     {
         for(unsigned long i = 0; i < size_t; i++)
         {
            if(set[ i ] == item)
              return true;
         }
         return false;
     }
     bool empty()
     { return (size_t==0); }
     void back()
     {
         if(size_t == 0) { return; }
         else size_t--;
         
         T* temp_set = NULL;
         temp_set = (T*)malloc(size_t*sizeof(T));
         
         if(temp_set == NULL)
         {
             cout << "std:  array_out_of_memory:\n\tback() size: " << size_t << endl;
             core_exit(1);
         }
         
         for(unsigned long i = 0; i < size_t; i++)
             temp_set[i] = set[i];
         
         if(set != NULL) free( set );
         set = &temp_set[0];
     }
     T& get(unsigned long ndx)
     {
         if(ndx >= size_t)
         {
             cout << "std:  array_out_of_bounds:\n\tget()[" << ndx << "] size: " << size_t << endl;
             core_exit(1);
         }
         return set[ ndx ];
     }
     void remove(long ndx)
     {
         if(ndx >= size_t || ndx < 0 || size_t == 0){
             cout << "std:  array_out_of_bounds:\n\tremove()[" << ndx << "] size: " << size_t << endl;
             core_exit(1);
         }

        size_t--;
        T* temp_set = NULL;
        temp_set = new (nothrow) T[size_t];
        if(temp_set == NULL){
             cout << "std:  array_out_of_bounds:\n\tremove()[" << size_t << "]" << endl;
             core_exit(1);
        }
        
        unsigned int i2 = 0;
        for(long i = 0; i < size_t; i++)
        {
            if(i == ndx){ i2++; }
            
              temp_set[i] = set[i2++];
        }
        
        if(set != NULL) free( set );
        set = &temp_set[0];
     }
     void clear()
     {
         if(set == NULL || empty()) return;
         free( set );
         set = NULL;
         size_t = 0;
     }
     unsigned long size() { return size_t; }
   
   private:
     T* set;
     unsigned long size_t;
};

#endif // array_list_h
