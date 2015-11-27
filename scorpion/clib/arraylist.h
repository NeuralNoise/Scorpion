#ifndef SCORPION_ARCH_ARRAYLIST_H
#define SCORPION_ARCH_ARRAYLIST_H

#include <string>
#include <iostream>
#include <stdlib.h>
using namespace std;

extern bool hasstring(string str, string *arry, int size);

template <class T>
class ListAdapter {
     T* values;
      unsigned long size_t;
      bool init;
    
     public:
        ListAdapter(){
            init=false;
        }
        void clear()
        {
            delete[] values;
            size_t=0;
            init=false;
        }
        void add(T value)
        {
            if(!init)
            {
                values = new T[1];
                size_t = 0;
                init = true;
            }
            
            size_t++;
            T* newValues = new T[size_t];
            
            for(long i = 0; i < size_t - 1; i++)
            {
                newValues[i] = values[i];
            }
            
            newValues[size_t - 1] = value;
            values = &newValues[0];
        }
        bool contains(T value)
        {
            for(unsigned long i = 0; i < size_t; i++)
            {
                if(values[i] == value)
                   return true;
            }
           return false;
        }
        void replace(T value, long i)
        {
            if(i >= size_t){
                cout << "std: array_out_of_bounds\n\tarray at[" << i << "] >= size[" << size_t << "]\n";
                exit(1);
            }
            
            values[i] = value;
        }
        unsigned long size(){ return size_t; }
        T valueAt(long i)
        {
            if(i >= size_t){
                cout << "std: array_out_of_bounds\n\tarray at[" << i << "] >= size[" << size_t << "]\n";
                exit(1);
            }
              
            else return values[i];
        }
};

#endif  //SCORPION_ARCH_ARRAYLIST_H