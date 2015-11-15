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
 #include "ccompiler.h"
 #include "compilr.h"
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
 
 struct function_definition
 {
    ListAdapter<string> varlist;
    ListAdapter<int> vartypes;
    std::string name;
    std::string body;
    
    void clear()
    {
       name    .clear();
       body    .clear();
       varlist .clear();
       vartypes.clear();
    }
 };
 
 struct node
 {
    ListAdapter<string> varlist;
    ListAdapter<int> vartypes;
    std::string name;
    std::string body;
    
    void clear()
    {
       name    .clear();
       body    .clear();
       varlist .clear();
       vartypes.clear();
    }
 };
 
 struct module_def
 {
    std::string name;
    
    void clear()
    {
       name.clear();
    };
 };
 
 struct import 
 {
    std::string _module;
    
    void clear()
    {
       _module.clear();
    };
 };
 
 struct Class
 {
     std::string name;
     std::string body;
     std::string baseClass;
     
     void clear()
     {
         name     .clear();
         body     .clear();
         baseClass.clear();
     }
 };
 
 struct parse_module_impl
 {
    int parse(lexr::parser_helper &lex, module_def &mDef, std::string file)
    {
       stringstream ss;
       std::string tmp="";
       bool dot;
       lexr::token t;
       lex.next_token();
       
       if (!lex.token_is(t.e_symbol,"module")){
           cout << file << ":" << lexr::line_t << ":  error: expected module before '" << lex.current_token().value << "'." << endl;
           cout << "\tmodulename not set, compilation terminated.\n";
          return -1;
       }
       
       for( ; ; )
       {
          if(lex.token_is_then_assign(t.e_symbol, tmp)){
             ss << tmp;
             dot=false;
          }
          else{
               
             if(lex.token_is(t.e_semicolon)){
               if(dot)
               {
                  cout << file << ":" << lexr::line_t << ":  error: expected symbol before ';'." << endl;
                  cout << "\tmodulename not set, compilation terminated.\n";
                  return -1;
               }
               break;
             }
             else if(lex.token_is(t.e_dot)){
                ss << '.';
                dot=true;
             }
             else{
                lexr::token t = lex.current_token();
                cout << file << ":" << lexr::line_t << ":  error: expected symbol or dot before '" << t.value << "'." << endl; 
                cout << "\tmodulename not set, compilation terminated.\n";
                return -1;
             }
          } 
       }
       mDef.name = ss.str();
       return 0;
    }
 };
 
 bool parse_module_definition(lexr::parser_helper &lexer, module_def &mod_def, std::string file_name)
 {
    parse_module_impl module_impl;
    return (module_impl.parse(lexer, mod_def, file_name) == 0);
 }
 
 int Compilr_Compile_Buf(Archive &zip_archive, stringstream &out_buf)
 {
    int res = 0;
     for(int i = 0; i < zip_archive.header.sourcecount.byte1; i++)
     {
        cout << "compiling " << zip_archive.fmap[i].name << endl;
        stringstream source;
        Zlib zippy;
        zippy.Decompress_Buffer2Buffer(zip_archive.fmap[i].contents, source);
        
        if(zres.response == ZLIB_FAILURE){
             cout << "\n" << zres.reason.str() << "Shutting down.\n";
             return COMPILR_FAILURE;
        }
        else if(zres._warnings_.str() != "")
            cout << zres._warnings_.str();
        
        lexr::parser_helper lex;
        
        lex.init(source.str());
        
        module_def mDef;
        if(!parse_module_definition(lex, mDef, zip_archive.fmap[i].name))
           continue;
        
        cout << "module " << mDef.name << endl;
        
        for(std::size_t i; i <lex.lexer().size(); i++)
        {
          
        }
     }
     return 0;
 }
 
 // TODO: Spruce up sar and scorpionc before release sar->(clib) scorpionc->(compiler src files)
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
           cmplr_cleanup( false );
           cplrfreelist2 = new_cmplr_item2();
        }
      
        success= Compilr_Compile_Buf(zip_archive, __out_buf__);
      
        if(success==0)
        {
           stringstream __new_buf__, version, obj_size_t, buf;
           Binary::encode_str("0.1.0.0", version);
           
           buf << cres.size_t.byte1;
           Binary::encode_str(buf.str(), obj_size_t);
           
           __new_buf__ << (char) 0xBA << (char) 0xCA << (char) 0xFE << (char) 0xBE; // add 4 byte magic number 
           
           for(int i = 0; i < 3; i++)
            __new_buf__ << (char) 0x0; // add null dummy data
            
           __new_buf__ << (char) 0x0 << version.str() << (char) 0x0 << obj_size_t.str() << (char) 0x0; // add version number and object size
           for(int i = 0; i < 10; i++)
            __new_buf__ << (char) 0x0; // add null dummy data
           
           __new_buf__ << endl << (char) 0x0; 
           
           Zlib zlib;
           zlib.Compress_Buffer2Buffer(__out_buf__.str(), __new_buf__, ZLIB_LAST_SEGMENT);
           
           if(zres.response == ZLIB_FAILURE){
              cout << "\n" << zres.reason.str() << "Shutting down.\n";
              return COMPILR_FAILURE;
           }
           else if(zres._warnings_.str() != "")
              cout << zres._warnings_.str();
          
           __out_buf__.str("");
           __out_buf__ << __new_buf__.str();
           __new_buf__.str("");
        }
        
        __out_buf__.str("");
        
        return success;

     }
     else {
         cres.reason << "compilr:  error: library is empty.\n";
         return COMPILR_FAILURE;
     }
 }
 
 