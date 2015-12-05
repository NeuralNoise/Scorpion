//
// Created by bnunnall on 8/19/2015.
//

#include "Byte.h"

 long Byte::getbyte(Byte byte)
 {
    return byte.value;
 }

 void Byte::byte(Byte byte, long value)
 {
    byte.value = (long) value;
 }
