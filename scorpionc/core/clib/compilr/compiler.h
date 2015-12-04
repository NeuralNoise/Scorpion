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

 #include "../../Archive.h"
 #include "lexr/lexr.h"
 #include <sstream>
 #include <string>
 
 using namespace std;

 struct Globals
 {
     int method_t, success, mode, eof, 
         scopeLevel, block_stack;
     std::string package, file;
     lexr::parser_helper lex;
     stringstream out;
     bool infunc, inclass, innamespace;
 };
 
 enum lex_Modes
 {
    mode_eat_endl = 0x1, mode_ignore_endl = 0x2
 };

 enum scope_Modes
 {
    mode_namespace = 0x1, mode_function = 0x2,
    mode_class = 0x3
 };

/************************************************************************/
/* Cmplr_Compile_Buffer2Buffer - Compile string input buffer	*/
/*  to another buffer.  This routine is handy for applications wishing	*/
/*  to compile data on the fly, perhaps to reduce dynamic memory or	*/
/*  network traffic between other applications.				*/
/*  First argument is input array, second is the output buffer stream, */
/*  and the final parameter is a continuation flag.  Set true	*/
/*  if last file to compile, otherwise set false if more files to come.	*/
/************************************************************************/
int Cmplr_Compile_Buffer2Buffer( Archive &zip_archive, string inbuffer, stringstream &outbuffer, int lastbuf_flag );
 
 #endif // _COMPILR_COMPILER