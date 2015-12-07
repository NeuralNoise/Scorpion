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
         lexr::token t;
         return t;
     }
     
}

 namespace memoryhelper 
 {
       string ttostr(int type)
       {
           if(type == typedef_byte)
              return "byte";
           else if(type == typedef_char)
              return "char";
           else if(type == typedef_class)
              return "class";
           else if(type == typedef_double)
              return "double";
           else if(type == typedef_float)
              return "float";
           else if(type == typedef_int)
              return "int";
           else if(type == typedef_long)
              return "long";
           else if(type == typedef_short)
              return "short";
           else if(type == typedef_string)
              return "string";
           else 
              return "unknown";
       }
       
       int ttoint(string type)
       {
           if(type == "byte")
              return typedef_byte;
           else if(type == "char")
              return typedef_char;
           else if(type == "class")
              return typedef_class;
           else if(type == "double")
              return typedef_double;
           else if(type == "float")
              return typedef_float;
           else if(type == "int")
              return typedef_int;
           else if(type == "long")
              return typedef_long;
           else if(type == "short")
              return typedef_short;
           else if(type == "string")
              return typedef_string;
           else 
              return -1;
       }
       
       string atostr(int access)
       {
          if(access == access_private)
             return "private";
          else if(access == access_protected)
             return "protected";
          else if(access == access_public)
             return "public";
          else 
             return "unknown";
       }
       
       void level1(int OP)
       {
           u2 init;
           init.byte1=OP;
           init.byte2=0;
          
           double* op_data;
          
           if(cglobals.classdepth == 0)
              cmplr_add_item( new_cmplr_item(init, op_data, "") ); 
           else if(cglobals.classdepth == 1)
              cglobals.classParent.C[0].iqueue.add( new_cmplr_item(init, op_data, "") ); // add item to main queue
           else cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue.add( new_cmplr_item(init, op_data, "") ); // add item to sub queue
       }
       
       void level2(int OP, double arg1)
       {
           u2 init;
           init.byte1=OP;
           init.byte2=1;
          
           double* op_data=new double[1];
           op_data[0] = arg1;
          
           if(cglobals.classdepth == 0)
              cmplr_add_item( new_cmplr_item(init, op_data, "") ); 
           else if(cglobals.classdepth == 1)
              cglobals.classParent.C[0].iqueue.add( new_cmplr_item(init, op_data, "") ); // add item to main queue
           else cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue.add( new_cmplr_item(init, op_data, "") ); // add item to sub queue 
       }
       
       void level3(int OP, double arg1, double arg2)
       {
           u2 init;
           init.byte1=OP;
           init.byte2=2;
          
           double* op_data=new double[2];
           op_data[0] = arg1;
           op_data[1] = arg2;
          
           if(cglobals.classdepth == 0)
              cmplr_add_item( new_cmplr_item(init, op_data, "") ); 
           else if(cglobals.classdepth == 1)
              cglobals.classParent.C[0].iqueue.add( new_cmplr_item(init, op_data, "") ); // add item to main queue
           else cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue.add( new_cmplr_item(init, op_data, "") ); // add item to sub queue
       }
       
       void level4(int OP, double arg1, double arg2, double arg3)
       {
           u2 init;
           init.byte1=OP;
           init.byte2=3;
          
           double* op_data=new double[3];
           op_data[0] = arg1;
           op_data[1] = arg2;
           op_data[2] = arg3;
          
           if(cglobals.classdepth == 0)
             cmplr_add_item( new_cmplr_item(init, op_data, "") ); 
           else if(cglobals.classdepth == 1)
             cglobals.classParent.C[0].iqueue.add( new_cmplr_item(init, op_data, "") ); // add item to main queue
           else cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue.add( new_cmplr_item(init, op_data, "") ); // add item to sub queue 
       }
       
       void _cout(string data)
       {
           u2 init;
           init.byte1=OP_COUT;
           init.byte2=0;
          
           double* op_data;
          
            if(cglobals.classdepth == 0)
              cmplr_add_item( new_cmplr_item(init, op_data, data) ); 
            else if(cglobals.classdepth == 1)
              cglobals.classParent.C[0].iqueue.add( new_cmplr_item(init, op_data, data) ); // add item to main queue
            else cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue.add( new_cmplr_item(init, op_data, data) ); // add item to sub queue
       }
       
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
             if(at(i).type == o.type && at(i)._namespace == o._namespace)
             {
                if(at(i).name == o.name && at(i).parentclass == o.parentclass) 
                      return true;
             }
          }
          
          return false;
       }
       
       Object ofc(std::string _namesp, std::string parentclass)
       {
          Object o;
          o.type = 9999;
          if(!objects._init())
            return o;
            
          for(long i =0; i < objects.size(); i++)
          {
             if((at(i).type != typedef_class && at(i).type != typedef_node) && at(i)._namespace == _namesp)
             {
                if(at(i).parentclass == parentclass) 
                      return at(i);
             }
          }
          
          return o;
       }
       
       long oadr(Object &o, const bool internal = true)
       {
          if(!objects._init())
            return 0;
            
          for(long i =0; i < objects.size(); i++)
          {
             if(at(i).type == o.type && at(i)._namespace == o._namespace )
             {
                if(at(i).name == o.name && at(i).parentclass == o.parentclass) 
                      return ((internal) ? i : at(i).eadr.byte1);
             }
          }
          
          return 0;
       }
       
       string oinfo(Object &o)
       {
          if(!objects._init())
            return "<null>";
        
          stringstream ss;    
          for(long i =0; i < objects.size(); i++)
          {
             if(at(i).type == o.type && at(i).name == o.name)
             {
                if(at(i)._namespace == o._namespace && at(i).parentclass == o.parentclass)
                {
                    ss << "from package:" << at(i).package << "  ";
                    
                    if(o._namespace != "<null>")
                        ss << o._namespace << "::";
                    
                    ss << atostr(at(i).access) << " ";
                    if(at(i).isStatic)
                       ss << "static ";
                    if(at(i).isConst)
                       ss << "const ";
                    
                    if(o.parentclass != "<null>")
                        ss << o.parentclass << ".";
                        
                    ss << o.name << ((at(i).isarray) ? "[]" : "") << " `" << ttostr(o.type) << "`";
                    return ss.str();
                }
             }
          }
          
          return "<null>";
       }
     
       bool exists(Object &o)
       { return ocmp(o); }
       
       bool find(std::string name, int type, std::string _namespace, 
              std::string parentclass)
       { 
           Object o;
           o.name = name;
           o.type = type;
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
           o.size_t.byte1 = 0;
           o.eadr.byte1 = cglobals.objectadr++;
           
           if(type == typedef_class)
               o.C = new ClassObject[1];
           
           if(ocmp(o))
             return false;
           else {
               objects.add(o);
               return true;
           }
       }
       
       long address(std::string name, int type, std::string _namespace, 
                 std::string parentclass, const bool internal = true)
       {
           Object o;
           o.name = name;
           o.type = type;
           o._namespace = _namespace;
           o.parentclass = parentclass;
           
           return oadr(o, internal);
       }
       
       string getobjinfo(std::string name, int type, std::string _namespace, 
                      std::string parentclass)
       {
           Object o;
           o.name = name;
           o.type = type;
           o._namespace = _namespace;
           o.parentclass = parentclass;
           return oinfo(o);
       }
    }
    
    namespace methodhelper
    {
       string argstostr(ListAdapter<Object> &args);
       bool sameargs(ListAdapter<Object> args1, ListAdapter<Object> args2);
       
       Method at(long i)
       { return methods.valueAt(i); }
       
       // TODO: create a sameargs(Object argset1, Object argset2) to compare method arguments
       bool mcmp(Method &o, ListAdapter<Object> &args)
       {
          if(!methods._init())
            return false;
            
          for(long i =0; i < methods.size(); i++)
          {
             if(at(i).name == o.name && at(i)._namespace == o._namespace)
             {
                if(at(i).parentclass == o.parentclass && sameargs(at(i).args, args) ) 
                      return true;
             }
          }
          
          return false;
       }
       
       long madr(Method &o, ListAdapter<Object> &args, bool internal)
       {
          if(!methods._init())
            return 0;
            
          for(long i =0; i < methods.size(); i++)
          {
             if(at(i).name == o.name && at(i)._namespace == o._namespace)
             {
                if(at(i).parentclass == o.parentclass && sameargs(at(i).args, args) ) 
                      return ((internal) ? i : at(i).eadr.byte1);
             }
          }
          
          return 0;
       }
       
       string minfo(Method &o, ListAdapter<Object> &args)
       {
          if(!methods._init())
            return "<null>";
        
          stringstream ss;    
          for(long i =0; i < methods.size(); i++)
          {
             if(at(i).name == o.name || at(i)._namespace == o._namespace)
             {
                if(at(i).parentclass == o.parentclass && sameargs(at(i).args, args) )
                {
                    ss << "from package:" << at(i).package << "  ";
                    
                    if(o._namespace != "<null>")
                        ss << o._namespace << "::";
                    
                    ss << atostr(at(i).access) << " ";
                    if(at(i).isStatic)
                       ss << "static ";
                       
                    if(o.parentclass != "<null>")
                        ss << o.parentclass << ".";
                        
                    ss << o.name << "(" << argstostr(args) << ") `function`"; // TODO: create a getargs() to turn the methods args to a string
                    return ss.str();
                }
             }
          }
          
          return "<null>";
       }
       
       bool sameargs(ListAdapter<Object> args1, ListAdapter<Object> args2)
       {
           if(!args1._init() || !args2._init())
              return false;
              
           if(args1.size() != args2.size())
             return false;
             
           for(int i = 0; i < args1.size(); i++)
           {
               if((args1.valueAt(i).type != args2.valueAt(i).type) || 
                    (args1.valueAt(i).array() != args2.valueAt(i).array()))
                      return false;
           }
          return true;
       }
       
       string argstostr(ListAdapter<Object> &args)
       {
           stringstream ss;
           if(!args._init())
             return ""; // A function can have no args
             
           for(int i = 0; i < args.size(); i++)
           {
               ss << ttostr(args.valueAt(i).type) << ((args.valueAt(i).array()) ? "[]" : "") 
                    << " " << args.valueAt(i).name << (((i + 1) < args.size()) ? "," : "");
           }     
             
           return ss.str();
       }
       
       bool exists(Method &o, ListAdapter<Object> &args)
       { return mcmp(o, args); }
       
       bool find(std::string name, std::string _namespace, std::string parentclass, 
              ListAdapter<Object> &args)
       { 
           Method o;
           o.name = name;
           o._namespace = _namespace;
           o.parentclass = parentclass;
           return mcmp(o, args); 
       }
       
       bool create(std::string name, bool isStatic, int access, std::string package, 
                std::string _namespace, std::string parentclass, ListAdapter<Object> &args)
       {
           Method o;
           o.name = name;
           o.access = access;
           o.package = package;
           o.args = args;
           o.eadr.byte1 = cglobals.methodadr++;
           o._namespace = _namespace;
           o.parentclass = parentclass;
           
           if(mcmp(o, args))
             return false;
           else {
               methods.add(o);
               return true;
           }
       }
       
       long address(std::string name, std::string _namespace, std::string parentclass, 
                 ListAdapter<Object> &args, const bool internal = true)
       {
           Method o;
           o.name = name;
           o._namespace = _namespace;
           o.parentclass = parentclass;
           
           return madr(o, args, internal);
       }
       
       string getmethodinfo(std::string name, std::string _namespace, 
                 std::string parentclass, ListAdapter<Object> &args)
       {
           Method o;
           o.name = name;
           o._namespace = _namespace;
           o.parentclass = parentclass;
           return minfo(o, args);
       }
       
    }
    
    namespace queuehelper
    {
        namespace classhelper
        {
            
            void getAllNodeObjects(ListAdapter<Object> &nodeObjects, std::string _namesp, std::string parentclass)
            {
                Object o;
                bool next=true;
                do
                {
                    o = memoryhelper::objecthelper::ofc(_namesp, parentclass);
                    if(o.type != 9999) next = false;
                    else 
                        nodeObjects.add(o);
                }while(next);
            }
            
            void inc()
            {
                if(cglobals.classdepth == 0){ return; }
                else if(cglobals.classdepth == 1){ cglobals.classParent.size_t.byte1++; }
                else cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).size_t.byte1++;
            }
            
            bool inserted = false;
            
            long queuesize()
            {
                if(cglobals.classdepth == 0){ return 0; }
                else if(cglobals.classdepth == 1){ cglobals.classParent.C[0].iqueue.size(); }
                else cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue.size();
            }
            
            /* Insert a class init instruction to the beginning of the queue */
            void insert(long size, std::string name, int type, std::string _namespace, 
                    std::string parentclass)
            {
                if(cglobals.classdepth == 0)
                { return; }
                
                if(cglobals.classdepth == 1)
                {
                    if(queuesize() == 0)
                    {
                        level3(OP_NODE, memoryhelper::objecthelper::address
                                    (name, type, _namespace, parentclass, false), size);
                    }
                    else 
                    {
                        u2 init;
                        init.byte1=OP_NODE;
                        init.byte2=2;
                      
                        double* op_data=new double[2];
                        op_data[0] = memoryhelper::objecthelper::address
                                        (name, type, _namespace, parentclass, false);
                        op_data[1] = size;
                        cglobals.classParent.C[0].iqueue.insert( new_cmplr_item(init, op_data, ""), 0 );
                        
                        /* Apply all node objects to create inital map */
                        /*ListAdapter<Object> nodeObjects;
                        getAllNodeObjects( nodeObjects , _namespace , parentclass );
                        
                        for(long i = 0; i < nodeObjects.size(); i++)
                        {
                            
                            init.byte1=OP_NICCONST;
                            init.byte2=2;
                          
                            double* op_data=new double[2];
                            op_data[0] = memoryhelper::objecthelper::address
                                            (name, type, _namespace, parentclass, false);
                            op_data[1] = size;
                            cglobals.classParent.C[0].iqueue.insert( new_cmplr_item(init, op_data, ""), 0 );
                        }*/
                    }
                }
                else
                {
                    if(queuesize() == 0)
                    {
                        level3(OP_NODE, memoryhelper::objecthelper::address
                                    (name, type, _namespace, parentclass, false), size);
                    }
                    else 
                    {
                        u2 init;
                        init.byte1=OP_NODE;
                        init.byte2=2;
                      
                        double* op_data=new double[2];
                        op_data[0] = memoryhelper::objecthelper::address
                                        (name, type, _namespace, parentclass, false);
                        op_data[1] = size;
                        cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue
                             .insert( new_cmplr_item(init, op_data, ""), 0);
                    }
                    
                }    
                inserted = true;
            }
            
            /* Release a queue */
            void release()
            {
                if(!inserted || cglobals.classdepth == 0)
                {
                    cglobals.out << "Class init instruction was not added to the queue.";
                    error(cglobals.lex);
                    return;
                }
                
                long len = queuesize();
                for(long i = 0; i < len; i++)
                {
                    cmplr_item item1;
                    
                    if(cglobals.classdepth == 1)
                    { item1 = cglobals.classParent.C[0].iqueue.valueAt(i); }
                    else item1 = cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue.valueAt(i);
            
                    cmplr_add_item( item1 );
                }
                
                inserted = false;
                cglobals.classdepth--;
            }
        }
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
           bool plus=false, needStr=false, finished=false;
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
               else if(temp_t.value == ","){
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
  
        	      temp_t = getNextToken(lex);
                 if(temp_t.type == temp_t.e_symbol)
                 {
                     string var = temp_t.value;
        	         temp_t = getNextToken(lex);
        	         if(temp_t.value != "=")
        	         {
        	             cglobals.out << "Expected '=' before '" << temp_t.value << "'.";
                         error(cglobals.lex);
        	         }
        	         parse_string_decliration(lex, var);
                 }
                 else if(temp_t.value == ";")
                 {
                     cglobals.out << "Unexpected unqualified symbol before '" << temp_t.value << "'.";
                     error(cglobals.lex);
                 }
                 else {
                     cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                     error(cglobals.lex);
                 }
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
                   
                   if(finished)
                      break;
                      
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
       
       void parse_return_decliration(lexr::parser_helper& lex, long addr)
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
                   
                 //  cout << "return \"" << ss.str() << "\"" << endl;
                   level3(OP_ICONST, var_return, atof(ss.str().c_str()));
                   level2(OP_PUSH, var_return);
                   level2(OP_RETURN, addr);
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
           bool plus=false, needNum=false, singleNum=true,
                negate=false, finished=false;
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
                   if(!negate)
                     ss << temp_t.value;
                   else 
                     ss << "-" << temp_t.value;
               }
               else if(temp_t.value == ",")
               {
                   finished = true;
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
                   
        	     temp_t = getNextToken(lex);
                 if(temp_t.type == temp_t.e_symbol)
                 {
                     string var = temp_t.value;
        	         temp_t = getNextToken(lex);
        	         if(temp_t.value != "=")
        	         {
        	             cglobals.out << "Expected '=' before '" << temp_t.value << "'.";
                         error(cglobals.lex);
        	         }
        	         parse_byte_decliration(lex, var);
                 }
                 else if(temp_t.value == ";")
                 {
                     cglobals.out << "Unexpected unqualified symbol before '" << temp_t.value << "'.";
                     error(cglobals.lex);
                 }
                 else {
                     cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                     error(cglobals.lex);
                 }
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
                   if(finished)
                      break;
                   
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
               else if(temp_t.value == "-")
                   negate = true;
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
       
       long parse_asm_decliration(long begin, string _asm)
       {
           lexr::token temp_t;
           bool intro=false;
        
           lexr::parser_helper lex;
           lex.init(_asm);
           lex.lexer().line_t = begin;
           
           for( ;; )
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
                 break;
               if(temp_t.value == "cout")
               {
                   temp_t = getNextToken(lex);
                   if(temp_t.type == temp_t.e_string)
                       _cout(temp_t.value);
                   else
                   {
                       if(!intro)
                         cout << "Assembler messages:\n";
                       cglobals.out << "Expected string literal before '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
               }
               else {
                   if(!intro)
                      cout << "Assembler messages:\n";
                   cglobals.out << "Expected assembly instruction before '" << temp_t.value << "'.";
                   error(cglobals.lex);
               }
           }
           
           cglobals.eof=0;
           return lex.lexer().line_t;
       }
       
       void parse_method_args(lexr::parser_helper& lex, ListAdapter<Object> &args)
       {
           lexr::token temp_t;
           bool comma;
           
           for( ;; )
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
                 break;
               if(temp_t.value == ")" || (reserved(temp_t.value) && !_typedef(temp_t.value)) 
                  || temp_t.value == "{")
               {
                   if(temp_t.value != ")")
                   {
                       cglobals.out << "Expected ')' at end of function decliration before '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                   
                   if(comma)
                   {
                       cglobals.out << "Expected qualified symbol before ')'.";
                       error(cglobals.lex);
                   }
                   break;
               }
               else if(temp_t.type == temp_t.e_symbol)
               {
                   Object o;
                   comma = false;
                   if(!_typedef(temp_t.value))
                   {
                       cglobals.out << "Expected qualified symbol before '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                   
                   o.type = ttoint(temp_t.value);
                   
                   temp_t = getNextToken(lex);
                   if(temp_t.value == "[")
                   {
                       temp_t = getNextToken(lex);
                       if(temp_t.type == temp_t.e_number)
                       {
                           cglobals.out << "Cannot have array size in function argument definition '[" << temp_t.value << "]'.";
                           error(cglobals.lex);
                           temp_t = getNextToken(lex);
                       }
                       
                       if(temp_t.value == "]")
                       { temp_t = getNextToken(lex); }
                       o.isarray = true;
                   }
                   
                   if(temp_t.type != temp_t.e_symbol)
                   {
                       cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
                   o.name = temp_t.value;
                  args.add(o);   
               }
               else if(temp_t.value == ",")
               {
                   if(comma)
                   {
                       cglobals.out << "Expected qulafied symbol before '" << temp_t.value << "'.";
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
       
       void parse_method_block(lexr::parser_helper& lex, int block_begin, long address)
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
               else if(temp_t.value == "{")
                  cglobals.block_stack++;
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
                           memoryhelper::helper::parse_return_decliration(cglobals.lex, address);
                   else if(temp_t.value == "asm")
                   {
                      temp_t = getNextToken(lex);
                      if(temp_t.value != "(")
                      {
                          cglobals.out << "Expected '(' before '" << temp_t.value << "'.";
                          error(cglobals.lex);
                      }
                      
                      temp_t = getNextToken(lex);
                      if(temp_t.type == temp_t.e_string)
                      {
                          cglobals.lex.lexer().line_t = memoryhelper::helper::parse_asm_decliration(cglobals.lex.lexer().line_t, temp_t.value);
                      }
                      else {
                          cglobals.out << "Expected string literal before '" << temp_t.value << "'.";
                          error(cglobals.lex);
                      }
                      
                      temp_t = getNextToken(lex);
                      if(temp_t.value != ")")
                      {
                          cglobals.out << "Expected ')' before '" << temp_t.value << "'.";
                          error(cglobals.lex);
                      }
                      
                      temp_t = getNextToken(lex);
                      if(temp_t.value != ";")
                      {
                          cglobals.out << "Expected ';' at end of statement.";
                          error(cglobals.lex);
                      }
                   }
                   else {
                       cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                       error(cglobals.lex);
                   }
               }
               else if(temp_t.value == ";")
               { }
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
                   if(accessspecifier(temp_t.value) || temp_t.value == "static" || temp_t.value == "const"
                       || temp_t.value == "def")
                   {
                       object_attrib atribs;
                       if(temp_t.value != "def"){
                           atribs.access = ((temp_t.value == "static" || temp_t.value == "const") ? access_public : memoryhelper::helper::strtoaccess(temp_t.value));
                           if(temp_t.value == "static" || temp_t.value == "const") cglobals.lex.lexer().token_sz1--;
                           
                           memoryhelper::helper::parse_access_specifier(cglobals.lex, atribs, temp_t);
                       }
                       else {
                           atribs.access = access_public;
                           atribs.isStatic = false;
                           atribs.isConst = false;
                       }
                       
                       if(temp_t.value == "def"){
                           cglobals.method_t++;
                           temp_t = getNextToken(lex);
                           if(temp_t.type != temp_t.e_symbol){
                               cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }   
                               
                           
                         //  cout << "def " << temp_t.value << endl;
                           string functionname = temp_t.value;
                           bool methodfound = false;
                           
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "("){
                               cglobals.out << "Expected '(' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           ListAdapter<Object> args;
                           memoryhelper::helper::parse_method_args(cglobals.lex, args);
                           
                           
                           string parentclass = "";
                           if(cglobals.classdepth == 0)
                              parentclass = "<null>";
                           else if(cglobals.classdepth == 1)
                              parentclass = cglobals.classParent.name;
                           else parentclass = cglobals.classParent.C[0].classObjects.valueAt(cglobals.classdepth-1).C[0].name;
                           
                           if(!memoryhelper::methodhelper::create(functionname, atribs.isStatic, atribs.access, 
                                    cglobals.package, "<null>", parentclass, args))
                           {
                               methodfound = true;
                               cglobals.out << "Symbol of type `function` has already been declared.\nPreviously declared here:\n\t" 
                                    << memoryhelper::methodhelper::getmethodinfo(functionname, "<null>", parentclass, args);
                               error(cglobals.lex);    
                           }
                           
                          Object arg1;
                          arg1.type = typedef_string;
                          arg1.isarray = true;
                         
                          ListAdapter<Object> initargs, functionargs = methods.valueAt(memoryhelper::methodhelper::address
                                                                         (functionname, "<null>", parentclass, args)).args;
                          initargs.add(arg1);
     
                           if(!methodfound && functionname == "__init__" && memoryhelper::methodhelper::sameargs(initargs, functionargs)
                              && parentclass == "Starter")
                           {
                               long adr = memoryhelper::methodhelper::address("__init__", "<null>", parentclass, functionargs);
                               Method m = methods.valueAt(adr);
                               m.eadr.byte1 = 0;
                            
                               methods.replace(m, adr);
                               
                               if(!atribs.isStatic)
                               {
                                   cglobals.out << "Entry point method 'public static def __init__(string[] args)' must be static.";
                                   error(cglobals.lex);
                               }
                               
                               if(atribs.access != access_public)
                               {
                                   cglobals.out << "Entry point method 'public static def __init__(string[] args)' must be public.";
                                   error(cglobals.lex);
                               }
                               
                               cglobals.hasInit = true;
                           }
                           
                           if(atribs.isConst)
                           {
                               cglobals.out << "Method '" << memoryhelper::atostr(atribs.access) << " " 
                                              << ((atribs.isStatic) ? "static " : "") << ((atribs.isConst) ? "const " : "") 
                                                << functionname << "(" << memoryhelper::methodhelper::argstostr(functionargs) << ")' cannot be const.";
                               error(cglobals.lex);
                           }
                           
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "{"){
                               cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                          
                          initargs.clear();
                          level2(OP_MTHD, memoryhelper::methodhelper::address(functionname, "<null>", parentclass, 
                                 functionargs)); 
                          memoryhelper::helper::parse_method_block(cglobals.lex, cglobals.block_stack, memoryhelper::methodhelper::address
                                         (functionname, "<null>", parentclass, functionargs));
                          
                          level3(OP_ICONST, var_return, 0); 
                          level2(OP_PUSH, var_return); 
                          level2(OP_RETURN, memoryhelper::methodhelper::address(functionname, "<null>", parentclass, 
                                 functionargs)); 
                          functionargs.clear();
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
                               
                           
                          // cout << "def " << temp_t.value << endl;
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "("){
                               cglobals.out << "Expected '(' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           ListAdapter<Object> args;
                           memoryhelper::helper::parse_method_args(cglobals.lex, args);
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "{"){
                               cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                          memoryhelper::helper::parse_method_block(cglobals.lex, cglobals.block_stack, 0);
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
                           
                       
                     //  cout << "def " << temp_t.value << endl;
                       temp_t = getNextToken(lex);
                       if(temp_t.value != "("){
                           cglobals.out << "Expected '(' before '" << temp_t.value << "'.";
                           error(cglobals.lex);
                       }
                       
                       ListAdapter<Object> args;
                       memoryhelper::helper::parse_method_args(cglobals.lex, args);
                       temp_t = getNextToken(lex);
                       if(temp_t.value != "{"){
                           cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                           error(cglobals.lex);
                       }
                       
                      memoryhelper::helper::parse_method_block(cglobals.lex, cglobals.block_stack,0);
                       
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
            || symbol == "package" || symbol == "namespace" || symbol == "asm");
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

void parse_cmplr_items(stringstream &out_buf)
 {
     cres.size_t.byte1 = cplrfreelist2[0].c_items.size();
     for(unsigned long i =0; i<cplrfreelist2->c_items.size(); i++)
     {
         cplrfreelist1.add(cplrfreelist2->c_items.valueAt(i));
         unsigned long ins= cplrfreelist1.valueAt(0).item.byte1;
         cres.size_t.byte1 += cplrfreelist1.valueAt(0).size_t.byte1;
         
       //  cout << "ins " << ins << endl;
         if(ins == OP_MTHD){
                Method m= methods.valueAt(cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1);
                
                out_buf << (char) cplr_method << OP_MTHD << (char) 0 << m.name << "&" << m.parentclass << "&" << m.package << (char) 0;
                out_buf << m.eadr.byte1 << (char) 0;
         }
         else if(ins == OP_NODE) cres.size_t.byte1 -= 3;
         else if(ins == OP_COUT){
             cres.size_t.byte1 +=cplrfreelist1.valueAt(0).str.size();
             out_buf << (char) cplr_string_instr << OP_COUT << (char) 0 << cplrfreelist1.valueAt(0).str << (char) 0;
         }
         else if(ins == OP_NOP || ins == OP_END || ins == OP_HLT || ins == OP_NO || ins == OP_ENDNO)
             out_buf << (char) cplr_instr <<  ins << (char) 0 ;
         else if( ins == OP_RETURN || ins == OP_CALL)
         {
              Method m= methods.valueAt(cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1);
       //       cout << "returning " << m.eadr.byte1 << endl;
              out_buf << (char) cplr_instr << ins << (char) 0 << m.eadr.byte1 << (char) 0;
         }
         else if(ins == OP_PUSH || ins == OP_POP || ins == OP_JMP || ins == OP_LBL || ins == OP_IF || ins == OP_INC || ins == OP_DEC
                 || ins == OP_KILL || ins == OP_DELETE || ins == OP_DELETE_ARRY){ // for instructions that access memory
    //           cout << ins << " -> " << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << endl;
               out_buf << (char) cplr_instr <<  ins << (char) 0  << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << (char) 0;
         }        
         else if(ins == OP_SCONST || ins == OP_BCONST || ins == OP_CCONST || ins == OP_RSHFT 
                   || ins == OP_LSHFT || ins == OP_CIN || ins == OP_JIF 
                   || ins == OP_JIT || ins == OP_ICONST || ins == OP_DCONST || ins == OP_FCONST 
                   || ins == OP_THROW)
         {
                out_buf << (char) cplr_instr <<  ins << (char) 0  << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << (char) 0
                << cplrfreelist1.valueAt(0).sub_item.valueAt(1).item.byte1 << (char) 0;   
       //         cout << ins << " -> " << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << " : " << cplrfreelist1.valueAt(0).sub_item.valueAt(1).item.byte1 << endl;
         }
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
                out_buf << (char) cplr_instr <<  ins << (char) 0  << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << (char) 0
                << cplrfreelist1.valueAt(0).sub_item.valueAt(1).item.byte1 << (char) 0 << cplrfreelist1.valueAt(0).sub_item.valueAt(2).item.byte1 << (char) 0;
                
                
         cmplrfree( cplrfreelist1 );
     }
 }

int Compilr_Compile_Buf(Archive &zip_archive, stringstream &out_buf)
 {
     cglobals.success = 0;
     cglobals.classdepth = 0;
     cglobals.namespacedepth = 0;
     cglobals.hasInit = false;
     cglobals.hasStarter = false;
     cglobals.methodadr = 1;
     cglobals.objectadr = 50;
     
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
        
        zippy.Cleanup();
        
        lexr::token temp_t;
        
        cglobals.lex.init(source.str());
        source.str("");
        
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
                  else if(memoryhelper::helper::accessspecifier(temp_t.value) || temp_t.value == "static" || temp_t.value == "const"
                          || temp_t.value == "class")
                   {
                       object_attrib atribs;
                       if(temp_t.value != "class") {
                           atribs.access = ((temp_t.value == "static" || temp_t.value == "const") ? access_public : memoryhelper::helper::strtoaccess(temp_t.value));
                           if(temp_t.value == "static" || temp_t.value == "const") cglobals.lex.lexer().token_sz1--;
                           
                           memoryhelper::helper::parse_access_specifier(cglobals.lex, atribs, temp_t);
                       }
                       else {
                           atribs.access = access_public;
                           atribs.isStatic = false;
                           atribs.isConst = false;
                       }
                       
                       if(temp_t.value == "class"){
                           temp_t = getNextToken(cglobals.lex);
                           if(temp_t.type != temp_t.e_symbol){
                               cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                          //cout << "class " << temp_t.value << endl;
                           string classname = temp_t.value;
                           bool classfound = false;
                           
                           if(!memoryhelper::objecthelper::create(temp_t.value, typedef_class, atribs.isStatic, false, 
                                  atribs.access, cglobals.package, "<null>", "<null>"))
                           {
                               classfound = true;
                               cglobals.out << "Symbol of type `class` has already been declared.\nPreviously declared here:\n\t" 
                                    << memoryhelper::objecthelper::getobjinfo(temp_t.value, typedef_class, "<null>", "<null>");
                               error(cglobals.lex);
                           }
                           
                           cglobals.classdepth++;
                           if(!classfound && classname == "Starter")
                           {
                               if(atribs.access != access_public)
                               {
                                   cglobals.out << "Main class 'Starter' must be public.";
                                   error(cglobals.lex);
                               }
                               
                               if(atribs.isStatic)
                               {
                                   cglobals.out << "Main class 'Starter' cannot be static.";
                                   error(cglobals.lex);
                               }
                               
                               cglobals.hasStarter = true;
                           }
                           
                           if(atribs.isConst)
                           {
                               cglobals.out << "Object '" << classname << "' of type `class` cannot be const.";
                               error(cglobals.lex);
                           }
                           
                           temp_t = getNextToken(cglobals.lex);
                           if(temp_t.value != "{"){
                               cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                               error(cglobals.lex);
                           }
                           
                           if(cglobals.classParent.C != nullptr)
                           {
                               if(cglobals.classParent.C[0].classObjects._init())
                                  cglobals.classParent.C[0].classObjects.clear();
                               if(cglobals.classParent.C[0].iqueue._init())
                                  cglobals.classParent.C[0].iqueue.clear();
                           }
                             
                           cglobals.classParent = objects.valueAt(memoryhelper::objecthelper::address
                                                       (classname, typedef_class, "<null>", "<null>"));
                          
                          memoryhelper::helper::parse_class_block(cglobals.lex, cglobals.block_stack);
                           
                     //     memoryhelper::queuehelper::classhelper::insert(cglobals.classParent.size_t.byte1, classname, 
                     //                 typedef_class, "<null>", "<null>");
 memoryhelper::queuehelper::classhelper::inserted = true;
                          memoryhelper::queuehelper::classhelper::release();
                       }
                       else {
                           cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                           error(cglobals.lex);
                       }
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
     
     if(!cglobals.hasStarter)
     {
         cglobals.out << "Main class 'Starter' is required in a Scorpion application.";
         error(cglobals.lex);
     }
     
     if(!cglobals.hasInit)
     {
         cglobals.out << "Entry point method 'public static def __init__(string[] args)' is required in a Scorpion application.";
         error(cglobals.lex);
     }
     
     if(cglobals.block_stack != 0)
     {
         cglobals.out << "Expected '}' at end of input.";
         error(cglobals.lex);
     }
     
     if(cglobals.success == 0)
        parse_cmplr_items(out_buf);
     
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
        
        cout << "failed\n";
        return success;

     }
     else {
         cres.reason << "compilr:  error: library is empty.\n";
         return COMPILR_FAILURE;
     }
 }
 
 
