/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for data type processing in scorpion
* 
*/

#ifndef stypes_h
#define stypes_h

#include <stdlib.h> 
#include "string.h"

// This will be defined as our primative type of a string
typedef hash_set<char> sstring;
typedef int8_t bit;
typedef int16_t sshort;
typedef int8_t byte;
typedef int32_t sint;
typedef int64_t slong;
typedef float sfloat;
typedef double sdouble;
typedef bool sbool;
typedef int16_t schar;

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

enum // special types
{
  node = 0xf2be  
};

struct SType
{
    int type;
    union
    {
        sstring s_str;
        bit s_bit:1;
        sshort s_short;
        byte s_byte;
        sint s_int;
        slong s_long;
        sfloat s_float;
        sdouble s_double;
        sbool s_bool;
        schar s_char;
    };
};

struct SObject
{
   SType* o;
   slong size_t, hex_a;
   bit alloc:1;
   int8_t gc, array;
   
   sstring* name;
   struct SObject* elements; // for creation of node objects
};

struct scorpion_state;

extern void create_object(int type, SObject* o, slong size_t = 1, bool array = false);
extern double sValue(SObject* o, slong ndx = -1);
extern void delete_object(SObject* o);
extern void sSet(SObject* o, double v, slong ndx = -1);
extern bool isnumber(SObject* o1, SObject* o2);
extern bool isarray(SObject* o);

#endif // stypes_h