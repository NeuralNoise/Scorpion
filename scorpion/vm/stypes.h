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
    primitive_string=0,
    primitive_short=1,
    primitive_byte=2,
    primitive_int=3,
    primitive_long=4,
    primitive_float=5,
    primitive_double=6,
    primitive_boolean=7,
    primitive_char=8,
    primitive_bit=9
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
   int8_t gc, array, alloc;
   
   sstring* name;
   struct SObject* elements; // for creation of node objects
};

struct scorpion_state;

extern void create_object(int type, SObject* o, slong size_t = 1, bool array = false);
extern double sValue(SObject* o, slong ndx = -1);
extern string s_strValue(SObject* o, slong ndx = -1);
extern void delete_object(SObject* o);
extern void sSet(SObject* o, double v, slong ndx = -1);
extern int sAt(SObject* o, long ndx);
extern void s_strSet(SObject* o, string v, slong ndx = -1);
extern bool isnumber(SObject* o);
extern bool isarray(SObject* o);
extern bool isstring(SObject* o);

#endif // stypes_h
