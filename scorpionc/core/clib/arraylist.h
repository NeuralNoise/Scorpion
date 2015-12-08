#ifndef SCORPION_ARCH_ARRAYLIST_H
#define SCORPION_ARCH_ARRAYLIST_H

#include <string>
#include <iostream>
#include <stdlib.h>
using namespace std;

#define nullptr ((void *)0)
  
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
            if(!init) return;
            delete[] values;
            size_t=0;
            init=false;
        }
        bool _init() { return init; }
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
            if(newValues == nullptr){
                cout << "std: array_out_of_memory\n\tarray add[" << size_t << "]\n";
                exit(1);
            }
            
            for(long i = 0; i < size_t - 1; i++)
            {
                newValues[i] = values[i];
            }
            
            newValues[size_t - 1] = value;
            values = &newValues[0];
        }
        void insert(T value, long pos)
        {
             if(pos >= size_t || pos < 0){
                cout << "std: array_out_of_bounds\n\tarray insert[" << pos << "] >= size[" << size_t << "]\n";
                exit(1);
            }
            
            if(!init)
            {
               add(value);
               return;    
            } 
           
            size_t++;
            T* newValues = new T[size_t];
            if(newValues == nullptr){
                cout << "std: array_out_of_memory\n\tarray insert[" << size_t << "]\n";
                exit(1);
            }
            
            int i2 = 0;
            for(long i = 0; i < size_t-1; i++)
            {
                if(i == pos)
                {
                  newValues[i2] = value;
                  newValues[++i2] = values[i];
                }
                else
                  newValues[i2] = values[i];
                i2++;
            }
            
            values = newValues;
        }
        void remove(long pos)
        {
             if(pos >= size_t || pos < 0 || !init){
                cout << "std: array_out_of_bounds\n\tarray insert[" << pos << "] >= size[" << size_t << "]\n";
                exit(1);
             }
    
            size_t--;
            T* newValues = new T[size_t];
            if(newValues == nullptr){
                cout << "std: array_out_of_memory\n\tarray insert[" << size_t << "]\n";
                exit(1);
            }
            
            int i2 = 0;
            for(long i = 0; i < size_t-1; i++)
            {
                if(i == pos){}
                else
                  newValues[i2] = values[i];
                i2++;
            }
            
            values = newValues;
        }      
        void pushback()
        {
            if(!init)
              return;
            else if((size_t - 1) < 0) return;
            
            T* newValues = new T[size_t-1];
            if(newValues == nullptr){
                cout << "std: array_out_of_memory\n\tarray pushback[" << size_t << "]\n";
                exit(1);
            }
            
            for(long i = 0; i < size_t - 1; i++)
            {
                newValues[i] = values[i];
            }
        
            size_t--;
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
            if(i >= size_t || i < 0 || !init){
                cout << "std: array_out_of_bounds\n\tarray replace[" << i << "] >= size[" << size_t << "]\n";
                exit(1);
            }
            
            values[i] = value;
        }
        unsigned long size()
        { return ( (!init) ? 0 : size_t ); }
        T& valueAt(long i)
        {
            if(i >= size_t || i < 0 || !init){
                cout << "std: array_out_of_bounds\n\tarray at[" << i << "] >= size[" << size_t << "]\n";
                exit(1);
            }
              
            return values[i];
        }
};

#endif  //SCORPION_ARCH_ARRAYLIST_H