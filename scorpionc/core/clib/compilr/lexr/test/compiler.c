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
 *
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

 
 bool EEOF_=0, hasMain=false;
 lexr::token getNextTok(lexr::parser_helper &lexer){ 
     if(lexer.lexer().token_sz1<lexer.lexer().size()){
       lexr::token t = lexer.lexer()[lexer.lexer().token_sz1++];
       while(t.type == t.e_line){ t = lexer.lexer()[lexer.lexer().token_sz1++]; lexer.lexer().line_t++; }
       
       if(t.type == t.e_eof){
         EEOF_=1;
         return lexer.lexer()[0];
       }
       else
         return t;
     }
     else {
         EEOF_=1;
         return lexer.lexer()[0];
     }
     
}

int addStaticMethod(string name, string module, string clazz, unsigned long address)
{
    Method method;
    method.name = name;
    method.module = module;
    method.clazz = clazz;
    staticMethods.add(method);
}

int addObj(int _typedef_, u1 objsz_t, string name, string m, string k)
{
   SVM_OBJECT_INIT(_typedef_, objsz_t, name, m, k);
}

bool isstype(string type)
{
    return (type == "string" || type == "int" ||
            type == "short" || type == "char" ||
            type == "bool"|| type == "float" ||
            type == "double");
}

int ttoi(string type)
{
    if(!isstype(type))
      return 0;
      
    if(type == "string")
      return 1;
    else if(type == "int")
      return 2;
    else if(type == "short")
      return 3;
    else if(type == "char")
      return 4;
    else if(type == "bool")
      return 5;
    else if(type == "float")
      return 6;
    else if(type == "double")
      return 7;
}
 
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
    *
    int _minor_version     = 0xAF;
    int _major_version     = 0xCE;
    int file_size          = 0xEF;
    int target_dev_vers    = 0xBC;
    int minimum_dev_vers   = 0xFD;
    int version_number     = 0xCE;//
    int debugging          = 0xEC;
    int logging            = 0xFC;
    int log_precedence     = 0xFA;
    int log_file           = 0x3C;//
    int application_id     = 0xB5; //
    int permissions        = 0xC1;//
    int nameflag           = 0x7A;
    int methodsize         = 0xD9;
    
 struct Header {
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
 
 struct mode
 {
     enum modes 
     {
         m_function_body    = 0, m_class_body = 1,
         m_node_struct_body = 2, m_if_body = 3
     };
 };
 
 struct function_definition
 {
    ListAdapter<string> varlist;
    ListAdapter<int> vartypes;
    ListAdapter<int> pointertype;
    std::string name;
    std::string body;
    unsigned long begin, address;
    
    void clear()
    {
       name    .clear();
       body    .clear();
       varlist .clear();
       vartypes.clear();
    }
 };
 
 struct variable_definition
 {
     std::string value;
     std::string var;
     int vartype;
     
     
     void clear()
     {
        value.clear();
        var  .clear();
        vartype=0;
     }
 };
 
 struct node_definition
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
 
 struct module_definition
 {
    std::string name;
    
    void clear()
    {
       name.clear();
    };
 } mDef;
 
 struct import_definition 
 {
    std::string _module;
    
    void clear()
    {
       _module.clear();
    };
 };
 
 struct class_definition
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
 
 // ------------------------------parser_Helper-------------------------------------------
 struct parser_Helper
 {
     
 };
 // ------------------------------parser_Helper end---------------------------------------
 
 void return_data(double data, unsigned long return_addr, bool assign)
 {
       if(!assign){
           u2 init;
           init.byte1=OP_ICONST;
           init.byte2=2;
          
           double* op_data=new double[2];
           op_data[0] = var_return;
           op_data[1] = data;
          
           cmplr_add_item( new_cmplr_item(init, op_data, "") ); 
           
           init.byte1=OP_PUSH;
           init.byte2=1;
          
           op_data=new double[1];
           op_data[0] = var_return;
          
           cmplr_add_item( new_cmplr_item(init, op_data, "") ); 
          
           init.byte1=OP_RETURN;
           init.byte2=1;
          
           op_data[0] = return_addr;
          
           cmplr_add_item( new_cmplr_item(init, op_data, "") ); // return 0
       }
       else {
           u2 init;
           init.byte1=OP_ASSIGN;
           init.byte2=2;
          
           double* op_data=new double[2];
           op_data[0] = var_return;
           op_data[1] = data + obj_offset;
          
           cmplr_add_item( new_cmplr_item(init, op_data, "") ); 
           
           init.byte1=OP_PUSH;
           init.byte2=1;
          
           op_data=new double[1];
           op_data[0] = var_return;
          
           cmplr_add_item( new_cmplr_item(init, op_data, "") ); 
          
           init.byte1=OP_RETURN;
           init.byte2=1;
          
           op_data[0] = return_addr;
          
           cmplr_add_item( new_cmplr_item(init, op_data, "") ); // return 0;
       }
 }
 
 struct parse_module_impl
 {
    int parse(lexr::parser_helper &lex, module_definition &mDef, std::string file)
    {
       stringstream ss;
       bool dot;
       lexr::token t = getNextTok(lex);
       
       if (t.value != "module"){
           cout << file << ":" << lex.lexer().line_t << ":  error: expected module before '" << t.value << "'." << endl;
           cout << "\tmodulename not set, compilation terminated.\n";
          return -1;
       }
       
       for( ; ; )
       {
           t = getNextTok(lex);
          if(t.type == t.e_symbol){
             ss << t.value;
             dot=false;
          }
          else if(t.type == t.e_semicolon){
            lex.lexer().token_sz1++;
            
            if(dot)
            {
               cout << file << ":" << lex.lexer().line_t << ":  error: expected identifier before ';'." << endl;
               cout << "\tmodulename not set, compilation terminated.\n";
               return -1;
            }
            break;
          }
          else if(t.type == t.e_dot){
             ss << '.';
             dot=true;
          }
          else{
             cout << file << ":" << lex.lexer().line_t << ":  error: expected identifier or '.' before '" << t.value << "'." << endl; 
             cout << "\tmodulename not set, compilation terminated.\n";
             return -1;
          }
        } 
       mDef.name = ss.str();
       return 0;
    }
 };
 
 struct parse_import_impl
 {
    int parse(lexr::parser_helper &lex, import_definition &iDef, std::string file)
    {
       stringstream ss;
       bool dot;
       lexr::token t;
       
       for( ; ; )
       {
          t = getNextTok(lex);
          if(t.type == t.e_symbol){
             ss << t.value;
             dot=false;
          }
          else if(EEOF_)
          return -2;
          else if(t.type == t.e_semicolon){
            if(dot)
            {
               cout << file << ":" << lex.lexer().line_t << ":  error: expected identifier before ';'." << endl;
               return -1;
            }
            lex.lexer().token_sz1++;
            break;
          }
          else if(t.value == "*"){
              t = getNextTok(lex);
              if(t.type != t.e_semicolon)
              {
                  cout << file << ":" << lex.lexer().line_t << ":  error: expected ; after '*'." << endl;
                  return -1;
              }
              break;
          }
          else if(t.type == t.e_dot){
             ss << '.';
             dot=true;
          }
          else{
             cout << file << ":" << lex.lexer().line_t << ":  error: expected identifier or '.' before '" << t.value << "'." << endl;
             return -1;
          }
        } 
       iDef._module = ss.str();
       return 0;
    }
 };
 
 struct parse_function_impl
 {
    int parse(lexr::parser_helper &lex, function_definition &fDef, std::string file)
    {
       int r=0;
       stringstream ss;
       bool comma =false, hassymbol=false;
       lexr::token t = getNextTok(lex);
       
       if (t.type != t.e_symbol){
           cout << file << ":" << lex.lexer().line_t << ":  error: expected identifier before '" << t.value << "'." << endl;
           cout << "\tmodulename not set, compilation terminated.\n";
          r = -1;
       }
       else
       {
           if(isstype(t.value))
           {
               cout << file << ":" << lex.lexer().line_t << ":  error: identifier '" << t.value << "' is a type." << endl;
               r = -1;
           }
           
           fDef.name = t.value;
           
           if(findSpecificMethodByDescriptor(mDef.name, fDef.name)){
              cout << file << ":" << lex.lexer().line_t << ":  error: static method 'def " << fDef.name << "()' is previously declared.\n";
              fDef.clear();
              r= -1;
           }
            
           addStaticMethod(fDef.name, mDef.name, "$global", staticMethods.size());
           // if(!import_f)
           method_t++;
           
          if(firstm)
          {
               firstm=0;
               u2 init;
               init.byte1=OP_MTHD;
               init.byte2=1;
              
               double* data=new double[1];
               if(fDef.name == "__init__")
                   data[0] = 0;
               else
                   data[0] = 1;
               fDef.address = data[0];
              
               cmplr_add_item( new_cmplr_item(init, data, "") );
          }
          else {
               u2 init;
               init.byte1=OP_MTHD;
               init.byte2=1;
              
               double* data=new double[1];
               if(fDef.name == "__init__")
                   data[0] = 0;
               else
                   data[0] = staticMethods.size();
               fDef.address = data[0];
              
               cmplr_add_item( new_cmplr_item(init, data, "") );
          }
           
       }
       
       t = getNextTok(lex);
       if (t.type != t.e_lbracket){
           cout << file << ":" << lex.lexer().line_t << ":  error: expected '(' before '" << t.value << "'." << endl;
          r = -1;
       }
       
       for( ; ; )
       {
          t = getNextTok(lex);
          if(t.type == t.e_symbol){
              comma=false;
              hassymbol=true;
             if(isstype(t.value)){
                 fDef.vartypes.add(ttoi(t.value));
             }
             else
             {
                 cout << file << ":" << lex.lexer().line_t << ":  error: identifier '" << t.value << "' does not name a type." << endl;
                 r = -1;
             }
             
             t = getNextTok(lex);
             if(t.type == t.e_mul)
               fDef.pointertype.add(1);
             else if(t.type == t.e_parry)
               fDef.pointertype.add(2);
             else
               fDef.pointertype.add(3);
               
             if(t.type == t.e_mul || t.type == t.e_parry)
               t = getNextTok(lex);
             
             if(t.type == t.e_symbol)
             {
                 if(isstype(t.value))
                 {
                     cout << file << ":" << lex.lexer().line_t << ":  error: identifier '" << t.value << "' is a type." << endl;
                     r = -1;
                 }
                 
                 fDef.varlist.add(t.value);
             }
          }
          else if(t.type == t.e_rbracket){
              if (comma)
              {
                   cout << file << ":" << lex.lexer().line_t << ":  error: expected identifier before ')'." << endl;
                   r = -1;
              }
            break;
          }
          else if(t.type == t.e_comma){ 
              if(!hassymbol)
              {
                   cout << file << ":" << lex.lexer().line_t << ":  error: expected identifier before ','." << endl;
                   r = -1;
              }
              else
                hassymbol=false;
              comma = true; 
          }
          else{
               cout << file << ":" << lex.lexer().line_t << ":  error: expected identifier or ',' before '" << t.value << "'." << endl;
               r = -1;
               break;
          }
          
       }
       
       
       unsigned long address=staticMethods.size() - 1;
       Method meth = staticMethods.valueAt(address);
       meth.vartypes = fDef.vartypes;
       meth.varlist = fDef.varlist;
       meth.pointertype = fDef.pointertype;
       staticMethods.replace(meth, address);
       
       unsigned int bracket_stack=0;
       stringstream body;
       
       t = getNextTok(lex);
       fDef.begin = lex.lexer().line_t;
       if(t.value == "{")
       {
           lex.lexer().token_sz1--;
           for( ;; )
           {
              t = getNextTok(lex);
              if(t.type == t.e_lcrlbracket)
              {
                  bracket_stack++;
                  body << "{";
              }
              else if (t.type == t.e_rcrlbracket)
              {
                  body << "}";
                  if(0 == --bracket_stack)
                    break;
              }
              else {
                  if(EEOF_)
                  {
                      cout << file << ":" << lex.lexer().line_t << ":  error: epected '}' at the end if the input.\n";
                      return -1;
                  }
                  if(t.type == t.e_string)
                    body << "\"" << t.value << "\"";
                  else
                    body << t.value;
              }
           }
           
           fDef.body = body.str();
       }
       else {
         cout << file << ":" << lex.lexer().line_t << ":  error: expected '{' before '" << t.value << "'." << endl;
         return -1;
      }
       return r;
    }
    
};
 
 struct parse_asm_impl
 {
    int parse(std::string _asm, unsigned long begin, std::string file)
    {
       int r=0;
       stringstream ss;
       std::string tmp="";
       lexr::parser_helper lex;
       lexr::token t;
       
       lex.init(_asm);
       lex.lexer().line_t=begin;
       
       for( ;; )
       {
           t = getNextTok(lex);
           if(EEOF_)
             break;
           else if(t.type == t.e_symbol)
           {
               if(t.value == "cout")
               {
                   t = getNextTok(lex);
                   if(t.type == t.e_string)
                   {
                       u2 init;
                       init.byte1=OP_COUT;
                       init.byte2=0;
                       double* op_data;
                       
                       cmplr_add_item( new_cmplr_item(init, op_data, t.value) );
                   }
                   else{
                       cout << file << ":" << lex.lexer().line_t << ":  error: expected string before '" << t.value << "'." << endl;
                       r = -1;
                   }
               }
               else {
                   cout << file << ":" << lex.lexer().line_t << ":  error: expected assembly instruction before '" << t.value << "'." << endl;
                   r = -1;
               }
           }
           else {
                cout << file << ":" << lex.lexer().line_t << ":  error: expected identifier before '" << t.value << "'." << endl;
                r = -1;
           }
       }
       
       return r;
    }
 };
 
 
 struct parse_method_block_impl
 {
    int parse(std::string block, unsigned long begin, std::string file, unsigned long return_addr)
    {
       int r=0;
       stringstream ss;
       std::string tmp="";
       lexr::parser_helper lex;
       lexr::token t;
       
       lex.init(block);
       lex.lexer().line_t=begin;
       
       for( ;; )
       {
           t = getNextTok(lex);
           if(t.type == t.e_symbol)
           {
               if(t.value == "asm")
               {
                   t = getNextTok(lex);
                   if(t.value == "(")
                   {
                       t = getNextTok(lex);
                       if(t.type == t.e_string)
                       {
                           parse_asm_impl asm_impl;
                           r=asm_impl.parse(t.value, lex.lexer().line_t, file);
                           
                           t = getNextTok(lex);
                           if(t.value == ")")
                           {
                               t = getNextTok(lex);
                               if(t.value != ";")
                               {
                                   cout << file << ":" << lex.lexer().line_t << ":  error: expected ';' before '" << t.value << "'." << endl;
                                   r = -1;
                               }
                               else continue;
                           }
                           else 
                           { 
                               cout << file << ":" << lex.lexer().line_t << ":  error: expected ')' before '" << t.value << "'." << endl;
                               r = -1;
                           }
                       }
                       else
                       {
                           cout << file << ":" << lex.lexer().line_t << ":  error: expected string literal before '" << t.value << "'." << endl;
                           r = -1;
                       }
                   }
                   else
                   {
                       cout << file << ":" << lex.lexer().line_t << ":  error: expected '(' before '" << t.value << "'." << endl;
                       r = -1;
                   }
               }
               else if(t.value == "return")
               {
                   // TODO: return data
                   t = getNextTok(lex);
                   if(t.type == t.e_number)
                       return_data(atoi(t.value.c_str()), return_addr, false);
                   else if(t.type == t.e_symbol){
                        stringstream var;
                        var << t.value;
                        
                        t = getNextTok(lex); // is this a variable?
                        if(t.value == ";"){
                            // find variable
                            if(!findSpecificObjectByDescriptor(mDef.name, var.str())){
                                if(findSpecificMethodByDescriptor(mDef.name, var.str())){
                                    cout << file << ":" << lex.lexer().line_t << ":  error: method '" << var.str() << "' requires '()' before ;." << endl;
                                    r = -1;
                                }
                                else {
                                    cout << file << ":" << lex.lexer().line_t << ":  error: could not resolve symbol '" << t.value << "'." << endl;
                                    r = -1;
                                }
                            }
                            else {
                                return_data(findSpecificObjectAddressByDescriptor(mDef.name, var.str()) + obj_offset, return_addr, true);
                            }
                        }
                        else if(t.value == "(") { // method call?
                            t = getNextTok(lex); // is this a variable?
                            if(t.value == ")"){
                                if(!findSpecificMethodByDescriptor(mDef.name, var.str())){
                                    cout << file << ":" << lex.lexer().line_t << ":  error: could not resolve method '" << t.value << "'." << endl;
                                    r = -1;
                                }
                                else {
                                    // call method then return with its value
                                }
                            }
                            else {
                                // if(!parse_function_args())
                                cout << file << ":" << lex.lexer().line_t << ":  error: expected ')' before '" << t.value << "'." << endl;
                                r = -1;
                            }
                        }
                        else if( t.value == ".") {
                          //  t = getNextTok(lex);
                            // is this a variable or method call?
                              
                        }
                        else {
                            cout << file << ":" << lex.lexer().line_t << ":  error: expected ';' before '" << t.value << "'." << endl;
                            r = -1;
                        }
                   }
               }
           }
           else {
              if(EEOF_)  break;
              
           }
       }
       
       
       
       return r;
    }
    
 };

 bool parse_module_definition(lexr::parser_helper &lexer, module_definition &mod_def, std::string file_name)
 {
    parse_module_impl module_impl;
    return (module_impl.parse(lexer, mod_def, file_name) == 0);
 }
 
 int parse_import_definition(lexr::parser_helper &lex, import_definition &iDef, std::string file)
 {
    parse_import_impl import_impl;
    return import_impl.parse(lex, iDef, file);
 }
 
 int parse_function_definition(lexr::parser_helper &lex, function_definition &fDef, std::string file)
 {
    parse_function_impl func_impl;
    return func_impl.parse(lex, fDef, file);
 }
 
 int parse_block_definition(std::string block, unsigned long begin, int mode, std::string file, unsigned long addr)
 {
    parse_method_block_impl block_impl;
    return block_impl.parse(block, begin, file, addr);
 }
 
 bool parse_source_file(lexr::parser_helper &lexer, Archive &zip, stringstream &_ostream_buf_, int pos)
 {
     bool parseResponseCode = true;
     stringstream ss;
     std::string tmp="";
     lexr::token t;
     
     while( lexer.lexer().token_sz1<lexer.lexer().size() )
     {
         t = getNextTok(lexer);
         
         if(EEOF_)
             break;
          else if(t.type == t.e_symbol)
          {
              if(t.value == "import")
              {
                    import_definition iDef;
                    int r=parse_import_definition(lexer, iDef, zip.fmap[pos].name);
                    if(r == -1)
                      parseResponseCode=false;
                    else if(r == -2)
                    {
                        parseResponseCode=false;
                        cout << zip.fmap[pos].name << ":" << lexer.lexer().line_t << ":  error: unexpected end of file.\n";
                        continue;
                    }
                    // TODO: handle import
              }
              else if(t.value == "def")
              {
                  function_definition fDef;
                  int r=parse_function_definition(lexer, fDef, zip.fmap[pos].name);
                  
                  if(r == -1)
                      parseResponseCode=false;
                  else if(r == -2)
                  {
                      parseResponseCode=false;
                      cout << zip.fmap[pos].name << ":" << lexer.lexer().line_t << ":  error: unexpected end of file.\n";
                      fDef.clear();
                      continue;
                  }
                  
                  mode m;
                  if(parse_block_definition(fDef.body, fDef.begin, m.m_function_body, zip.fmap[pos].name, fDef.address) != 0)
                     parseResponseCode=false;
                  else 
                     return_data(0, fDef.address, false);
                     
              }
          }
          else
          {
              cout << zip.fmap[pos].name << ":" << lexer.lexer().line_t << ":  error expected identifier before '" << t.value << "'."<< endl;
              parseResponseCode = false;
          }
     }
     
     return parseResponseCode;
 }
 
 
 void parse_cmplr_items(stringstream &out_buf)
 {
     for(unsigned long i =0; i<cplr_item_sz1; i++)
     {
         cplrfreelist1=new cmplr_item[1];
         cplrfreelist1[0] = cplrfreelist2->c_items[i];
         unsigned long ins= cplrfreelist1->item.byte1;
         
         if(ins == OP_MTHD){
                Method m= staticMethods.valueAt(cplrfreelist1->sub_item->item.byte1);
                
                out_buf << (char) cplr_method << (char) OP_MTHD << m.name << "&" << m.clazz << "&" << m.module << (char) 0;
                out_buf << cplrfreelist1->sub_item->item.byte1 << (char) 0;
         }
         else if(ins == OP_COUT){
             
             out_buf << (char) cplr_string_instr << (char) ins << cplrfreelist1->str[0] << (char) 0;
         }
         else if(ins == OP_NOP || ins == OP_END || ins == OP_HLT || ins == OP_NO || ins == OP_ENDNO)
             out_buf << (char) cplr_instr << (char) ins;
         else if( ins == OP_RETURN || ins == OP_CALL || ins == OP_SLP || ins == OP_USLP)
                out_buf << (char) cplr_instr << (char) ins << cplrfreelist1->sub_item->item.byte1 << (char) 0;
         else if(ins == OP_PUSH || ins == OP_POP || ins == OP_JMP || ins == OP_LBL || ins == OP_IF || ins == OP_INC || ins == OP_DEC
                 || ins == OP_KILL || ins == OP_DELETE || ins == OP_DELETE_ARRY) // for instructions that access memory
                out_buf << (char) cplr_instr << (char) ins << cplrfreelist1->sub_item->item.byte1 << (char) 0;
         else if(ins == OP_SCONST || ins == OP_BCONST || ins == OP_CCONST || ins == OP_RSHFT 
                   || ins == OP_LSHFT || ins == OP_CIN || ins == OP_JIF 
                   || ins == OP_JIT || ins == OP_ICONST || ins == OP_DCONST || ins == OP_FCONST 
                   || ins == OP_THROW)
                out_buf << (char) cplr_instr << (char) ins << cplrfreelist1->sub_item[0].item.byte1 << (char) 0
                << cplrfreelist1->sub_item[1].item.byte1 << (char) 0;
         else if(ins == OP_IADD || ins == OP_ISEQ || ins == OP_ISNEQ
                   || ins == OP_ISLT || ins == OP_ISNLT || ins == OP_ISLE || ins == OP_ISNLE
                   || ins == OP_ISGT || ins == OP_ISNGT || ins == OP_ISGE || ins == OP_ISNGE
                   || ins == OP_ISUB || ins == OP_IMULT || ins == OP_IDIV || ins == OP_SADD
                   || ins == OP_SSUB || ins == OP_SMULT || ins == OP_SDIV || ins == OP_DADD
                   || ins == OP_DSUB || ins == OP_DMULT || ins == OP_DDIV || ins == OP_FADD
                   || ins == OP_FSUB || ins == OP_FMULT || ins == OP_FDIV || ins == OP_CADD
                   || ins == OP_CSUB || ins == OP_CMULT || ins == OP_CDIV || ins == OP_IMOD
                   || ins == OP_CMOD || ins == OP_SMOD || ins == OP_OR || ins == OP_AND
                   || ins == OP_AT || ins == OP_ALOAD || ins == OP_ASTORE)
                out_buf << (char) cplr_instr << (char) ins << cplrfreelist1->sub_item[0].item.byte1 << (char) 0
                << cplrfreelist1->sub_item[1].item.byte1 << (char) 0 << cplrfreelist1->sub_item[2].item.byte1 << (char) 0;
                
         cmplrfree( cplrfreelist1 );
     }
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
        lexr::token temp_t;
        
        lex.init(source.str());
        lex.lexer().line_t++;
        
        if(!parse_module_definition(lex, mDef, zip_archive.fmap[i].name)){
           res = -1;
           continue;
        }
        
        int end = 0;
        for(int oi = zip_archive.fmap[i].include.size() - 1; oi > 0; oi--)
        {
            if(zip_archive.fmap[i].include.at(oi) == '.'){
                end++;
                break;
            }
            else
              end++;
        }
        
        if(end >= zip_archive.fmap[i].include.size())
           end = 0;
        
        stringstream ss;
        for(int oi = 0; oi < zip_archive.fmap[i].include.size() - end; oi++)
            ss << zip_archive.fmap[i].include.at(oi);
        
        if(ss.str() != mDef.name)
        {
           cout << zip_archive.fmap[i].name << ":" << lex.lexer().line_t << ": module name does not match with file name."<< endl;
           res = -1;
        }
        
        if(!parse_source_file(lex, zip_archive, out_buf, i))
            res = -1;
        else 
          parse_cmplr_items(out_buf);
     }
     return res;
 }
 
 string offset(int offset)
 {
    stringstream ss;
    char null = 0;
    for(int i = 0; i < offset; i++)
        ss << (char) null;
   return ss.str();
 }
 string prmns(Header &header)
 {
    stringstream ss, ss1;
    ss << (char) permissions;
    for(int i = 0; i < header.permission_t; i++)
       ss1 << header.permissions[i] << ";";
    
    Binary::encode_str(ss1.str(), ss);
    return ss.str();
 }

 void encode_header(Header header, stringstream &out_buf)
 {
     out_buf << (char) header.magic.byte1 << (char) header.magic.byte2 << (char) header.magic.byte3 << (char) header.magic.byte4 << (char) 0;
     out_buf << (char) nameflag << header.name << offset(3);
     
     stringstream ss1;
     ss1 << (long) header.method_size.byte1;
     
     stringstream __ostream_buf__;
     Binary::encode_str(ss1.str(), __ostream_buf__);
     out_buf << (char) methodsize << __ostream_buf__.str()  << offset(1);
     __ostream_buf__.str("");
     ss1.str("");
     
     out_buf << (char) _minor_version << (char) header.minor_version.byte1 << (char) header.minor_version.byte2 << offset(1);
     out_buf << (char) _major_version << (char) header.major_version.byte1 << (char) header.major_version.byte2 << offset(1);
     
     ss1 << (long) header.filesize.byte1;
    
     Binary::encode_str(ss1.str(), __ostream_buf__);
     ss1.str("");

     out_buf << (char) file_size << __ostream_buf__.str() << offset(1);
     __ostream_buf__.str("");
     out_buf << (char) target_dev_vers << (char) header.target_dev_vers.byte1 << offset(1);
     out_buf << (char) minimum_dev_vers << (char) header.minimum_dev_vers.byte1 << offset(1);
     
     Binary::encode_str(header.version_number, __ostream_buf__);
     out_buf << (char) version_number << __ostream_buf__.str() << offset(1);
     __ostream_buf__.str("");
     
     out_buf << (char) debugging << (long) header.debug.byte1 << offset(1);
     out_buf << (char) logging << (long) header.logging.byte1 << offset(1);
     out_buf << (char) log_precedence << header.log_precedence.byte1 << offset(1);
     
     Binary::encode_str(header.log_file, __ostream_buf__);
     out_buf << (char) log_file << __ostream_buf__.str()  << offset(1);
     __ostream_buf__.str("");
     
     Binary::encode_str(header.application_id, __ostream_buf__);
    cout << "entry.\n";
     out_buf << (char) application_id << __ostream_buf__.str() << offset(1) << "\n" << prmns(header);
     out_buf << offset(1) << offset(12) << "\n" << offset(20);
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
           cmplr_cleanup( true );
           success = cmplr_init();
           if(success!=0){  return COMPILR_FAILURE; }
        }
        
        cres.size_t.byte1=0;
        method_t=0;
        success= Compilr_Compile_Buf(zip_archive, __out_buf__);
      
        if(success==0)
        {
           cout<<"success\n";
           stringstream __new_buf__;
           
           Header head;
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
        
           head.application_id = "com.scorpion.microsystems";
           head.permissions = new string[2];
           head.permissions[0] = "scorpion.permission.READ_INTERNAL_STORAGE";
           head.permissions[1] = "scorpion.permission.WRITE_INTERNAL_STORAGE";
           head.name = "scorpionc";
           head.permission_t=2;
           
           head.filesize.byte1 = cres.size_t.byte1;
           head.method_size.byte1 = method_t;
           encode_header(head, __new_buf__);
           
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
 
 */