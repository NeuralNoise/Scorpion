//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_COMPILER_H
#define SCORPION_COMPILER_H

#include <string>
using namespace std;

extern void setup();
extern void help();
extern void parseargs(int argc, const char **args);
extern int file_start, fsize;
extern int readheader(string file);
extern void unpack(string file);
extern string build_file, appdir;
extern string application_file;
extern bool hasbuild;

#endif //SCORPION_COMPILER_H