/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard structure for scorpion virtual machine
* 
*/

#ifndef state_h
#define state_h

#include <stdint.h>
#include "arraylist.h"
#include "sfunc.h"
#include "sio.h"
#include "eso.h"
#include "stypes.h"
#include "string.h"

/**
 * This structure will hold everything we need 
 * for our Scorpion virtual machine
 * 
 */
struct scorpion_state
{
    uint64_t pc, size_t, state;
    int64_t if_count, if_depth, 
            func_depth, sp;
    function_tracker func_tracker;
    ListAdapter<double> i_heap;
    ListAdapter<std::string> permission_list;
    
    SObject *heap;
    func* static_methods;
};

int emplode(scorpion_state* s_state, int N);
int Scorpion_Init_create_state( BIO* io, scorpion_state* s_state, Eso *reader, sstring* ScorpionOptions, int argc );

/**
 * This structure will hold everything we need 
 * for the entire vm. This represents a 
 * global state that will hold everything 
 * that is used globally.
 * 
 */
struct scorpion_gstate
{
    uint64_t s_ptr;  // "state pointer" this will point to wich state we are running
    BIO* io;  // our standard io buffer
};

extern scorpion_gstate gstate;

#endif // state_h