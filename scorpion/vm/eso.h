/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard Routines for processing a Scorpion
* image.
* 
*/
#ifndef eso_h
#define eso_h

#include <stdio.h>
#include <stdint.h>
#include <sstream>
#include "stypes.h"
#include "sio.h"
#include "sstate.h"
#include <string.h>

struct Header
{
     sstring     magic;
    uint16_t     minor_version[2];
    uint16_t     major_version[2];
     int64_t     target_plat_vers;
     int64_t     minimum_plat_vers;
     sstring     version_number;
        bool     debug;
        bool     logging;
      int8_t     log_precedence;
     sstring     log_file;
     sstring     application_id;
     sstring     permissions;
    uint64_t     file_size;            // The full size of the image section in bytes
     sstring     name;
    uint64_t     address_cap;        // how much are we allocating to the vm
    uint64_t     instruction_cap;
    uint64_t     method_size;
};

enum // header_flags
{
  flag_minor_version=1,
  flag_major_version=2,
  flag_target=3,
  flag_base_target=4,
  flag_version=5,
  flag_debuggable=6,
  flag_loggable=7,
  flag_log_level=8,
  flag_log_file=9,
  flag_app_id=11,
  flag_permissions=12,
  flag_image_size=13,
  flag_name=14,
  flag_address_cap=15,
  flag_method_cap=16,
  flag_instruction_cap=17,
  flag_eoh=18   // end of header flag
};

enum // e_limits
{
    e_limit_max_file_size = 0
};

class Eso
{
    public:
      Header header;
      std::string image;
      uint64_t n, p;
      int read(const char* file, BIO* io);
      int process(scorpion_state* v_state);
      string header_info();
};

#endif // eso_h