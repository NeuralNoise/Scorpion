/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard IO Routines for scorpion
* 
*/
 
#include <stdint.h>
#include <sstream>
#include <stdlib.h> 
#include <string.h>
#include <iosfwd>
#include "sio.h"

using namespace std;

/* This will be used to hold our buffer 
   This buf is overrided upon each read */
buf io_buf;

uint64_t file_size(FILE *fp)
{
    uint64_t len, cur;
    cur = ftell( fp );            /* remember where we are */
    fseek( fp, 0L, SEEK_END );    /* move to the end */
    len = ftell( fp );            /* get position there */
    fseek( fp, cur, SEEK_SET );   /* return back to original position */
      
    return ( len );
}

void buff_leak(BIO* b)
{
    io_buf.str("");
    if(b->n==-1) // Have we freed this buf before?
        return;
    
    b->n=-1;
    b->size_t=0;
    b->p=0;
    free( b->buf );
    b->buf = NULL;
}

/* Initalize a buffer */
void buff_init(BIO* io)
{
    io->n=-1; // we dont want to free an empty buffer!
    io->size_t=0;
    io->p=0;
    io->buf = NULL;
    io_buf.str("");
}


int buffS_read(const char *f, BIO* io)
{
    if(io->n != -1)
      return buf_fail;     // Has the prev buffer been leaked? Has the buffer been initalized?
    
    FILE* fp=NULL;
    int buf_stat = buf_ok;
    int64_t len;
    io_buf.str(""); 
    
    fp = fopen(f,"rb");
    if(fp == 0)
      return buf_fail;  // could not open file
    
    
    len = file_size(fp);
    if(len == -1) return buf_fail;
    if(len>MAX_BUF_SEGMENT) { len -= (len/8); buf_stat = buf_trunk; } // greatley reduce buf length
    
    //printf("buf size %u\n", (unsigned int)len);
       
    io->buf=(char*)malloc(len*sizeof(char));
    if( io->buf == NULL ) return buf_fail;   // failed to create buf
    io->n = 1;
    io->size_t = len;
    io->p=0;
    
    do {
      io->buf[io->p++] = getc(fp);
    }while(io->p<(io->size_t));
    fclose(fp);
    return buf_stat;
}

buf& buf_data(BIO* io)
{
    if( io->n == -1 )
      return io_buf; // we just return ""
    
    io_buf.str("");
    for( uint64_t i = 0; i < io->size_t; i++)
        io_buf << io->buf[i];
    return io_buf;
}

// fill internal buf ( mainly used for kernal system calls )
int buffS_fill(std::string s, BIO* io)
{
    if(io->n != -1)
      return buf_fail;     // Has the prev buffer been leaked? Has the buffer been initalized?
    
    int buf_stat = buf_ok;
    uint64_t len=s.size(), iter=0;  // we prevent buf from being NULL
    
    if(len>MAX_BUF_SEGMENT) { len -= (len/8); buf_stat = buf_trunk; } // greatley reduce buf length
    
    io->buf=(char*)malloc(len*sizeof(char));
    if( io->buf == NULL ) return buf_fail;   // failed to create buf
    
    io->n = 1;
    io->size_t = len;
    io->p=0;
    
    if(s.size() != 0)
    {
        do {
          io->buf[io->p++] = s.at( iter++ );
        }while(io->p<io->size_t);
    }
    
    return buf_stat;
}

/* Depreciated */
char* buf_c_str(std::string b)
{
    char *c = (char*)malloc((b.size()+1)*sizeof(char));
    if(c == NULL) { return c; } // We check for null later
   
   uint64_t iter=0; 
   while(iter<b.size())
   {
       c[iter] = b.at(iter);
       iter++;
   }
   c[b.size()] = '\0'; // add terminating char at end
    return c;
}

int buffS_write(const char *f, BIO* io)
{
    if(io->n != 1)
      return buf_fail;     // Has the prev buffer been leaked? Has the buffer been initalized?
    
    FILE* fp=NULL;
    int buf_stat = buf_ok;
    
    fp = fopen(f,"wb+");
    if(fp == 0)
      return buf_fail;  // could not open file
      
    if( io->buf == NULL ) return buf_fail;   // buf is null
    io->p=0;
   // cout << "buf size " << (io->size_t-1) << endl;
    do {
      fputc( io->buf[io->p++], fp );
    }while(io->p<(io->size_t));
    fclose(fp);
    return buf_stat;
}
