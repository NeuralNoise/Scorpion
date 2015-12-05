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
 #include "compiler.h"
 #include "compilr.h"
 #include "Opcodes.h"
 #include "../zlib/zlib.h"
 #include "../../Archive.h"
 #include "../binary.h"
 #include "../arraylist.h"
 #include "core.h"
 #include "../filestream.h"
 #include "lexr/lexr.h"
 #include <sstream>
 #include <string>
 #include <iostream>
 
 using namespace std;

 Globals cglobals;
    /*
    * ***************************************
    * ***************************************
    *       Header File Symbols
    *
    *         minor version: 0xAF
    *         major version: 0xAE
    *             file size: 0xFF
    *    target dev version: 0xBC
    *      minimum_dev_vers: 0xFD
    *        version_number: 0xCE
    *                 debug: 0xCC
    *               logging: 0xDB
    *        log precedence: 0xFA
    *              log file: 0x3C
    *        application id: 0xB5
    *           permissions: 0xC1
    *
    * ***************************************
    * ***************************************
    */
    int _minor_version     = 0xAF;
    int _major_version     = 0xCE;
    int file_size          = 0xEF;
    int target_dev_vers    = 0xBC;
    int minimum_dev_vers   = 0xFD;
    int version_number     = 0xCE;
    int debugging          = 0xEC;
    int logging            = 0xFC;
    int log_precedence     = 0xFA;
    int log_file           = 0x3C;
    int application_id     = 0xB5; 
    int permissions        = 0xC1;
    int nameflag           = 0x7A;
    int methodsize         = 0xD9;

 struct XsoHeader {
    u4            magic;
    u2            minor_version;
    u2            major_version;
    u1            target_dev_vers;
    u1            minimum_dev_vers;
    string        version_number;
    u1            debug;
    u1            logging;
    u1            log_precedence;
    string        log_file;
    string        application_id;
    string*       permissions;
    int           permission_t;
    u1            filesize;            // The full size of the image section in bytes
    string        name;
    u1            method_size; 
 };
 
 struct object_attrib
 {
     int access;
     bool isStatic, isConst;
 };
 
 void error(lexr::parser_helper &lex)
 {
     cout << cglobals.file << ":" << lex.lexer().line_t << "  error: " << cglobals.out.str() << endl;
     cglobals.success = 1;
     cglobals.out.str("");
 }
 
 lexr::token getNextToken(lexr::parser_helper &lexer){ 
     if(lexer.lexer().token_sz1<lexer.lexer().size()){
       
       lexr::token t = lexer.lexer()[lexer.lexer().token_sz1++];
       if(cglobals.mode == mode_eat_endl)
       { 
          while(t.type == t.e_line)
          { 
             t = lexer.lexer()[lexer.lexer().token_sz1++]; 
             lexer.lexer().line_t++; 
          }
       }
       
       if(!(lexer.lexer().token_sz1<lexer.lexer().size())){
         cglobals.eof=1;
         return lexer.lexer()[0];
       }
       else if(t.type == t.e_error) { 
        cglobals.out << "Stray `" << t.value << "` in program.";
        error(lexer);
        return getNextToken(lexer);
       }
       else
         return t;
     }
     else {
         cglobals.eof=1;
         return lexer.lexer()[0];
     }
     
}

 namespace memoryhelper 
 {
    namespace objecthelper
    {
       Object at(long i)
       { return objects.valueAt(i); }
     
       bool ocmp(Object &o)
       {
          if(!objects._init())
            return false;
            
          for(long i =0; i < objects.size(); i++)
          {
             if(at(i).type == o.type && at(i).name == o.name)
             {
                return (at(i).package == o.package && at(i)._namespace == o._namespace 
                        && at(i).parentclass == o.parentclass);
             }
          }
          
          return false;
       }
     
       bool exists(Object &o)
       { return ocmp(o); }
       
       bool find(std::string name, int type, bool isStatic, bool isarray, int access, 
                 std::string package, std::string _namespace, std::string parentclass)
       { 
           Object o;
           o.name = name;
           o.type = type;
           o.isStatic = isStatic;
           o.isarray = isarray;
           o.access = access;
           o.package = package;
           o._namespace = _namespace;
           o.parentclass = parentclass;
           return ocmp(o); 
       }
       
       bool create(std::string name, int type, bool isStatic, bool isarray, int access, 
                 std::string package, std::string _namespace, std::string parentclass)
       {
           Object o;
           o.name = name;
           o.type = type;
           o.isStatic = isStatic;
           o.isarray = isarray;
           o.access = access;
           o.package = package;
           o._namespace = _namespace;
           o.parentclass = parentclass;
           
           if(ocmp(o))
             return false;
           else {
               objects.add(o);
               return true;
           }
       }
    }
    
    namespace methodhelper
    {
      
    }
    
    namespace namespacehelper 
    {
       _namespace at(long i)
       { return namespaces.valueAt(i); }
       
       bool ncmp(_namespace &o)
       {
          if(!namespaces._init())
            return false;
            
          for(long i =0; i < namespaces.size(); i++)
          {
             if(at(i).name == o.name && at(i).parent == o.parent)
             {
                return true;
             }
          }
          
          return false;
       }
       
       bool exists(_namespace &o)
       { return ncmp(o); }
    }
    
    namespace helper
    {
       bool reserved(std::string symbol);    
       std::string reverse(string str);
       bool accessspecifier(std::string symbol);
       int strtoaccess(std::string s);
       bool _typedef(std::string symbol);
       
       std::string parse_dot_notation(lexr::parser_helper& lex, bool dotfirst, std::string breaker, const bool asterisk = false)
       {
          lexr::token temp_t;
          bool needsWord=false;
          int asterisk_flag = 0;
          stringstream word;
          
          for( ;; )
          {
              temp_t = getNextToken(lex);
              if(temp_t.value == ".")
              {
                 if(asterisk_flag==5)
                   asterisk_flag++;
                   
                  if(needsWord && !dotfirst)
                  {
                     cglobals.out << "Expected symbol before '.'.";
                     error(cglobals.lex);
                  }
                  
                  needsWord = true;
                  dotfirst = false;
                  word << ".";
              }
              else if(temp_t.type == temp_t.e_symbol)
              {
                 if(asterisk_flag==5)
                   asterisk_flag++;
                   
                 word << temp_t.value;
                 needsWord = false;
              }
              else if(temp_t.value == breaker)
              {
                 if(needsWord)
                 {
                    cglobals.out << "Expected symbol before '" << breaker << "'.";
                    error(cglobals.lex);
                 }
                 
                 if(asterisk_flag > 5)
                 {
                    cglobals.out << "Expected ';' immediatley after '*'.";
                    error(cglobals.lex);
                 }
                break;
              }
              else if(temp_t.value == "*" & asterisk)
              {
                 asterisk_flag = 5;
                 word << "*";
              }
              else if(temp_t.type == temp_t.e_eof)
              {
                 cglobals.out << "Unexpected end of file.";
                 error(cglobals.lex);
                 cglobals.eof=1;
              }
              else {
                 needsWord = true;
                 cglobals.out << "Unexpected symbol '" << temp_t.value << "', expecting '.' or ';'.";
                 error(cglobals.lex);
              }
          }
          return word.str();
       }
       
       void parse_access_specifier(lexr::parser_helper& lex, object_attrib &atribs, lexr::token &temp_t)
       {
          temp_t = getNextToken(lex);
          if(temp_t.value == "static"){
            atribs.isStatic = true;
            temp_t = getNextToken(lex);
          }
          else atribs.isStatic = false;
          
          if(temp_t.value == "const"){
              atribs.isConst = true;
              temp_t = getNextToken(lex);
          }
          else atribs.isConst = false;
          
       }
       
       void parse_string_decliration(lexr::parser_helper& lex, std::string var)
       {
           lexr::token temp_t;
           bool plus=false, needStr=false;
           stringstream ss;
           int bracket_stack=0;
           for( ;; )
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
                 break;
               
               if(temp_t.type == temp_t.e_string){
                   plus = false;
                   ss << temp_t.value;
                   needStr = false;
               }
               else if(temp_t.type == temp_t.e_number){
                   if(needStr)
                   {
                       cglobals.out << "Expected string literal after previous numeric literal.";
                       error(cglobals.lex);
                   }
                   
                   needStr = true;
                   plus = false;
                   ss << temp_t.value;
               }
               else if(temp_t.value == "+"){
                   if(plus)
                   {
                       cglobals.out << "Expected string literal after previous '+'.";
                       error(cglobals.lex);
                   }
                   plus = true;
               }
               else if(temp_t.value == ";" || reserved(temp_t.value)){
                   if(reserved(temp_t.value)){
                     lex.lexer().token_sz1--;
                       cglobals.out << "Expected ';' before symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                     
                   if(plus)
                   {
                       cglobals.out << "Expected string literal after previous '+'.";
                       error(cglobals.lex);
                   }
                   
                   if(bracket_stack != 0)
                   {
                       cglobals.out << "Expected ')' at end of statement.";
                       error(cglobals.lex);
                   }
                   cout << "string " << var << " = \"" << ss.str() << "\"" << endl;
                   break;
               }
               else if(temp_t.value == "(")
                   bracket_stack++;
               else if(temp_t.value == ")")
                   bracket_stack--;
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(cglobals.lex);
               }
           }
       }
       
       void parse_return_decliration(lexr::parser_helper& lex)
       {
           lexr::token temp_t;
           bool plus, needStr;
           stringstream ss;
           for( ;; )
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
                 break;
               if(temp_t.type == temp_t.e_string){
                   plus = false;
                   ss << temp_t.value;
                   needStr = false;
               }
               else if(temp_t.type == temp_t.e_number){
                   if(needStr)
                   {
                       cglobals.out << "Expected string literal or qulafied symbol after previous numeric literal.";
                       error(cglobals.lex);
                   }
                   
                   needStr = true;
                   plus = false;
                   ss << temp_t.value;
               }
               else if(temp_t.type == temp_t.e_symbol){
                   plus = false;
                   ss << "$[v" << temp_t.value << "|";
                   needStr = false;
               }
               else if(temp_t.value == "+"){
                   if(plus)
                   {
                       cglobals.out << "Expected string literal or qulafied symbol after previous '+'.";
                       error(cglobals.lex);
                   }
                   plus = true;
               }
               else if(temp_t.value == ";" || reserved(temp_t.value)){
                   if(reserved(temp_t.value)){
                     lex.lexer().token_sz1--;
                       cglobals.out << "Expected ';' before symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                     
                   if(plus)
                   {
                       cglobals.out << "Expected string literal or qulafied symbol after previous '+'.";
                       error(cglobals.lex);
                   }
                   cout << "return \"" << ss.str() << "\"" << endl;
                   break;
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(cglobals.lex);
               }
           }
       }
       
       void parse_byte_decliration(lexr::parser_helper& lex, std::string var)
       {
           lexr::token temp_t;
           bool plus=false, needNum=false, singleNum=true;
           stringstream ss;
           int bracket_stack=0;
  	   double maxValue = 127, minValue = -128;        

	   for( ;; )
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
                 break;
               
               if(temp_t.type == temp_t.e_number){
                   if(needNum)
                   {
                       cglobals.out << "Expected string literal after previous numeric literal.";
                       error(cglobals.lex);
              	       singleNum=false;
	           }
                   
                   needNum = true;
                   plus = false;
                   ss << temp_t.value;
               }
               else if(temp_t.value == ",")
	       {
		  temp_t = getNextToken(lex);
		  
	       }
               else if(temp_t.value == "+"){
                   if(plus)
                   {
                       cglobals.out << "Expected string literal after previous '+'.";
                       error(cglobals.lex);
                   }
                   plus = true;
                   singleNum=false;
               }
               else if(temp_t.value == ";" || reserved(temp_t.value)){
                   if(reserved(temp_t.value)){
                     lex.lexer().token_sz1--;
                       cglobals.out << "Expected ';' before symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                     
                   if(plus)
                   {
                       cglobals.out << "Expected string literal after previous '+'.";
                       error(cglobals.lex);
                   }
                   
                   if(bracket_stack != 0)
                   {
                       cglobals.out << "Expected ')' at end of statement.";
                       error(cglobals.lex);
                   }
                
  		   if(singleNum)
		   {
		       double num = atoi(ss.str().c_str());
		       if(num > maxValue || num < minValue)
		       {
			   cglobals.out << "Constant value '" << num << "' cannot be converted into `byte`.";
                           error(cglobals.lex);
		       }
		   }
  		
		   cout << "string " << var << " = \"" << ss.str() << "\"" << endl;
                   break;
               }
               else if(temp_t.value == "(")
                   bracket_stack++;
               else if(temp_t.value == ")")
                   bracket_stack--;
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(cglobals.lex);
               }
           }
       }
       
       void parse_short_decliration(lexr::parser_helper& lex, std::string var)
       {
           
       }
       
       void parse_char_decliration(lexr::parser_helper& lex, std::string var)
       {
           
       }
       
       void parse_int_decliration(lexr::parser_helper& lex, std::string var)
       {
           
       }
       
       void parse_long_decliration(lexr::parser_helper& lex, std::string var)
       {
           
       }
       
       void parse_float_decliration(lexr::parser_helper& lex, std::string var)
       {
           
       }
       
       void parse_double_decliration(lexr::parser_helper& lex, std::string var)
       {
           
       }
       
       void parse_typedef_block(lexr::parser_helper& lex, std::string type, object_attrib atribs)
       {
           lexr::token temp_t = getNextToken(lex);
           std::string var=temp_t.value;
           if(temp_t.type != temp_t.e_symbol)
           {
               cglobals.out << "Excpected unqualified symbol before '" << temp_t.value << "'.";
               error(cglobals.lex);
           }
           else if(reserved(var))
           {
               cglobals.out << "Cannot assign reserved word to " << type << " variable.";
               error(cglobals.lex);
           }
           
           temp_t = getNextToken(lex);
           if(temp_t.value == "[")
           {
              temp_t = getNextToken(lex);
              if(temp_t.value == "]")
              {
                 temp_t = getNextToken(lex);
              }
              else if(temp_t.type == temp_t.e_number){
                  cout << "array[" << temp_t.value << "]\n";
                  std::string num=temp_t.value;
                  temp_t = getNextToken(lex);
                  if(temp_t.value != "]")
                  {
                      cglobals.out << "Excpected ']' after numeric literal '" << type << "-array::[" << num << "]'.";
                      error(cglobals.lex);
                  }
                  temp_t = getNextToken(lex);
              }
              else {
                  cglobals.out << "Excpected ']' before '" << temp_t.value << "'.";
                  error(cglobals.lex);
              }
           }
           
           if(temp_t.value != "=" && temp_t.value != ";")
           {
               cglobals.out << "Excpected '=' before '" << temp_t.value << "'.";
               error(cglobals.lex);
           }
           
           if(temp_t.value == ";")
              lex.lexer().token_sz1--;
           
           if(type == "string")
             memoryhelper::helper::parse_string_decliration(lex, var);
           else if(type == "byte")
             memoryhelper::helper::parse_byte_decliration(lex, var);
           else if(type == "short")
             memoryhelper::helper::parse_short_decliration(lex, var);
           else if(type == "char")
             memoryhelper::helper::parse_char_decliration(lex, var);
           else if(type == "int")
             memoryhelper::helper::parse_int_decliration(lex, var);
           else if(type == "long")
             memoryhelper::helper::parse_long_decliration(lex, var);
           else if(type == "float")
             memoryhelper::helper::parse_float_decliration(lex, var);
           else if(type == "double")
             memoryhelper::helper::parse_double_decliration(lex, var);
       }
       
       void parse_method_args(lexr::parser_helper& lex)
       {
           lexr::token temp_t;
           bool comma;
           
           for( ;; )
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
                 break;
               if(temp_t.value == ")")
               {
                   if(comma)
                   {
                       cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                   break;
               }
               else if(temp_t.type == temp_t.e_symbol)
               {
                   comma = false;
                   if(!reserved(temp_t.value))
                   {
                       cglobals.out << "Unexpected qualified symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                   
                   temp_t = getNextToken(lex);
                   if(temp_t.value == "[")
                   {
                       temp_t = getNextToken(lex);
                       if(temp_t.type == temp_t.e_number)
                       {
                           cglobals.out << "Cannot have array size in function argument definition '[" << temp_t.value << "]'.";
                           error(cglobals.lex);
                       }
                       else if(temp_t.value == "]")
                       {
                           
                       }
                       temp_t = getNextToken(lex);
                   }
                   
                   if(temp_t.type != temp_t.e_symbol)
                   {
                       cglobals.out << "Unexpected unqualified symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                   
               }
               else if(temp_t.value == ",")
               {
                   if(comma)
                   {
                       cglobals.out << "Unexpected qulafied symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                   
                   comma = true;
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(cglobals.lex);
               }
           }
       }
       
       void parse_method_block(lexr::parser_helper& lex, int block_begin)
       {
           cglobals.block_stack++;
           lexr::token temp_t;
           while( cglobals.block_stack!=block_begin)
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
                 break;
               if(temp_t.value == "}")
                  cglobals.block_stack--;
               else if(reserved(temp_t.value))
               {
                   if(accessspecifier(temp_t.value))
                   {
                       object_attrib atribs;
                       atribs.access = strtoaccess(temp_t.value);
                       memoryhelper::helper::parse_access_specifier(cglobals.lex, atribs, temp_t);
                       if(_typedef(temp_t.value))
                       {
                           memoryhelper::helper::parse_typedef_block(cglobals.lex, temp_t.value, atribs);
                       }
                   }
                   else if(temp_t.value == "return")
                           memoryhelper::helper::parse_return_decliration(cglobals.lex);
                   else {
                       cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(cglobals.lex);
               }
           }
       }
       
       void parse_class_block(lexr::parser_helper& lex, int block_begin)
       {
           cglobals.block_stack++;
           lexr::token temp_t;
           while( cglobals.block_stack!=block_begin)
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
                 break;
               if(temp_t.value == "}")
                  cglobals.block_stack--;
               else if(reserved(temp_t.value))
               {
                   if(accessspecifier(temp_t.value))
                   {
                       object_attrib atribs;
                       atribs.access = strtoaccess(temp_t.value);
                       memoryhelper::helper::parse_access_specifier(cglobals.lex, atribs, temp_t);
                       if(temp_t.value == "class"){
                           temp_t = getNextToken(lex);
                           if(temp_t.type != temp_t.e_symbol){
                               cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           cout << "class " << temp_t.value << endl;
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "{"){
                               cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           memoryhelper::helper::parse_class_block(cglobals.lex, cglobals.block_stack);
                       }
                       else if(_typedef(temp_t.value))
                       {
                           object_attrib atribs;
                           memoryhelper::helper::parse_typedef_block(cglobals.lex, temp_t.value, atribs);
                       }
                       else if(temp_t.value == "def"){
                           temp_t = getNextToken(lex);
                           if(temp_t.type != temp_t.e_symbol){
                               cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }   
                               
                           
                           cout << "def " << temp_t.value << endl;
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "("){
                               cglobals.out << "Expected '(' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           memoryhelper::helper::parse_method_args(cglobals.lex);
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "{"){
                               cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                          memoryhelper::helper::parse_method_block(cglobals.lex, cglobals.block_stack);
                           
                       }
                   }
                   else {
                       cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(cglobals.lex);
               }
           }
       }
       
       void parse_namespace_block(lexr::parser_helper& lex, int block_begin)
       {
           cglobals.block_stack++;
           lexr::token temp_t;
           while( cglobals.block_stack!=block_begin)
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
                 break;
               if(temp_t.value == "}")
                  cglobals.block_stack--;
               else if(reserved(temp_t.value))
               {
                   if(temp_t.value == "namespace"){
                      string name = memoryhelper::helper::parse_dot_notation(cglobals.lex, false, "{");
                      cout << "namespace " << name << endl;
                      memoryhelper::helper::parse_namespace_block(cglobals.lex, cglobals.block_stack);
                   }
                   else if(accessspecifier(temp_t.value) || temp_t.value == "static" || temp_t.value == "const")
                   {
                       object_attrib atribs;
                       atribs.access = ((temp_t.value == "static") ? access_public : strtoaccess(temp_t.value));
                       if(temp_t.value == "static") lex.lexer().token_sz1--;
                       
                       memoryhelper::helper::parse_access_specifier(cglobals.lex, atribs, temp_t);
                       if(temp_t.value == "class"){
                           temp_t = getNextToken(lex);
                           if(temp_t.type != temp_t.e_symbol){
                               cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           cout << "class " << temp_t.value << endl;
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "{"){
                               cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           memoryhelper::helper::parse_class_block(cglobals.lex, cglobals.block_stack);
                       }
                       else if(_typedef(temp_t.value))
                           memoryhelper::helper::parse_typedef_block(cglobals.lex, temp_t.value, atribs);
                       else if(temp_t.value == "def"){
                           temp_t = getNextToken(lex);
                           if(temp_t.type != temp_t.e_symbol){
                               cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }   
                               
                           
                           cout << "def " << temp_t.value << endl;
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "("){
                               cglobals.out << "Expected '(' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           memoryhelper::helper::parse_method_args(cglobals.lex);
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "{"){
                               cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                          memoryhelper::helper::parse_method_block(cglobals.lex, cglobals.block_stack);
                           
                       }
                       else {
                           cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                           error(cglobals.lex);
                       }
                   }
                   else if(temp_t.value == "class"){
                           temp_t = getNextToken(lex);
                           if(temp_t.type != temp_t.e_symbol){
                               cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           cout << "class " << temp_t.value << endl;
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "{"){
                               cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           memoryhelper::helper::parse_class_block(cglobals.lex, cglobals.block_stack);
                       }
                   else if(_typedef(temp_t.value))
                   {
                       object_attrib atribs;
                       atribs.access = access_public;
                       atribs.isStatic = false;
                       atribs.isConst = false;
                       memoryhelper::helper::parse_typedef_block(cglobals.lex, temp_t.value, atribs);
                   }
                   else if(temp_t.value == "def"){
                       temp_t = getNextToken(lex);
                       if(temp_t.type != temp_t.e_symbol){
                           cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                           error(cglobals.lex);
                       }   
                           
                       
                       cout << "def " << temp_t.value << endl;
                       temp_t = getNextToken(lex);
                       if(temp_t.value != "("){
                           cglobals.out << "Expected '(' before '" << temp_t.value << "'.";
                           error(cglobals.lex);
                       }
                       
                       memoryhelper::helper::parse_method_args(cglobals.lex);
                       temp_t = getNextToken(lex);
                       if(temp_t.value != "{"){
                           cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                           error(cglobals.lex);
                       }
                       
                      memoryhelper::helper::parse_method_block(cglobals.lex, cglobals.block_stack);
                       
                   }
                   else {
                       cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(cglobals.lex);
               }
           }
       }
       
       bool reserved(std::string symbol)
       {
          return (symbol == "def" || symbol == "class" || symbol == "native" 
            || symbol == "extern" || symbol == "if" || symbol == "do" || symbol == "while"
            || symbol == "for" || symbol == "return" || symbol == "goto" || symbol == "int"
            || symbol == "bool" || symbol == "short" || symbol == "double" || symbol == "float"
            || symbol == "string" || symbol == "base" || symbol == "else" || symbol == "char" 
            || symbol == "const" || symbol == "true" || symbol == "false" || symbol == "new" 
            || symbol == "throw" || symbol == "throws" || symbol == "node" || symbol == "this"
            || symbol == "sizeof" || symbol == "null" || symbol == "self" || symbol == "byte" 
            || symbol == "long" || symbol == "typedef" || symbol == "static" || symbol == "public"
            || symbol == "private" || symbol == "protected" || symbol == "import" || symbol == "instanceof"
            || symbol == "package" || symbol == "namespace");
       }
       
       bool _typedef(std::string symbol)
       {
           return (symbol == "char" || symbol == "byte" || symbol == "short"
                   || symbol == "int" || symbol == "long" || symbol == "double"
                   || symbol == "float" || symbol == "string");
       }
      
       bool accessspecifier(std::string symbol)
       {
           return (symbol == "public" || symbol == "private" 
                   || symbol == "protected");
       }
       
       int strtoaccess(std::string s)
       {
           if(s == "protected")
             return access_protected;
           else if(s == "private")
             return access_private;
           else if(s == "public")
             return access_public;
       }
       
       std::string reverse(string str)
       {
           stringstream ss;
           for(int i = str.size() - 1; i >= 0; i--)
              ss << str.at(i);
            return ss.str();
       }
    }
 }

bool validate_package_file(string pkg, string file)
{
    int end = 0;
    for(int i = file.size() - 1; i > 0; i--)
    {
        if(file.at(i) == '.'){
            end++;
            break;
        }
        else
          end++;
    }
    
    if(end >= file.size())
       end = 0;
    
    stringstream ss;
    for(int i = 0; i < file.size() - end; i++)
        ss << file.at(i);
    return ss.str() == pkg;
}

int Compilr_Compile_Buf(Archive &zip_archive, stringstream &out_buf)
 {
     cglobals.success = 0;
     for(int i = 0; i < zip_archive.header.sourcecount.byte1; i++)
     {
        cout << "compiling " << zip_archive.fmap[i].name << endl;
        cglobals.mode = mode_eat_endl;
        cglobals.file = zip_archive.fmap[i].name;
    
        stringstream source;
        Zlib zippy;
        zippy.Decompress_Buffer2Buffer(zip_archive.fmap[i].contents, source);
        
        if(zres.response == ZLIB_FAILURE){
             cout << "\n" << zres.reason.str() << "Shutting down.\n";
             return COMPILR_FAILURE;
        }
        else if(zres._warnings_.str() != "")
            cout << zres._warnings_.str();
        
        lexr::token temp_t;
        
        cglobals.lex.init(source.str());
        
        temp_t = getNextToken(cglobals.lex);
        if(temp_t.value != "package")
        {
           cglobals.out << "Expected package before '" << temp_t.value << "'.";
           error(cglobals.lex);
        }
        
        string package = memoryhelper::helper::parse_dot_notation(cglobals.lex, false, ";");
        cglobals.package = package;
        
        if(cglobals.eof)
          continue;
        
        if(!validate_package_file(package, zip_archive.fmap[i].include))
        {
           cglobals.out << "Package name does not match file name, expected '" << package << "', found '" << zip_archive.fmap[i].include << "'.";
           error(cglobals.lex);
        }
        
        if(cglobals.success == 0)
        {
            stringstream ss;
            for(int i2 = zip_archive.fmap[i].name.size() - 1; i2 > 0; i2--)
            {
                if(zip_archive.fmap[i].name.at(i2) == '/' || zip_archive.fmap[i].name.at(i2) == '\\')
                  break;
                else 
                  ss << zip_archive.fmap[i].name.at(i2);
            }
           cglobals.file = memoryhelper::helper::reverse(ss.str());
        }
        
        for( ;; )
        {
           temp_t = getNextToken(cglobals.lex);
           
           if(cglobals.eof)
             break;
           else if(temp_t.type == temp_t.e_symbol)
           {
              if(memoryhelper::helper::reserved(temp_t.value))
              {
                  if(temp_t.value == "import")
                  {
                     string import = memoryhelper::helper::parse_dot_notation(cglobals.lex, false, ";", true);
                     cout << "import " << import << endl;
                     continue;
                  }
                  else if(temp_t.value == "namespace")
                  {
                      string name = memoryhelper::helper::parse_dot_notation(cglobals.lex, false, "{");
                      cout << "namespace " << name << endl;
                      memoryhelper::helper::parse_namespace_block(cglobals.lex, cglobals.block_stack);
                  }
              }
              else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(cglobals.lex);
               }
           }
           else if(temp_t.value == ";"){}
           else {
             cglobals.out << "Expected qualified symbol before '" << temp_t.value << "'.";
             error(cglobals.lex);
           }
        }
     }
     return cglobals.success;
 }

string null(int offset)
 {
    stringstream ss;
    char _nil_ = 0;
    for(int i = 0; i < offset; i++)
        ss << (char) _nil_;
   return ss.str();
 }
 
 string addpermissions(XsoHeader &header)
 {
    stringstream ss, ss1;
    ss << (char) permissions;
    for(int i = 0; i < header.permission_t; i++)
       ss1 << header.permissions[i] << ";";
    
    Binary::encode_str(ss1.str(), ss);
    return ss.str();
 }

void objto_xso(XsoHeader header, stringstream &out_buf)
 {
     out_buf << (char) header.magic.byte1 << (char) header.magic.byte2 << (char) header.magic.byte3 << (char) header.magic.byte4 << (char) 0;
     out_buf << (char) nameflag << header.name << null(3);
     
     stringstream ss1;
     ss1 << (long) header.method_size.byte1;
     
     stringstream __ostream_buf__;
     Binary::encode_str(ss1.str(), __ostream_buf__);
     out_buf << (char) methodsize << __ostream_buf__.str()  << null(1);
     __ostream_buf__.str("");
     ss1.str("");
     
     out_buf << (char) _minor_version << (char) header.minor_version.byte1 << (char) header.minor_version.byte2 << null(1);
     out_buf << (char) _major_version << (char) header.major_version.byte1 << (char) header.major_version.byte2 << null(1);
     
     ss1 << (long) header.filesize.byte1;
    
     Binary::encode_str(ss1.str(), __ostream_buf__);
     ss1.str("");

     out_buf << (char) file_size << __ostream_buf__.str() << null(1);
     __ostream_buf__.str("");
     out_buf << (char) target_dev_vers << (char) header.target_dev_vers.byte1 << null(1);
     out_buf << (char) minimum_dev_vers << (char) header.minimum_dev_vers.byte1 << null(1);
     
     Binary::encode_str(header.version_number, __ostream_buf__);
     out_buf << (char) version_number << __ostream_buf__.str() << null(1);
     __ostream_buf__.str("");
     
     out_buf << (char) debugging << (long) header.debug.byte1 << null(1);
     out_buf << (char) logging << (long) header.logging.byte1 << null(1);
     out_buf << (char) log_precedence << header.log_precedence.byte1 << null(1);
     
     Binary::encode_str(header.log_file, __ostream_buf__);
     out_buf << (char) log_file << __ostream_buf__.str()  << null(1);
     __ostream_buf__.str("");
     
     Binary::encode_str(header.application_id, __ostream_buf__);
     out_buf << (char) application_id << __ostream_buf__.str() << null(1) << "\n" << addpermissions(header);
     out_buf << null(1) << null(12) << "\n" << null(20);
 } 
 
int Cmplr_Compile_Zip( Archive &zip_archive, stringstream &__out_buf__ )
 {
     if(zip_archive.header.sourcecount.byte1 > 0)
     {
        int success=0;
        if(!init)
        {
           success = cmplr_init();
           if(success!=0){  return COMPILR_FAILURE; }
        }
        else
        {
           cmplr_cleanup( true );
           success = cmplr_init();
           if(success!=0){  return COMPILR_FAILURE; }
        }
        
        cres.size_t.byte1=0;
        cglobals.method_t=0;
        success= Compilr_Compile_Buf(zip_archive, __out_buf__);
      
        if(success==0)
        {
           cout<<"success\n";
           stringstream __new_buf__;
           
           XsoHeader head;
           head.magic.byte1=0xdf;
           head.magic.byte2=0x4e;
           head.magic.byte3=0xfa;
           head.magic.byte4=0x2b;
           
           head.minor_version.byte1 = 0x70;
           head.minor_version.byte2 = 0x010;

           head.major_version.byte1 = 0x7;
           head.major_version.byte2 = 0x10;
           
           head.version_number = "0.1.0.0";
           head.debug.byte1 = 0;
           head.logging.byte1 = 0;
           head.log_precedence.byte1 = 2; // verbose
        
           head.log_file = "/usr/share/scorpion/lib/";
           head.target_dev_vers.byte1  = 200;
           head.minimum_dev_vers.byte1 = 7;
        
           head.application_id = "com.scorpion.microsystems";
           head.permissions = new string[2];
           head.permissions[0] = "scorpion.permission.READ_INTERNAL_STORAGE";
           head.permissions[1] = "scorpion.permission.WRITE_INTERNAL_STORAGE";
           head.name = "scorpionc";
           head.permission_t=2;
           
           head.filesize.byte1 = cres.size_t.byte1;
           head.method_size.byte1 = cglobals.method_t;
           objto_xso(head, __new_buf__);
           
           Zlib zlib;
           zlib.Compress_Buffer2Buffer(__out_buf__.str(), __new_buf__, ZLIB_LAST_SEGMENT);
           __out_buf__.str("");
           
           if(zres.response == ZLIB_FAILURE){
              cout << "\n" << zres.reason.str() << "Shutting down.\n";
              return COMPILR_FAILURE;
           }
           else if(zres._warnings_.str() != "")
              cout << zres._warnings_.str();
          
           __out_buf__ << __new_buf__.str();
           __new_buf__.str("");
           return 0;
        }
        
        return success;

     }
     else {
         cres.reason << "compilr:  error: library is empty.\n";
         return COMPILR_FAILURE;
     }
 }
 
 
