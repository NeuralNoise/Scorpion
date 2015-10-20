//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_HEADER_H
#define SCORPION_HEADER_H

#include "clib/u4.h"
#include "clib/u2.h"
#include "clib/u1.h"
#include <sstream>
#include <string>
using namespace std;

#define MAX_LIMIT ((12 * 164) - 32)
#define MAX_SIZE ((64 * 1020) * 1032)   // max image size per application file

extern stringstream spk; // the actual file that will hold packaged files
extern long SIZE, imagesize;  // the total permission size

struct header {
    u4            magic;
    u2            minor_version;
    u2            major_version;
    u1            target_dev_vers;
    u1            minimum_dev_vers;
    string        version_number;
    u1            debug;
    u1            logging;
    u1            log_precedence;
    string        log_file;
    string        application_id;
    string        *permissions;
    u1            filesize;            // The full size of the image section in bytes
    string        name;
    u1            method_size;
};

extern header h;

class sheader {
    public:
      static int makeheader();
};



#endif //SCORPION_HEADER_H
