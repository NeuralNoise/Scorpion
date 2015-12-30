/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard structure for scorpion primitive strings
* 
*/

#ifndef string_h
#define string_h

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <sstream>
using namespace std;

template<class T>
struct hash_set // this will act as our generic string
{
    T* set;
    uint64_t length;
    std::string str()
    {
        if(set==NULL) return "";
        stringstream ss;
        for(uint64_t i = 0; i < length; i++)
          ss << (char)set[i];
        return ss.str();
    }
    int str(std::string svalue)
    {
        set = NULL;
        set =(T*)malloc(svalue.size()*sizeof(T));
        if(set==NULL) return 1;
        length=svalue.size();
        for(uint64_t i = 0; i < length; i++)
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

// TODO: Add string conversion subroutines here (strtoi) (itostr) (strtof) (ftostr) ...

#endif // string_h