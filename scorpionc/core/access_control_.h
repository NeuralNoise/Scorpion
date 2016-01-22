/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for providing access control
* to Scorpion objects, classes, and node structs
* 
*/
#ifndef access_control_h
#define access_control_h

#include <string>
using namespace std;

#define ACCESS_SPEC_SIZE 5

enum // access specs
{
    access_public = 0x0, access_private = 0x1,
    access_protected = 0x2,  access_static = 0x3, 
    access_read_only = 0x4
};

#define ACCESS_SPEC(s,i) (1 & (i >> s))
#define SET_ASPEC(s,x,i) ((i) ^= (-x ^ i) & (1 << s)) // s = spec, x = on/off, i = accessibility

struct access_priv
{
   int accessibility;  
};

void parse_access_spec(access_priv &priv);
string access_spec_itstr(int a);
string access_spec_strinfo(int a);
#endif // access_control_h