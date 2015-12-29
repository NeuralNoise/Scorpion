/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard IO Routines for scorpion
* 
*/

#ifndef stypes_h
#define stypes_h

#include <stdlib.h> 
#include "string.h"

// This will be defined as our primative type of a string
typedef hash_set<char> sstring;
typedef uint8_t bit;
enum // primitives
{
    primitive_string,
    primitive_short,
    primitive_byte,
    primitive_int,
    primitive_long,
    primitive_float,
    primitive_double,
    primitive_boolean,
    primitive_char,
    primitive_bit
};

struct SType
{
    int type;
    union
    {
        sstring s_str;
        bit s_bit:1;
    };
};

struct SObject
{
   SType* o;
   uint64_t size_t;
   
   const char* name;
};

#endif // stypes_h