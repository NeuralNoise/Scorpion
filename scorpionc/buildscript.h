//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_BUILD_SCRIPT_H
#define SCORPION_BUILD_SCRIPT_H

#include <sstream>
#include <string>
using namespace std;



extern string name;
extern int minDevVersion;
extern int targetDevVersion;
extern string version_num;
extern bool debug, loog;
extern int logprecedence;
extern string logfile;
extern int compile_size;
extern string compilefiles[];
extern int permission_size;
extern string io_permissions[];
extern int include_size;
extern string include;
extern string appdir, srcdir;
extern string applicationid, exportFile;

#endif //SCORPION_BUILD_SCRIPT_H