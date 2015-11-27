//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_ARCH_U2_H
#define SCORPION_ARCH_U2_H

#include "Byte.h"

struct u2 {
   long byte1;
   long byte2;
};

struct u2_d {
   double byte1;
   double byte2;
};

extern bool cmp_u2(u2 a, u2 b);

#endif //SCORPION_ARCH_U4_H
