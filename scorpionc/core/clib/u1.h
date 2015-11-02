//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_ARCH_U1_H
#define SCORPION_ARCH_U1_H

#include "Byte.h"

struct u1 {
   long byte1;
};

struct u1_d {
   double byte1;
};

extern bool cmp_u1(u1 a, u1 b);

#endif //SCORPION_ARCH_U4_H
