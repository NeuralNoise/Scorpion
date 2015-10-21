/****************************************************************************/ 
/* SCZ_Decompress_lib.c - Decompress what SCZ_Compress.c compresses.
   Simple decompression algorithms for SCZ compressed data.

   This file contains the following user-callable routines:
     Scz_Decompress_File( *infilename, *outfilename );
     Scz_Decompress_File2Buffer( *infilename, **outbuffer, *M );
     Scz_Decompress_Buffer2Buffer( *inbuffer, *N, **outbuffer, *M );
  See below for formal definitions and comments.

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

  Carl Kindman 11-21-2004     carl_kindman@yahoo.com
                7-5-2005        Added checksums and blocking.
		9-14-2006	Freeing fixes by David Senterfitt.
*****************************************************************************/

#ifndef SCZ_DECOMPRESSLIB_DEFD_H
#define SCZ_DECOMPRESSLIB_DEFD_H

#include "scz_core.h"


/****************************************************************/
/* Decompress - Decompress a buffer.  Returns 1 on success, 0 	*/
/*  if failure.							*/
/****************************************************************/
int Scz_Decompress_Seg( struct scz_item **buffer0_hd );


/************************************************************************/
/* Scz_Decompress_File - Decompresses input file to output file.        */
/*  First argument is input file name.  Second argument is output file  */
/*  name.  The two file names must be different.                        */
/*                                                                      */
/************************************************************************/
int Scz_Decompress_File( char *infilename, char *outfilename );


/***************************************************************************/
/* Scz_Decompress_File2Buffer - Decompresses input file to output buffer.  */
/*  This routine is handy for applications wishing to read compressed data */
/*  directly from disk and to uncompress the input directly while reading  */
/*  from the disk.             						   */
/*  First argument is input file name.  Second argument is output buffer   */
/*  with return variable for array length. 				   */
/*  This routine allocates the output array and passes back the size.      */ 
/*                                                                         */
/**************************************************************************/
int Scz_Decompress_File2Buffer( char *infilename, char **outbuffer, int *M );




/*******************************************************************************/
/* Scz_Decompress_Buffer2Buffer - Decompresses input buffer to output buffer.  */
/*  This routine is handy for applications wishing to decompress data on the   */
/*  fly, perhaps to reduce dynamic memory or network traffic between other     */
/*  applications.                         				       */
/*  First argument is input array, second is the array's length, the third is  */
/*  the output buffer array, with return variable for array length.  	       */
/*  from the disk.             						       */
/*  This routine allocates the output array and passes back the size.          */ 
/*                                                                             */
/*******************************************************************************/
int Scz_Decompress_Buffer2Buffer( char *inbuffer, int N, char **outbuffer, int *M );

#endif /* SCZ_DECOMPRESSLIB_DEFD_H */
