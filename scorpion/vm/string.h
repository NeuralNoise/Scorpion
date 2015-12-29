/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard structure for scorpion primitive strings
* 
*/

#ifndef string_h
#define string_h

#include <stdio.h>
#include <string.h>
using namespace std;

template<class T>
struct hash_set // this will act as our generic string
{
    T* set;
    uint64_t length;
    string str()
    {
        if(set==NULL) return "";
        stringstream ss;
        for(uint64_t i = 0; i < length-1; i++)
          ss << (char)set[i];
        return ss.str();
    }
    int str(string svalue)
    {
        set =(T*)malloc((svalue.size()+1)*sizeof(T));
        if(set==NULL) return 1;
        length=svalue.size()+1;
        for(uint64_t i = 0; i < length-1; i++)
          set[i] = (T)svalue.at(i);
        return 0;
    }
    void clear()
    {
        free( set );
        length = 0;
    }
    T at(unsigned int ndx)
    {
        if(ndx >= length)
         return 0;
        return set[ ndx ];
    }
    uint64_t size()
    { return length; }
};

#endif // string_h