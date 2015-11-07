
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
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements. The ASF licenses this file to You
 * under the Apache License, Version 2.0 (the "License"); you may not use 
 * this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef SCORPION_TOKENIZER_H
#define SCORPION_TOKENIZER_H

#include "clib/filestream.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
using namespace std;


#define nullptr ((void *)0)

enum token_type {
    tok_unused = 0x1,
    tok_newline = 0x2,
    tok_eof =  -2,
    tok_unexpected_char = 0x3,
    tok_line_comment = 0x4,
    tok_multi_line_comment = 0x5,
    tok_identifier = 0x6,
    tok_reserved = 0x7,
    tok_unidentified = 0x8,
    tok_multi_char_sym = 0x9,
    tok_sym = 0x10,
    tok_string_literal = 0x11,
    tok_integer = 0x12,
    tok_decimal = 0x13,
};

template <class key, class T> class map {
    key* key_types;
    T* def_types;
    
    public:
       unsigned long size_t, p;
    
    public:
       int add(key key_type, T type){
            if(p >= size_t){
              cout << "scorpionc:  error: token index out of bounds.\n";
              exit(-1);
            }
             
           key_types[p] = key_type;
           def_types[p++] = type;
           
           return 0;
       }
       int set(unsigned long t){
           key_types = new (nothrow) key[t];
           def_types = new (nothrow) T[t];
           
           if(key_types == nullptr || def_types == nullptr)
              return -1;
        
           size_t=t;
           p=0;      
           return 0;
       }
       key key_at(unsigned long x){
             if(x >= size_t){
                cout << "scorpionc:  error: token index out of bounds.\n";
                exit(-1);
             }
             
             return key_types[x];
       }
       T def_at(unsigned long x){
             if(x >= size_t){
                cout << "scorpionc:  error: token index out of bounds.\n";
                exit(-1);
             }
           
             return def_types[x];
       }
       void dump_last()
       {
           if(!((p-1) < 0)){
              p--;
              key_types[p] = "";
              def_types[p++] = tok_unused;
           }
       }
       unsigned long size(){ return size_t; }
       void cln(){
           p=0;
           size_t=0;
           delete[] key_types;
           delete[] def_types;
       }
};

extern unsigned int LastChar;
class Tokenizer {
      string tokenStream;
      
    public:
      unsigned long p;
      long line, charptr;
      stringstream lineTxt;
      bool eof;
      
    public:
     map<string, int> tokens;
     void nextTok();
     long getLine(){ return line; }
     int getCharPtr(){ return charptr; }
     void setStream(string stream){ tokenStream = stream; }
     string& getStream(){ return tokenStream; }
     char getChar(){
         if(!(p < tokenStream.size())){
            return -2;
            eof = true;
         }
         charptr++;
         
         if(tokenStream.at(p) == '\n')
           line++;
         
         return tokenStream.at(p++); 
     }
     void nextLine(){
         line++;
         charptr=0;
         lineTxt.str("");
         for(unsigned long i = p; i < getStream().size(); i++){
             if(getStream().at(i) == '\n' || !((i + 1) < getStream().size()))
               break;
             else
               lineTxt << getStream().at(i);
         }
         cout << "line " << lineTxt.str() << endl;
     }
     void clean(){
         p=0;
         tokenStream="";
         tokens.cln();
     }
};


#endif // SCORPION_TOKENIZER_H

