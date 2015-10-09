//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_ARCH_U4_H
#define SCORPION_ARCH_U4_H

#include "Byte.h"

struct u4 {
   long byte1;
   long byte2;
   long byte3;
   long byte4;
};

extern bool cmp_u4(u4 a, u4 b);

#endif //SCORPION_ARCH_U4_H
