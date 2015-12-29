#ifndef SCORPION_VM_ARRAYLIST_H
#define SCORPION_VM_ARRAYLIST_H

#include <string>
#include <iostream>
#include <stdlib.h>
using namespace std;

#define nullptr ((void *)0)
  
extern void list_handler(std::string fail);

template <class T>
class ListAdapter { // This class is intregrated with the virtual machine
     T* values;
     T* newValues;
     T blank;
      unsigned long size_t;
      bool init, listener;
    
     public:
        ListAdapter(){
            init=false;
            listener = false;
            size_t=0;
        }
        void clear()
        {
            if(!init||size_t==0) return;
            delete[] values;
            size_t=0;
            init=false;
            listener = false;
        }
        bool _init() { return init; }
        void register_listener()
        { 
          listener = true; 
        }
        void _init_()
        {
		   init = false;	
           listener = false;
           size_t = 0;
	    }
        void add(T value)
        {
            if(!init)
            {
                values = new (nothrow) T[1];
                size_t = 0;
                init = true;
            }
            
            
            size_t++;
            newValues = new (nothrow) T[size_t];
            if(newValues == nullptr){
                if(listener) {  list_handler("OutOfMemory"); }
                cout << "std: array_out_of_memory\n\tarray add[" << size_t << "]\n";
                exit(1);
            }
            
            for(unsigned long i = 0; i < size_t - 1; i++)
            {
                newValues[i] = values[i];
            }
            
            newValues[size_t - 1] = value;
            values = &newValues[0];
        }
        void insert(T value, long pos)
        {
             if(pos >= size_t || pos < 0){
                if(listener) {  list_handler("ArrayOutOfBounds"); }
                cout << "std: array_out_of_bounds\n\tarray insert[" << pos << "] >= size[" << size_t << "]\n";
                exit(1);
            }
            
            if(!init)
            {
               add(value);
               return;    
            } 
           
            size_t++;
            newValues = new (nothrow) T[size_t];
            if(newValues == nullptr){
                if(listener) {  list_handler("OutOfMemory"); }
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
                if(listener) {  list_handler("ArrayOutOfBounds"); }
                cout << "std: array_out_of_bounds\n\tarray remove[" << pos << "] >= size[" << size_t << "]\n";
                exit(1);
             }
    
            size_t--;
            newValues = new (nothrow) T[size_t];
            if(newValues == nullptr){
                if(listener) {  list_handler("OutOfMemory"); }
                cout << "std: array_out_of_memory\n\tarray remove[" << size_t << "]\n";
                exit(1);
            }
            
            unsigned int i2 = 0;
            for(long i = 0; i < size_t; i++)
            {
                if(i == pos){ i2++; }
                
                  newValues[i] = values[i2++];
            }
            
            delete[] values;
            values = &newValues[0];
        }
        void pushback()
        {
            if(!init)
              return;
            else if((size_t - 1) < 0) return;
            
            newValues = new (nothrow) T[size_t-1];
            if(newValues == nullptr){
                if(listener) {  list_handler("OutOfMemory"); }
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
        long long indexof(T value)
        {
            for(long long i = 0; i < size_t; i++)
            {
                if(values[i] == value)
                   return i;
            }
           return 0;
        }
        void replace(T value, long i)
        {
            if(i >= size_t || i < 0 || !init){
                if(listener) {  list_handler("ArrayOutOfBounds"); }
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
                if(listener) {  list_handler("ArrayOutOfBounds"); }
                cout << "std: array_out_of_bounds\n\tarray at[" << i << "] >= size[" << size_t << "]\n";
                exit(1);
            }
              
            return values[i];
        }
};

#endif  //SCORPION_VM_ARRAYLIST_H
