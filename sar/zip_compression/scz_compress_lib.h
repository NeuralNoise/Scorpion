/*************************************************************************/
/* SCZ_Compress_Lib.c - Compress files or buffers.  Simple compression.	*/
/*
/  This file contains the following user-callable routines:
/    Scz_Compress_File( *infilename, *outfilename );
/    Scz_Compress_Buffer2File( *buffer, N, *outfilename );
/    Scz_Compress_Buffer2Buffer( *inbuffer, N, **outbuffer, *M, lastbuf_flag );
/
/  See below for formal definitions.

  SCZ Method:
   Finds the most frequent pairs, and least frequent chars.
   Uses the least frequent char as 'forcing-char', and other infrequent 
   char(s) as replacement for most frequent pair(s).

   Advantage of working with pairs:  A random access incidence array 
   can be maintained and rapidly searched.
   Repeating the process builds effective triplets, quadruplets, etc..

   At each stage, we are interested only in knowing the least used 
   character(s), and the most common pair(s).

   Process for pairs consumes: 256*256 = 65,536 words or 0.26-MB. 
   (I.E. Really nothing on modern computers.)

   Process could be expanded to triplets, with array of 16.7 words or 
   67 MB.  Recommend going to trees after that.  But have not found
   advantages to going above pairs. On the contrary, pairs are faster
   to search and allow lower granularity replacement (compression).
 ---

 SCZ_Compress - LGPL License:
  Copyright (C) 2001, Carl Kindman
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  For updates/info, see:  http://sourceforge.net/projects/scz-compress/

  Carl Kindman 11-21-2004     carlkindman@yahoo.com
		7-5-2005	Added checksums and blocking.
		9-14-2006       Freeing fixes by David Senterfitt.
*************************************************************************/

#ifndef SCZ_COMPRESSLIB_DEFD_H
#define SCZ_COMPRESSLIB_DEFD_H


#include <stdio.h>

extern int *scz_freq; 


/*------------------------------------------------------------*/
/* Add an item to a value-sorted list of maximum-length N.    */
/* Sort from largest to smaller values.  (A descending list.) */
/*------------------------------------------------------------*/
void scz_add_sorted_nmax( struct scz_amalgam *list, unsigned char *phrase, int lngth, int value, int N );


/*------------------------------------------------------------*/
/* Add an item to a value-sorted list of maximum-length N.    */
/* Sort from smaller to larger values.  (An ascending list.)  */
/*------------------------------------------------------------*/
void scz_add_sorted_nmin( struct scz_amalgam *list, unsigned char *phrase, int lngth, int value, int N );


/*----------------------------------------------------------------------*/
/* Analyze a buffer to determine the frequency of characters and pairs. */
/*----------------------------------------------------------------------*/
void scz_analyze( struct scz_item *buffer0_hd, int *freq1, int *freq2 );

/*------------------------------------------------------*/
/* Compress a buffer, step.  Called iteratively.	*/
/*------------------------------------------------------*/
int scz_compress_iter( struct scz_item **buffer0_hd );

/*******************************************************************/
/* Scz_Compress - Compress a buffer.  Entry-point to Scz_Compress. */
/*  Compresses the buffer passed in.				   */
/* Pass-in the buffer and its initial size, as sz1. 		   */
/* Returns 1 on success, 0 on failure.				   */
/*******************************************************************/
int Scz_Compress_Seg( struct scz_item **buffer0_hd, int sz1 );


long Scz_get_file_length( FILE *fp );


/************************************************************************/
/* Scz_Compress_File - Compresses input file to output file.		*/
/*  First argument is input file name.  Second argument is output file	*/
/*  name.  The two file names must be different.			*/
/*									*/
/************************************************************************/
int Scz_Compress_File( char *infilename, char *outfilename );



/************************************************************************/
/* Scz_Compress_Buffer2File - Compresses character array input buffer	*/
/*  to an output file.  This routine is handy for applications wishing	*/
/*  to compress their output directly while saving to disk.		*/
/*  First argument is input array, second is the array's length, and	*/
/*  third is the output file name to store to.				*/
/*  (It also shows how to compress and write data out in segments for	*/
/*   very large data sets.)						*/
/*									*/
/************************************************************************/
int Scz_Compress_Buffer2File( unsigned char *buffer, int N, char *outfilename );


/************************************************************************/
/* Scz_Compress_Buffer2Buffer - Compresses character array input buffer	*/
/*  to another buffer.  This routine is handy for applications wishing	*/
/*  to compress data on the fly, perhaps to reduce dynamic memory or	*/
/*  network traffic between other applications.				*/
/*  First argument is input array, second is the array's length, the	*/
/*  third is the output buffer array, with return variable for array	*/
/*  length, and the final parameter is a continuation flag.  Set true	*/
/*  if last segment, otherwise set false if more segments to come.	*/
/*  This routine allocates the output array and passes back the size.	*/ 
/*									*/
/************************************************************************/
int Scz_Compress_Buffer2Buffer( char *inbuffer, int N, char **outbuffer, int *M, int lastbuf_flag );

#endif /* SCZ_COMPRESSLIB_DEFD */
