//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_ARCH_SAR_H
#define SCORPION_ARCH_SAR_H

#include <string>
using namespace std;

extern void setup();
extern void help();
extern void parseargs(int argc, const char **args);
extern int file_start, fsize;
extern string packagefile;

#endif //SCORPION_ARCH_SAR_H
