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
 #ifndef _COMPILR
 #define _COMPILR
 
 #include "../u1.h"
 #include "../u4.h"
 #include <string>
 #include <sstream>
 
 using namespace std;
 
 #define COMPILR_FAILURE (0x00003)
 
 /*
 * Compilr response data
 *
 * This data structure holds the response code from the 
 * compilr functions. Use this to check for errors, warnings, 
 * and to get useful information such as the compression size 
 * of your data;
 *
 */
 struct Compilr_Response {
     /* 
     * u2 size_t
     *
     * This will hold the data for the 
     * final size executable size of out application
     *
     * The final compiled size is calculated 
     * in bytes 
     *
     */
     u1 size_t;
     
     /* (Unused) */
     u4 data;// if there is any extra data to be returned
     
     stringstream _warnings_; // our buffer for if there are any warnings
     stringstream reason; // if compression fails, this will be populated with the reason why it failed
     
     /* 
     * Compare this code to the 
     * standard failure code (COMPILR_FAILURE)
     *
     * if (response == COMPILR_FAILURE) // compilation failed
     * else // continue
     *
     */
     int response; // the response from our library
 } ;
 
 extern Compilr_Response cres;
 
 /*
 * Scorpion compilation library
 *
 * This is the high level portion of the library.
 * We make this class to abstract the library from the 
 * confusing concepts of the library.
 *
 * This library can only be used to compile Scorpion
 *
 */
 class Compilr {
     
     public:
        void Compile(string file, string output);
        void Compile_Buffer2File(string buffer, string outfile);
        void Compile_Buffer2Buffer(string buffer, stringstream &__obuff__, bool lastbuf);
        void Compile_File2Buffer(string f, stringstream &__buf);
        void Link(string file, string output);
        void Link_Buffer2File(string buffer, string outfile);
        void Link_Buffer2Buffer(string buffer, stringstream &__obuff__, bool lastbuf);
        void Link_File2Buffer(string f, stringstream &__buf);
        void Cleanup(bool full_flush);
 };
 
#endif // _COMPILR 