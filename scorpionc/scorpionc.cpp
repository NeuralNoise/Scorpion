/*
* Scorpion Compiler revision 7
* version 0.2.0
* build date: July 2015
*
* This compiler is created based
* on the most recent revision of the
* Scorpion programming language.
*
* For up-to-date documentation the the
* most recent Revision of Scorpion, please
* visit "https://github.com/AndroDevcd/Scorpion/wiki"
*
*
* Copyright 2015 Braxton Nunnally
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*/
#include "scorpionc.h"
#include "scorpionheader.h"
#include "libheader.h"
#include "clib/filestream.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

extern int devparse(string f, string contents);
extern int compile_size;
extern string compilefiles[];
extern void parse(string file, string src);

// TODO: Add sub dirs based on appdir: (build/outputs/exe|logs|sar)
int main(int argc, const char **args)
{
    setup();
    
    if(argc >= 2)
       parseargs(argc, args);
    else if(argc == 1)
       help();
    
    if(fsize > 0){
        int index = 0;
         string *files = new string[fsize];
         for(int i = file_start + 1; i < argc; i++){
             stringstream ss;
             ss << args[i];
             files[index++] = ss.str(); 
         } 
         
         if(hasbuild){
             int ret = 0;
             for(int i = 0; i < fsize; i++){
                 if(!FileStream::exists(files[i].c_str())){
                     cout << "Error: file '" << files[i] << "' is no such file or directory." << endl;
                     ret = -1;
                 }
                 else{
                     if(!FileStream::endswith(".scorpion", files[i])){
                           cout << "Error: file '" << files[i] << "' is not a scorpion source file." << endl;
                           ret = -1;
                     }
                 }
             }
             
             if(FileStream::endswith(".dev", build_file)){
                 stringstream ss;
                 ss << application_file << ".xso";
                 application_file = ss.str();
                 ss.str("");
                 
                 if(FileStream::exists(build_file.c_str())){
                     if(ret != 0){
                         cout << "Shutting down." << endl;
                         return -1;
                     }
                     
                     int response = devparse(build_file, FileStream::getfile(build_file.c_str())); // run the build script
                     if(response != 0){
                        cout << "BUILD FAILED" << endl;
                        return -1;
                     }
                     else
                        cout << "BUILD SUCCESSFUL" << endl;
                        
                     if(compile_size > 0){
                       // cout << "\nUnpackaging libraries...\n" << endl;
                        for(int i = 0; i < compile_size; i++){
                            stringstream ss;
                            ss << appdir << compilefiles[i];
                            cout << "\ncompiling " << compilefiles[i] << endl;
                           if(readheader(FileStream::getfile(ss.str().c_str())) == 0){
                               cout << endl;
                               // TODO: Compile library
                               for(int i = 0; i < lh.sourcecount.byte1; i++){
                                  // cout << "src " << fmap[i].contents << endl;
                                   parse(fmap[i].name, fmap[i].contents);
                               }
                           }
                           else{
                            cout << "Error: lib file: " << compilefiles[i] << " sar file format error!" << endl;
                            return -1;
                           }
                        }
                     }
                    // cout << "\ncompile source code" << endl;
                     // TODO: implment source compiler here
                     
                     sheader::makeheader();
                     // TODO: build and finish app make
                 }
                 else{
                    cout << "Error: could not find build script file. \nA fatal Error has occurred, shutting down." << endl;
                    return -1;
                 }
              }   
              else{
                 cout << "Error: build file is not a Dev script file!. Shutting down." << endl;
                 return -1;
              }
         }
         else {
            cout << "Error: build script file not provided. \nA fatal Error has occurred, shutting down." << endl;
            return -1;
         }
    }
    else
      cout << "Error: could not start scorpionc. \nA fatal Error has occurred, shutting down." << endl;
  return 0;
}
