#ifndef SCORPION_ARCH_ARRAYLIST_H
#define SCORPION_ARCH_ARRAYLIST_H

#include <string>
using namespace std;

extern bool hasstring(string str, string *arry, int size);

template <class T>
class ListAdapter {
     T* values;
     unsigned long size_t;
     bool init;
    
     public:
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
                values = new string[1];
                size_t = 0;
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
        long long indexof(T value)
        {
            for(long long i = 0; i < size_t; i++)
            {
                if(values[i] == value)
                   return i;
            }
           return -1;
        }
        long size(){ return size_t; }
        string valueAt(long i)
        {
            if(i >= size_t)
              return "null";
              
            else return values[i];
        }
};

#endif  //SCORPION_ARCH_ARRAYLIST_H