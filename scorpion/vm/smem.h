/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard  Subroutines for scorpion
* meory allocation
* 
*/
 
#ifndef smem_h
#define smem_h

/*  Do not call this directly */
void *scorpionM_realloc_(void *m, uint64_t old, uint64_t new);

#endif // smem_h
