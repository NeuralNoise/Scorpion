
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

#include "clib/filestream.h"
#include "Tokenizer.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
using namespace std;

bool isreservedword(string word)
{
  if(word == "module" || word == "import" || word == "global" || word == "static" 
  || word == "def" || word == "string" || word == "int" || word == "char" || word == "bool"
   || word == "short" || word == "double" || word == "float" || word == "while" || word == "return"
    || word == "if" || word == "else" || word == "true" || word == "false" || word == "in" || word == "out")
     return true;
  else   
    return false;
}

bool issymbol(char s)
{
  if(s = '{' || s == '}' || s == ')' || s == '(' || s == '[' || s == ']' || s == '\'' || s == '"' || s == '*' 
  || s == '\\' || s == '!' || s == '-' || s == '+' || s == '=' || s == '|' || s == '%' || s == ';' || s == ',' 
   || s == '.')
   return true;
  else 
    return false;
}

bool ismultisymbol(string s)
{
  if(s == "**" || s == "--" || s == "++" || s == "==" || s == "&&"
   || s == "..")
   return true;
   else
    return false;
}

bool isparen(char c)
{
    return (c == '"');
}

bool start = true;

unsigned int LastChar = ' ';
static double number;
static stringstream str;
void Tokenizer::nextTok(){
   static string word = " ";
  
  while (isspace(LastChar)){
    LastChar = getChar();
    
    if (LastChar == '\n')
      nextLine();
  }
  
  //cout << "LastCHar " << (char) LastChar << endl;
  
  if(LastChar == '"'){
        cout << "string\n";
        str.str("");
        str << (char) LastChar;
        
      for(long i = p; i < tokenStream.size(); p++){
            stringstream ss;
            ss << tokenStream.at(i);
            if(ss.str() == "\"" || ss.str() == "\n"){
               p++;
               break;
            }
            else
              str << ss.str();
            i++;
      }
      
        str << "\"";
        cout << str.str() << endl;
        LastChar = getChar(); // weird trick to not confuse the compiler
             
        tokens.add(str.str(), tok_string_literal);
        
        return;
    }
  
    if (LastChar == '@') {
      // Comment until end of line.
      do
        LastChar = getChar();
      while (LastChar != tok_eof && LastChar != '\n' && LastChar != '\r');
    
      if (LastChar != tok_eof)
        nextTok();
        return;
    }
  
  if (LastChar == '/'){
      stringstream lookahead;
      int chr = LastChar;
      
      lookahead << (char) LastChar;
      LastChar = getChar();
      lookahead << (char) LastChar;
      
      if(lookahead.str() == "/*"){
          int lookAhead;
          for(long i = p; i < tokenStream.size(); p++){
            stringstream ss;
            ss << tokenStream.at(i) << tokenStream.at(i + 1);
            if(ss.str() == "*/"){
               p+=2;
               break;
            }
            i++;
           }
           
           LastChar = getChar();
             
           if (LastChar != tok_eof)
             nextTok();
             return;
      }
      else{
          p--;
          LastChar = chr;
      }
  }
  
  if (isalpha(LastChar)) { // words: [a-zA-Z][a-zA-Z0-9]*
      word = (char) LastChar;
      LastChar = getChar();
      
      while (isalnum(LastChar) || LastChar == '_'){
        word += (char) LastChar;
        LastChar = getChar();
      }
      
      if (isreservedword(word)){ tokens.add(word, tok_reserved); }
      else{ tokens.add(word, tok_identifier); }
      
      cout << "word " << word << endl;
      
      return;
  }
  
  if (isdigit(LastChar)) {   // Number: [0-9.]+
          std::string NumStr;
          bool isint = true;
          do {
            NumStr += (char) LastChar;
            LastChar = getChar();
          } while (isdigit(LastChar) || LastChar == '.');
          
          if(LastChar == '.'){
              isint = false;
              NumStr += (char) LastChar;
              do {
                NumStr += (char) LastChar;
                LastChar = getChar();
              } while (isdigit(LastChar));
          }
          
          number = strtod(NumStr.c_str(), 0);
          if(isint)
            tokens.add(NumStr, tok_integer);
          else
            tokens.add(NumStr, tok_decimal);
          cout << "digit " << number << endl;
    }
  
  if (LastChar == tok_eof){
      tokens.add("eof", tok_eof);
      eof=true;
      return;
  }
  else if (issymbol(LastChar) && LastChar != tok_eof){
      int chr = LastChar;
      stringstream multi_char_symbol;
      
      multi_char_symbol << (char) LastChar;
      LastChar = getChar();
      multi_char_symbol << (char) LastChar;
      
      if(ismultisymbol(multi_char_symbol.str())){
          cout << "symbol " << multi_char_symbol.str() << endl;
          tokens.add(multi_char_symbol.str(), tok_multi_char_sym);
          return;
      }
      else{
          p--;
          LastChar = chr;
      }    
          
      word = LastChar;      
      tokens.add(word, tok_sym);
      LastChar = getChar();
      cout << "symbol " << word << endl;
      return;
  }
  else
       tokens.add(LastChar + "", tok_unexpected_char);
       cout << "unexpected character " << LastChar << endl;
       
   return;
}
