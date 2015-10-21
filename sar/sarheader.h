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

#define MAX_LIMIT ((16* 164) - 32)
#define MAX_SIZE ((64 * 186) * 224)  // max byte size per file

extern stringstream archive; // the actual file that will hold packaged files

struct header {
    u4            magic;
    u2            minor_version;
    u2            major_version;
    u1            sourcecount;         // The number of source files there are
    string        *sourcefiles;        // The names of each source file
    u1            filesize;            // The full size of all files in bytes
};

extern header h;

class sarheader {
    public:
      static int makeheader(int size, string *files);
};



#endif //SCORPION_ARCH_SARHEADER_H
