/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard structure for scorpion virtual machine
* 
*/

#ifndef state_h
#define state_h

class Eso;
struct scorpion_state;

#include <stdint.h>
#include <stdlib.h>
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
    uint64_t pc, size_t, state, iheap_t,permission_t,
             stack_t, method_t, args_t;
    int64_t if_count, if_depth, 
            func_depth, sp,*stack;
    function_tracker func_tracker;
    
    double* i_heap;
    sstring* permission_list, *p_args;
    
    SObject *heap;
    SObject *node_map; /* i'll deal with this later */
    func* static_methods;
    
    void init() // zero out all values
    {
        pc=0;
        size_t=0;
        state=0;
        iheap_t=0;
        permission_t=0;
        if_count=0;
        if_depth=0;
        func_depth=0;
        sp=-1;
        i_heap = NULL;
        p_args = NULL;
        heap = NULL;
        stack = NULL;
        static_methods = NULL;
    }
    int alloc(int64_t base_addrs, int64_t _stack_t, int64_t _method_t)
    {
        heap = (SObject*)malloc(base_addrs*sizeof(SObject));
        stack = (int64_t*)malloc(_stack_t*sizeof(uint64_t));
        static_methods = (func*)malloc(_method_t*sizeof(func));
        
        if(heap == NULL || stack == NULL || static_methods == NULL)
          return 1;
          
        size_t=base_addrs;
        stack_t=_stack_t;
        method_t=_method_t;
        return 0;
    }
};

enum // vm_states
{
   state_normal=0,
   state_frozen=1,
   state_passive_frozen=2
};

int destroy_state(scorpion_state* s_state, int N);
int Scorpion_Init_create_state( BIO* io, scorpion_state* s_state, Eso *reader, 
       sstring* ScorpionOptions, int argc );
void Scorpion_Init_start_state(scorpion_state* s_state);
/**
 * This structure will hold everything we need 
 * for the entire vm. This represents a 
 * global state that will hold everything 
 * that is used globally.
 * 
 */
struct scorpion_gstate
{
    BIO* io;  // our standard io buffer
};

extern scorpion_gstate gstate;

#endif // state_h