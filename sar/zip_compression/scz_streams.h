/************************************************************************/
/* SCZ_Streams - The core SCZ routines enable whole files or buffers to	*/
/*  be compressed or decompressed.  The following routines offer 	*/
/*  alternative methods for accessing SCZ compression incrementally, by */
/*  individual lines or XML tags, instead of whole file buffers.	*/
/*  They enable a .scz file to be opened for writing or reading, 	*/
/*  followed by many line-or-tag writes-or-reads, and finally closed.	*/
/*  Much like fopen, feof, fgets, fputs, and fclose.  			*/
/*  The SCZ equivalents are: 						*/
/*	Scz_File_Open( filename, mode );				*/
/*	Scz_Feof( sczfile );						*/
/*	Scz_ReadString( sczfile, delimiter, outstring, maxN );		*/
/*	Scz_WriteString( sczfile, string, N );				*/
/*	Scz_File_Close( sczfile );					*/
/*									*/
/* SCZ_Streams - LGPL License:						*/
/*   Copyright (C) 2001, Carl Kindman					*/
/*   This library is free software; you can redistribute it and/or	*/
/*   modify it under the terms of the GNU Lesser General Public		*/
/*   License as published by the Free Software Foundation; either	*/
/*   version 2.1 of the License, or (at your option) any later version.	*/
/*   This library is distributed in the hope that it will be useful,	*/
/*   but WITHOUT ANY WARRANTY; without even the implied warranty of	*/
/*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU	*/
/*   Lesser General Public License for more details.			*/
/*   You should have received a copy of the GNU Lesser General Public	*/
/*   License along with this; if not, write to the Free Software	*/
/*   Foundation, 59 Temple Place, Suite 330, Boston, MA 02111-1307	*/
/* 									*/
/*   For updates/info:  http://sourceforge.net/projects/scz-compress	*/
/*   Carl Kindman 11-21-2004     carl_kindman@yahoo.com			*/
/*   9-15-06 Includes fixes due to David Senterfitt.			*/
/************************************************************************/

#ifndef SCZ_STREAMS
#define SCZ_STREAMS

#include "scz_compress_lib.h"
#include "scz_decompress_lib.h"

struct SCZ_File
 {
  FILE *fptr;

  struct scz_item *buffer_hd, *buffer_tl, *rdptr;
  int bufsz, totalsz, eof, feof;
  char mode;
  unsigned char chksum;
 };



/************************************************************************/
/* Scz_File_Open - Open a file for reading or writing compressed data.	*/
/*  First argument is file-name.  Second argument is 'r' or 'w'.	*/
/*  Returns 0 on failure to open file.					*/
/************************************************************************/
struct SCZ_File *Scz_File_Open( char *filename, char *mode )
{
 struct SCZ_File *stateptr;

 stateptr = (struct SCZ_File *)malloc(sizeof(struct SCZ_File));
 if (mode[0]=='r')
  {
   stateptr->mode = 'r';
   stateptr->fptr = fopen(filename,"rb");
  }
 else if (mode[0]=='w')
  {
   stateptr->mode = 'w';
   stateptr->fptr = fopen(filename,"wb");
  }
 else {printf("SCZ ERROR: Bad file open mode '%s'\n", mode);  return 0;}
 if (stateptr->fptr==0) {printf("SCZ ERROR: Cannot open output file '%s'\n", filename); return 0;}
 stateptr->buffer_hd = 0;
 stateptr->buffer_tl = 0;
 stateptr->rdptr = 0;
 stateptr->bufsz = 0;
 stateptr->totalsz = 0;
 stateptr->chksum = 0;
 stateptr->eof = 0;
 stateptr->feof = 0;
 return stateptr;
}


/************************************************************************/
/* Scz_WriteString - Incrementally buffers data and compresses it to	*/
/*  an output file when appropriate.  This routine is handy for 	*/
/*  applications wishing to compress output while writing to disk.	*/
/*  Especially those creating only small amounts of data at a time.	*/
/*  First argument is SCZ_File pointer, as returned by Scz_File_Open(). */
/*  Second argument is input array, third is the array's length.	*/
/*									*/
/* This routine just adds to an scz buffer, until enough has accmulated */
/* to compress a block's worth.   After all writing is done, file must 	*/
/* be closed with Scz_File_Close().					*/
/************************************************************************/
int Scz_WriteString( struct SCZ_File *sczfile, unsigned char *buffer, int N );


/************************************************************************/
/* Scz_File_Close - Close a file after reading or writing compressed 	*/
/*  data.  Argument is scz_file to be closed.				*/
/************************************************************************/
void Scz_File_Close( struct SCZ_File *sczfile );



/************************************************************************/
/* Scz_Feof - Test for end of a compressed input file.			*/
/************************************************************************/
int Scz_Feof( struct SCZ_File *sczfile );


/************************************************************************/
/* Scz_ReadString - Incrementally reads compressed file, decompresses  	*/
/*  and returns the next line, xml-tag or xml-contents.			*/
/*  This routine is handy for applications wishing to read compressed 	*/
/*  data directly from disk and to uncompress the input directly while	*/
/*  reading from the disk.						*/
/*  First argument is SCZ_File pointer, as returned by Scz_File_Open(). */
/*  Second argument is the delimiter list. To return input by carriage- */
/*  return delineated lines, use "\n", to parse by XML tags, use "<>".	*/
/*  (Delimiters "<>" will be left in the returned tags.)		*/
/*  Third argument is null-terminated output character string, which    */
/*  must be pre-allocated.  This routine does not allocate the output   */
/*  string!  Forth parameter is the maximum length of returned strings, */
/*  IE. the length of the supplied output array.			*/
/************************************************************************/
int Scz_ReadString( struct SCZ_File *sczfile, char *delimiter, unsigned char *outbuffer, int maxN );


#endif // SCZ_STREAMS