/*
 * Copyright (C) 2015 The Scorpion Programming Language
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Portions of the code surrounded by "// Begin Dalvik code" and
 * "// END Delvik code" are copyrighted and licensed separately, as
 * follows:
 *
 * The core software depicted in this library goes to 
 * the creator of SCZ(Simple Compression Utilities and Library)
 *
 * (http://scz-compress.sourceforge.net/) November 26, 2008
 *
 */
 #ifndef _COMPILR_COMPILER
 #define _COMPILR_COMPILER

 #include <sstream>
 #include <string>
 
 using namespace std;

 long Scz_get_file_length( FILE *fp );

/************************************************************************/
/* Cmplr_Compile_File - Compile input file to output file.		*/
/*  First argument is input file name(MUST HAVE .scorpion or .sn extn.).  Second argument is output file	*/
/*  name.  The two file names must be different.			*/
/*  The generated output file will produce a .o file (this file must be linked before runing) */
/************************************************************************/
int Cmplr_Compile_File( string infilename, char *outfilename );


/************************************************************************/
/* Cmplr_Compile_Buffer2File - Compile string input buffer	*/
/*  to an output file.  This routine is handy for applications wishing	*/
/*  to compile their output directly while saving to disk.		*/
/*  First argument is input string buffer,	*/
/*  third is the output file name to store to.				*/
/*  The generated output file will produce a .o file (this file must be linked before runing) */
/************************************************************************/
int Cmplr_Compile_Buffer2File( string buffer, char *outfilename );


/************************************************************************/
/* Cmplr_Compile_Buffer2Buffer - Compile string input buffer	*/
/*  to another buffer.  This routine is handy for applications wishing	*/
/*  to compile data on the fly, perhaps to reduce dynamic memory or	*/
/*  network traffic between other applications.				*/
/*  First argument is input array, second is the output buffer stream, */
/*  and the final parameter is a continuation flag.  Set true	*/
/*  if last file to compile, otherwise set false if more files to come.	*/
/************************************************************************/
int Cmplr_Compile_Buffer2Buffer( string inbuffer, stringstream &outbuffer, int lastbuf_flag );


/***************************************************************************/
/* Cmplr_Compile_File2Buffer - Compile input file to output buffer.  */
/*  This routine is handy for applications wishing to read compressed data */
/*  directly from disk and to compile the input directly while reading  */
/*  from the disk.             						   */
/*  First argument is input file name.  Second argument is output buffer   */
/*  Input file name(MUST HAVE .scorpion or .sn extn.) to be compiled.	*/
/**************************************************************************/
int Cmplr_Compile_File2Buffer( string infilename, stringstream &__outbuff__);
 
 #endif // _COMPILR_COMPILER