/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard IO Routines for scorpion
* 
*/
 
#ifndef sio_h
#define sio_h

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sstream>

typedef struct Sio BIO;
typedef std::stringstream buf;

extern buf io_buf;
#define MAX_BUF_SEGMENT ((156*1024)*(8*156))


enum // buf states
{
   buf_ok=0,    // standard state, the buffer was successfully pulled
   buf_trunk=1, // The buffer was pulled but was truncated
   buf_fail=2  // standard state, the buffer failed due to an io failure ( buffer is leaked )
};

/* Standard buffered IO */
struct Sio {
  int64_t n;    // R/W privlages 
  char* buf;       //  Buffered content
  uint64_t size_t, p; // buffer size in bytes & num of bytes read
  unsigned int data;    // helpful data about an io buf ( reset after each read & write )
};

uint64_t file_size(FILE *f);

/**
 * Return the next segment in the buf and free
 */
buf& buf_data(BIO* io);

void buff_init(BIO* io);

void buff_leak(BIO* b);

int buffS_read(const char *f, BIO* io);

int buffS_fill(std::string s, BIO* io);

int buffS_write(const char *f, BIO* io);

/* Depreciated */
char* buf_c_str(std::string b);

#endif // sio_h