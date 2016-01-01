/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for the scorpion
* virtual machine
* 
*/

#ifndef vm_h
#define vm_h

#include "sstate.h"
#include "stypes.h"

#define DMAX_P 16

void _stdout(string out, scorpion_state* state);
void scorpion_vexecute(scorpion_state* v_state);

void runtime_err(scorpion_state* v_state, string what, string err);
void segfault(scorpion_state* state);
void quit(scorpion_state* v_state);

#endif // vm_h