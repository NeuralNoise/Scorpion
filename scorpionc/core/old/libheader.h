//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_ARCH_SARHEADER_H
#define SCORPION_ARCH_SARHEADER_H

#include "clib/u4.h"
#include "clib/u2.h"
#include "clib/u1.h"
#include <sstream>
#include <string>
using namespace std;

struct lheader {
    u4            magic;
    u2            minor_version;
    u2            major_version;
    u1            sourcecount;         // The number of source files there are
    string        *sourcefiles;        // The names of each source file
};

extern lheader lh;
struct FileMap {
    string name;
    string contents;
};

extern FileMap *fmap;

class libheader {
    public:
      static int readheader(int size, string *files);
};



#endif //SCORPION_ARCH_SARHEADER_H