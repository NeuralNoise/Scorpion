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
 #ifndef _ZLIB
 #define _ZLIB
 
 #include "../u2.h"
 #include "../u4.h"
 #include <string>
 
 using namespace std;
 
 struct ZLib_Response {
     u2 size_t;
     u2 compressionRatio;
     u2 decompressionRatio;
     
     u4 data;
     
     string reason;
     int response;
 } ;
 
 extern ZLib_Response zres;
 
 /*
 * Zip compression library
 *
 * This is the high level portion of the library.
 * We make this class to abstract the library from the 
 * confusing concepts of the library.
 *
 */
 class Zip {
     
     public:
        void Compress(string file, string output);
        void Decompress(string file, string output);
        void Compress_Buffer2File(string buffer, string outfile);
        void Compress_Buffer2Buffer(string buffer, char** outbuffer, bool lastseg);
        void Decompress_File2Buffer(string file, char **buf);
        void Decompress_Buffer2Buffer(string buffer, char** outbuffer);
        void Cleanup();
 };
 
#endif // _ZLIB 