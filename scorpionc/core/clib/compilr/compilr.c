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
 #include "cinclude.h"
 #include "compilr.h"
 #include "core.h"
 #include <string>
 #include <sstream>
 
 using namespace std;
 
 Compilr_Response cres;
 
 extern char* c_ptr(string data);
 
 void Compilr::Compile(string file, string output)
 {
    cres.response = Cmplr_Compile_File( file, c_ptr(output) );
 }
 
 void Compilr::Compile_Buffer2File(string buffer, string outfile)
 {
     
 }
 
 void Compilr::Compile_Buffer2Buffer(string buffer, stringstream &__obuff__, bool lastbuf)
 {
     
 }
 
 void Compilr::Compile_File2Buffer(string f, stringstream &__buf)
 {
     
 }
 
 void Compilr::Link(string file, string output)
 {
     
 }
 
 void Compilr::Link_Buffer2File(string buffer, string outfile)
 {
     
 }
 
 void Compilr::Link_Buffer2Buffer(string buffer, stringstream &__obuff__, bool lastbuf)
 {
     
 }
 
 void Compilr::Link_File2Buffer(string f, stringstream &__buf)
 {
     
 }
 
 void Compilr::Cleanup(bool full_flush)
 {
     cres.size_t.byte1=0;
     cres._warnings_.str("");
     cres.reason.str("");
     cres.response=0;
     cmplr_cleanup(full_flush);
 }
 
 