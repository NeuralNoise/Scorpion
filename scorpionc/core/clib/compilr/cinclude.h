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
 * A small portion of the software depicted in this library goes to 
 * the creator of C LTL(C++ Lexer Toolkit Library)
 *
 * (http://www.partow.net/programming/lexertk/index.html) 2001
 * Author - Arash Partow
 */
 
/**************************************************************************
 * cinclude.h - Function protoypes for User-Callable Compilr compilation routines. *
 * Compilr-version 1.0	11-9-15				  *
 * This library only compiles and links valid scorpion source and object files.    *
 **************************************************************************/

#ifndef C_INCLUDE
#define C_INCLUDE 

#include "../../Archive.h"
#include <string>
#include <sstream>
using namespace std;
#define CMPLR_VERSION 1.0
float compilr_version=CMPLR_VERSION;


/************************************************************************/
/* Cmplr_Compile_Buffer2Buffer - Compile string input buffer	*/
/*  to another buffer.  This routine is handy for applications wishing	*/
/*  to compile data on the fly, perhaps to reduce dynamic memory or	*/
/*  network traffic between other applications.				*/
/*  First argument is input array, second is the output buffer stream, */
/*  and the final parameter is a continuation flag.  Set true	*/
/*  if last file to compile, otherwise set false if more files to come.	*/
/************************************************************************/
int Cmplr_Compile_Zip( Archive &zip_archive, stringstream &outbuffer );

/************************************************************************/
/* Standard Linker routines for linking together multiple object files */
/*  Note: Not using cmpir_cleanup() before linking may cause memory failure */
/*  during linking process. */
/************************************************************************/

/************************************************************************/
/* Cmplr_Link_File - Link input file to output file.        */
/*  First argument is input file name.  Second argument is output file  */
/*  name.  The two file names must be different.                        */
/*  Input file name(MUST HAVE .o extn.) to be linked.	*/
/************************************************************************/
int Cmplr_Link_Buffer2Buffer(char *buffer, stringstream &__obuff__, bool lastbuf);
 

void cmpir_cleanup();	/* Call after last Compilr call to free temporarily allocated memory. */


#endif // C_INCLUDE