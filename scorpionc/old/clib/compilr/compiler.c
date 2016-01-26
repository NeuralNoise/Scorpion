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
 #include "../random.h"
 #include "core.h"
 #include "../filestream.h"
 #include "lexr/lexr.h"
 #include <sstream>
 #include <string>
 #include <iostream>
 #include <iomanip>
 
 using namespace std;
 
 Globals cglobals;
    /*
    * ***************************************
    * ***************************************
    *       Header File Symbols
    *
    * ***************************************
    * ***************************************
    */
    
    enum // header_flags
    {
      minor_version=1,
      major_version=2,
      target=3,
      base_target=4,
      version=5,
      debuggable=6,
      loggable=7,
      log_level=8,
      log_file=9,
      app_id=11,
      permissions=12,
      image_size=13,
      name=14,
      address_cap=15,
      method_cap=16,
      instruction_cap=17,
      eoh=18
    };


 struct EsoHeader {
    int           edian;
    u4            magic;
    u2            minor_version;
    u2            major_version;
    u1            target;
    u1            base_target;
    string        version;
    u1            debug;
    u1            loggable;
    u1            log_level;
    string        log_file;
    string        app_id;
    string*       permissions;
    int           permission_t;
    u1            filesize;            // The full size of the image section in bytes
    string        name;
    u1            address_cap;
    u1            instruction_cap;
    u1            method_size; 
 };
 
 struct object_attrib
 {
     int access;
     bool isStatic, isConst;
 };
 
 bool asmintro = false;
 void error(lexr::parser_helper &lex)
 {
	 if(!asmintro)
	 {
		asmintro = true;
		cout << "Assembler messages:\n";
	 }
     if(cglobals.infunction == 1)
     {
         cout << cglobals.file << ":  In function '" << cglobals.functioninfo << "':\n";
         cglobals.infunction = 2; // were still in a function
     }
     cout << cglobals.file << ":" << lex.lexer().line_t << "  error: " << cglobals.out.str() << endl;
     cglobals.success = 1;
     cglobals.out.str("");
 }
 
 void warning(lexr::parser_helper &lex)
 {
     cout << cglobals.file << ":" << lex.lexer().line_t << "  warning: " << cglobals.out.str() << endl;
     cglobals.out.str("");
 }
 
 lexr::token getNextToken(lexr::parser_helper &lexer){
	 lexr::token teof;
	 teof.value = "";
	 teof.type = teof.e_eof; 
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
         return teof;
       }
       else if(t.type == t.e_error && !cglobals.ignorestrays) { 
        cglobals.out << "Stray `" << t.value << "` in program.";
        error(lexer);
        return getNextToken(lexer);
       }
       else
         return t;
     }
     else {
         cglobals.eof=1;
         return teof;
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
           else if(type == typedef_bit)
              return "bit";
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
           else if(type == "bit")
              return typedef_bit;
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
       
       void _string(long address, string data)
       {
		   u2 init;
           init.byte1=OP_STRCONST;
           init.byte2=1;
          
           double* op_data = new double[1];
           op_data[0] = address;
          
            if(cglobals.classdepth == 0)
              cmplr_add_item( new_cmplr_item(init, op_data, data) ); 
            else if(cglobals.classdepth == 1)
              cglobals.classParent.C[0].iqueue.add( new_cmplr_item(init, op_data, data) ); // add item to main queue
            else cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue.add( new_cmplr_item(init, op_data, data) ); 
	   }
	   
	   int getesc(int tok)
	   {
	       if(tok == 'n') return (int) '\n';
	       else if(tok == 'v') return (int) '\v';
	       else if(tok == '\\') return (int) '\\';
	       else if(tok == 't') return (int) '\t';
	       else if(tok == 'b') return (int) '\b';
	       else if(tok == 'f') return (int) '\f';
	       else if(tok == 'r') return (int) '\r';
	       else if(tok == '"') return (int) '"';
	       else if(tok == '\'') return (int) '\'';
	       else return 0;
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
             if(((o.type == typedef_class) ? (at(i).type == typedef_class) : true) && at(i)._namespace == o._namespace)
             {
                if(at(i).name == o.name && at(i).parentclass == o.parentclass) 
                      return true;
             }
          }
          
          return false;
       }
     
       long oadr(Object &o, const bool internal = true)
       {
          if(!objects._init())
            return 0;
            
          for(long i =0; i < objects.size(); i++)
          {
             if(at(i)._namespace == o._namespace )
             { 
                if(((o.type == typedef_class) ? (at(i).type == typedef_class) : true) && at(i).name == o.name 
                     && at(i).parentclass == o.parentclass) 
                      return ((internal) ? i : at(i).eadr.byte1);
             }
          }
          
          return 0;
       }
       
       bool oreuse(Object &o)
       {
          if(!objects._init())
            return false;
            
          for(long i = 0; i < objects.size(); i++)
          {
              if(at(i).type == typedef_unused_object)
              {
                  objects.replace(o, i);
                  return true;
              }
          }
            
           return false;
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
           o.scopeLevel = ((cglobals.infunction == 0) ? -1 : cglobals.scopelevel); // TODO: Add scope level processing instead of local -1 if not local
           
           if(type == typedef_class)
           {
               o.C = new ClassObject[1];
               o.C[0].iqueue._init_();
           }
           
           if(ocmp(o))
             return false;
           else {
               o.eadr.byte1 = cglobals.objectadr++;
               if(!oreuse(o)) // reuse previously deleted objects in memory
                 objects.add(o);
           cout << "create " << name << " ADR " << (cglobals.objectadr) << endl;
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
       string nativeargstostr(ListAdapter<Object> &args);
       bool sameargs(ListAdapter<Object> args1, ListAdapter<Object> args2);
       
       Method at(long i)
       { return methods.valueAt(i); }
       
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
            return 1;
            
          for(long i =0; i < methods.size(); i++)
          {
             if(at(i).name == o.name && at(i)._namespace == o._namespace)
             {
                if(at(i).parentclass == o.parentclass && sameargs(at(i).args, args) ) 
                      return ((internal) ? i : at(i).eadr.byte1);
             }
          }
          
          return 1;
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
                    
                   ss << ((at(i).isnative) ? "native " : "" ) << "def ";   
                    if(o.parentclass != "<null>")
                        ss << o.parentclass << ".";
                        
                    ss << o.name << "(" << ((at(i).isnative) ? nativeargstostr(args) : argstostr(args)) << ") `function`"; 
                    return ss.str();
                }
             }
          }
          
          return "<null>";
       }
       
       string basic_minfo(Method &o, ListAdapter<Object> &args) // TODO: Process info for native functions
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
                    if(o._namespace != "<null>")
                        ss << o._namespace << "::";
                    
                    ss << atostr(at(i).access) << " ";
                    if(at(i).isStatic)
                       ss << "static ";
                    
                   ss << ((at(i).isnative) ? "native " : "" ) << "def ";   
                    if(o.parentclass != "<null>")
                        ss << o.parentclass << ".";
                        
                    ss << o.name << "("  << ((at(i).isnative) ? nativeargstostr(args) : argstostr(args)) << ")"; 
                    return ss.str();
                }
             }
          }
          
          return "<null>";
       }
       
       bool sameargs(ListAdapter<Object> args1, ListAdapter<Object> args2)
       {    
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
       
       string nativeargstostr(ListAdapter<Object> &args)
       {
           stringstream ss;
           if(!args._init())
             return ""; // A function can have no args
             
           for(int i = 0; i < args.size(); i++)
           {
               ss << ttostr(args.valueAt(i).type) << ((args.valueAt(i).array()) ? "[]" : "") 
                    << args.valueAt(i).name << (((i + 1) < args.size()) ? "," : "");
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
                std::string _namespace, std::string parentclass, ListAdapter<Object> &args, bool isnative = false)
       {
           Method o;
           o.name = name;
           o.access = access;
           o.package = package;
           o.args = args;
           o._namespace = _namespace;
           o.parentclass = parentclass;
           o.isnative = isnative;
           
           if(mcmp(o, args))
             return false;
           else {
			   Object arg1;
			   arg1.type = typedef_string;
			   arg1.isarray = true;
		 
			   ListAdapter<Object> initargs;
			   initargs.add(arg1);
				// TODO: find a better way to address method ptrs	  
			   if((cglobals.methodadr == 0 || cglobals.methodadr == 1) && 
			        !(name == "__init__" && parentclass == "Starter" && sameargs(initargs, args)))
                  o.eadr.byte1 = ++cglobals.methodadr;
               else if(name == "__init__" && parentclass == "Starter" && sameargs(initargs, args))
                  o.eadr.byte1 = 0;
			   else
			   {
				   if(cglobals.methodadr == 2) cglobals.methodadr++;
			       o.eadr.byte1 = cglobals.methodadr++;
               } 
                 
               methods.add(o);
               return true;
           }
       }
       
       long address(std::string name, std::string _namespace, std::string parentclass, 
                 ListAdapter<Object> &args, bool internal = true)
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
    
    string getparentclass()
    { 
       if(cglobals.classdepth == 0)
         return "<null>";
       else if(cglobals.classdepth == 1)
         return cglobals.classParent.name;
       else return cglobals.classParent.C->classObjects.valueAt( cglobals.classdepth-1 ).name;
    }
       
    namespace queuehelper
    {
        namespace classhelper
        {
            
            bool inserted = false;
            
            long queuesize()
            {
                if(cglobals.classdepth == 0){ return 0; }
                else if(cglobals.classdepth == 1){ return cglobals.classParent.C[0].iqueue.size(); }
                else return cglobals.classParent.C[0].classObjects.valueAt( cglobals.classdepth-1 ).C[0].iqueue.size();
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
    
    void run_gc() // kill all local variables for alloc
    {
     //  cout << "scope level " << cglobals.scopelevel << ":\n";
       for(unsigned long i = 0; i < objects.size(); i++)
       {
           if(objects.valueAt(i).scopeLevel == cglobals.scopelevel 
              && objects.valueAt(i).type != typedef_class)
           {
               level2(OP_KILL, objects.valueAt(i).eadr.byte1);
               //cout << "kill " << objects.valueAt(i).name << endl;
               Object objforalloc;
               objforalloc.type = typedef_unused_object;
               objects.replace(objforalloc, i); // Reuse old objects for alloc
           }
       }
       
    }
	   
    namespace helper
    {
       bool reserved(std::string symbol);    
       std::string reverse(string str);
       bool accessspecifier(std::string symbol);
       int strtoaccess(std::string s);
       bool _typedef(std::string symbol);
       int ttop(int type);
       int tatop(int type);
       int _strttot(std::string symbol);
       int _asm_strttot(std::string symbol);
       int _asm_strtop(string op);
       
       
       
       std::string parse_dot_notation(lexr::parser_helper& lex, bool dotfirst, std::string breaker, bool asterisk = false)
       {
          lexr::token temp_t;
          bool needsWord=true;
          int asterisk_flag = 0, err_flag=0;
          stringstream word;
          
          for( ;; )
          {
              temp_t = getNextToken(lex);
              if(temp_t.type == temp_t.e_eof)
              {
			     cglobals.out << "Unexpected end of file.";
			     error(lex);
                 break;
			  }
              if(temp_t.value == ".")
              {
                 if(asterisk_flag==5)
                   asterisk_flag++;
                   
                  if(needsWord && !dotfirst)
                  {
                     cglobals.out << "Expected symbol before '.'.";
                     error(lex);
                  }
                  
                  needsWord = true;
                  dotfirst = false;
                  word << ".";
              }
              else if(temp_t.type == temp_t.e_symbol)
              {
				 if(dotfirst)
				 {
					 cglobals.out << "Expected '.' before symbol '" << temp_t.value << "'.";
                     error(lex);
			     }
			     
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
                    error(lex);
                 }
                 
                 if(asterisk_flag > 5)
                 {
                    cglobals.out << "Expected ';' immediatley after '*'.";
                    error(lex);
                 }
                 
                 if(dotfirst)
				 {
					 cglobals.out << "Expected '.' before symbol '" << temp_t.value << "'.";
                     error(lex);
			     }
                break;
              }
              else if(temp_t.value == "*" & asterisk)
              {
				 if(dotfirst)
				 {
					 cglobals.out << "Expected '.' before symbol '*'.";
                     error(lex);
			     }
			     
                 asterisk_flag = 5;
                 word << "*";
              }
              else if(temp_t.type == temp_t.e_eof)
              {
                 cglobals.out << "Unexpected end of file.";
                 error(lex);
                 cglobals.eof=1;
              }
              else {
                 needsWord = true;
                 cglobals.out << "Unexpected symbol '" << temp_t.value << "', expecting '.' or ';'.";
                 error(lex);
                 err_flag++;
                 if(err_flag==2) break;
              }
          }
          return word.str();
       }

       struct wordmap
       {
           int j;
           ListAdapter<string>* wlist;
       };
       
       struct objmap
       {
           long adr, internal_adr;
           int type;
           string symbol;
       };
        
       wordmap parse_complex_dot_notation(lexr::parser_helper& lex, bool dotfirst, ListAdapter<std::string> breakers, bool isasm = false, bool asterisk = false)
       {
          lexr::token temp_t;
          bool needsWord=false;
          int err_flag=0, mapsz=0;
          ListAdapter<std::string> tmplist;
          wordmap wmap;
          
          for( ;; )
          {
              temp_t = getNextToken(lex);
              //cout << "text " << temp_t.value << endl;
              if(temp_t.type == temp_t.e_eof)
              {
				 if(dotfirst)
				 {
					 cglobals.out << "Expected '.' before symbol '" << temp_t.value << "'.";
                     error(lex);
			     }
			     
				 if(breakers.size() ==0)
				 {
					 if(needsWord)
					 {
						cglobals.out << "Expected symbol before '" << temp_t.value << "'.";
						error(lex);
					 }
					 
					mapsz++;
					lex.lexer().token_sz1--;
					break;
			     }
			     break;  
			  }
              if(temp_t.value == ".")
              {
                   
                  if(needsWord && !dotfirst)
                  {
                     cglobals.out << "Expected symbol before '.'.";
                     error(lex);
                  }
                  
                  needsWord = true;
                  dotfirst = false;
              }
              else if(temp_t.type == temp_t.e_symbol)
              {
				 if(dotfirst)
				 {
					 cglobals.out << "Expected '.' before symbol '" << temp_t.value << "'.";
                     error(lex);
			     }
			     
                 if(breakers.size()==0)
                 {
                     if((isasm && _asm_strtop(temp_t.value) != -33) || reserved(temp_t.value))
                     {
                         if(needsWord)
    					 {
    						cglobals.out << "Expected symbol before '" << temp_t.value << "'.";
    						error(lex);
    					 }
    					
    					mapsz++;
    					lex.lexer().token_sz1--;
    					break;
                     }
                 }
                
                 tmplist.add(temp_t.value);
                 needsWord = false;
              }
              else if(temp_t.value == ":")
              {
				 if(dotfirst)
				 {
					 cglobals.out << "Expected '.' before symbol '" << temp_t.value << "'.";
                     error(lex);
			     }
			     
				 temp_t = getNextToken(lex);
				 if(temp_t.value != ":")
                 {
					 cglobals.out << "Expected ':' symbol before '" << temp_t.value << "'.";
                     error(lex);
				 }
                 
                 if(needsWord)
                 {
                    cglobals.out << "Expected symbol before '" << temp_t.value << "'.";
                    error(lex);
                 }
        
                 mapsz++;
                 tmplist.add( "::" );
                 needsWord = true;
              }
              else if(breakers.contains(temp_t.value))
              {
				 if(dotfirst)
				 {
					 cglobals.out << "Expected '.' before symbol '" << temp_t.value << "'.";
                     error(lex);
			     }
			     
                 if(needsWord)
                 {
                    cglobals.out << "Expected symbol before '" << temp_t.value << "'.";
                    error(lex);
                 }
                 
                mapsz++;
                break;
              }
              else {
				 if(dotfirst)
				 {
					 cglobals.out << "Expected '.' before symbol '" << temp_t.value << "'.";
                     error(lex);
			     }
			     
				 if(breakers.size() ==0)
				 {
					 if(needsWord)
					 {
						cglobals.out << "Expected symbol before '" << temp_t.value << "'.";
						error(lex);
					 }
					 
					mapsz++;
					lex.lexer().token_sz1--;
					break;
			     }
                 needsWord = true;
                 cglobals.out << "Unexpected symbol '" << temp_t.value << "', expecting '.' or ';'.";
                 error(lex);
                 err_flag++;
                 if(err_flag==2) break;
              }
          }
          
          wmap.j = mapsz;
          wmap.wlist = new ListAdapter<string>[wmap.j];
          int mpos = 0;
          for(int i = 0; i < tmplist.size(); i++)
          {
             if(tmplist.valueAt(i) == "::")
             { mpos++; }
             else wmap.wlist[mpos].add(tmplist.valueAt(i));
          }
          
          return wmap;
       }
       
       objmap parse_wordmap(lexr::parser_helper &lex, wordmap map, ListAdapter<Object> args)
       {
           objmap omap;
           stringstream symbol;
           omap.adr = -1;
           omap.internal_adr = 0;
           omap.type = typedef_unknown;
           
           if(map.j == 1)
           {
               if(map.wlist->size() == 1)
               {
                   symbol << map.wlist->valueAt(0);
                   if(args.size()==0 && objecthelper::find(map.wlist->valueAt(0), typedef_unknown, "<null>", 
                         memoryhelper::getparentclass()))
                   {
                       omap.adr = objecthelper::address(map.wlist->valueAt(0), typedef_unknown, "<null>", 
                                     memoryhelper::getparentclass(), false);
                        cout << "address @ " << omap.adr << endl;
                       omap.internal_adr = objecthelper::address(map.wlist->valueAt(0), typedef_unknown, "<null>", 
                                     memoryhelper::getparentclass(), true);
                       omap.type = objects.valueAt(objecthelper::address(map.wlist->valueAt(0), typedef_unknown, "<null>", 
                                     memoryhelper::getparentclass())).type;
                   }
                   else if(methodhelper::find(map.wlist->valueAt(0), "<null>", 
                         memoryhelper::getparentclass(), args))
                   {
                       omap.adr = methodhelper::address(map.wlist->valueAt(0), "<null>", 
                                     memoryhelper::getparentclass(), args);
                       omap.type = typedef_function;
                   }
                   else
                   {
                       cglobals.out << "Cannot resolve symbol '" << symbol.str() << "'.";
                       error(lex);
                   }
               }
           }
           
           omap.symbol = symbol.str();
		   return omap;
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
       
       void parse_return_decliration(lexr::parser_helper& lex, long addr)
       {
           lexr::token temp_t;
           bool plus=false, needStr=false;
           stringstream ss;
           for( ;; )
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
               {
			      cglobals.out << "Unexpected end of file.";
			      error(lex);
                  break;
			   }
               if(temp_t.type == temp_t.e_string){
                   plus = false;
                   ss << temp_t.value;
                   needStr = false;
               }
               else if(temp_t.type == temp_t.e_number){
                   if(needStr)
                   {
                       cglobals.out << "Expected string literal or qulafied symbol after previous numeric literal.";
                       error(lex);
                   }
                   
                   needStr = true;
                   plus = false;
                   ss << temp_t.value;
               }
               else if(temp_t.type == temp_t.e_symbol){
                   plus = false;
                   ss << "\\[v" << temp_t.value << ";";
                   needStr = false;
               }
               else if(temp_t.value == "+"){
                   if(plus)
                   {
                       cglobals.out << "Expected string literal or qulafied symbol after previous '+'.";
                       error(lex);
                   }
                   plus = true;
               }
               else if(temp_t.value == ";" || reserved(temp_t.value)){
                   if(reserved(temp_t.value)){
                     lex.lexer().token_sz1--;
                       cglobals.out << "Expected ';' before symbol '" << temp_t.value << "'.";
                       error(lex);
                   }
                     
                   if(plus)
                   {
                       cglobals.out << "Expected string literal or qulafied symbol after previous '+'.";
                       error(lex);
                   }
                   
                   level3(OP_ICONST, var_return, atof(ss.str().c_str()));
                   level2(OP_PUSH, var_return);
                   level2(OP_RETURN, addr);
                   break;
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(lex);
               }
           }
       }
       
       char parse_type_cast(lexr::parser_helper& lex)
       {
           lexr::token temp_t;
           temp_t = getNextToken(lex);
           char type = 'v';
           
           if(temp_t.type == temp_t.e_symbol)
           {
               if(!_typedef(temp_t.value))
               {
                   cglobals.out << "Expected qualified symbol before '" << temp_t.value << "'.";
                   error(lex);
               }
               
               if(temp_t.value == "string")
               {
                   cglobals.out << "Cannot apply explicit typecast of type `string`.";
                   error(lex);                    
               }
               else if(temp_t.value == "char")
                   type = 'c';
               else if(temp_t.value == "byte")
                   type = 'B';
               else if(temp_t.value == "int")
                   type = 'i';
               else if(temp_t.value == "short")
                   type = 's';
               else if(temp_t.value == "long")
                   type = 'l';
               else if(temp_t.value == "bool")
                   type = 'b';
               else if(temp_t.value == "float")
                   type = 'f';
               else if(temp_t.value == "double")
                   type = 'd';
               else if(temp_t.value == "bit")
                   type = 'I';
           }
           
           temp_t = getNextToken(lex);
           if(temp_t.value != ")")
           {
               cglobals.out << "Expected ')' before symbol '" << temp_t.value << "'.";
               error(lex);
           }
           
           return type;
       }
       
       void parse_cout_decliration(lexr::parser_helper& lex)
       {
           lexr::token temp_t;
           int bracket_stack = 1, errcount=0;
           stringstream outstream;
           outstream << "";
           
           for( ;; )
           {
               temp_t = getNextToken(lex);
               
               if(cglobals.eof)
               {
			      cglobals.out << "Unexpected end of file.";
			      error(lex);
                  break;
			   }
               if(temp_t.value == ")")
               {
                 bracket_stack--;
                 if(bracket_stack<=0)
                    break;
               }
               else if(temp_t.value == "(")
                 bracket_stack++;
               else if(temp_t.type == temp_t.e_string)
                 outstream << temp_t.value;
               else if(temp_t.value == "<<")
               {
                   temp_t = getNextToken(lex);
                   char cast = 'v';
                   
                   if(temp_t.type == temp_t.e_string){
                      outstream << temp_t.value;
                      continue;
                   }
                   else if(temp_t.type == temp_t.e_number)
                   {
                       outstream << atof(temp_t.value.c_str());
                       continue;
                   }
                      
                   if(temp_t.value == "(")
                   {
					   temp_t = getNextToken(lex);
					   if(temp_t.value == "*")
					   {
                          cast = 'p';
                          bracket_stack++;
					   }
					   else
					   {
						   lex.lexer().token_sz1--;   
						   cast = parse_type_cast(lex);
						   temp_t = getNextToken(lex);
						   
						   if(temp_t.type == temp_t.e_number) // apply type cast to number
						   {
							   level3(OP_DCONST, var_dummy_data, atof(temp_t.value.c_str()));
							   outstream << "\\[" << cast << var_dummy_data << ";";
							   continue;
						   }
						   else lex.lexer().token_sz1--;
					   }
                   }
                   else if(temp_t.value == "*")
                          cast = 'p';
                   else lex.lexer().token_sz1--;

				   ListAdapter<std::string> breakers;
				   ListAdapter<Object> args;
				   args._init_();
				   breakers._init_();
				   
                   objmap map = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers), args);
                   
                   if(map.adr == -1)
                   {
					   cglobals.out << "Expected qualified symbol before '" << temp_t.value << "'.";
					   error(lex);   
			       }
			       else 
			       {
					  if(map.type == typedef_function)
					  {
						   cglobals.out << "Cannot explicitly output function '" << map.symbol << "'.";
						   error(lex);
					  }  
				   }
                   outstream << "\\[" << cast << map.adr << ";";
               }
               else if(temp_t.type == temp_t.e_symbol)
               {
                   cglobals.out << "Expected '<<' prefix before symbol '" << temp_t.value << "'.";
                   error(lex);
               }
               else // could be another instruction?
               {
                   cglobals.out << "Expected '<<', ')', '(', or string literal before symbol '" << temp_t.value << "'.";
                   error(lex);
                   
                   errcount++;
                   if(errcount>1)
                   {
                       if(!(bracket_stack<=0))
                       {
                           cglobals.out << "Expected ')' at end of cout instruction.";
                           error(lex);
                       }
                       break;
                   }
               }
           }
           
          // cout << "outstream.str() " << outstream.str() << endl;
           _cout(outstream.str());
       }
       
       int castvalue(lexr::parser_helper &lex, string cast)
       {
           int i = 0;
           if(cast == "string")
           {
               cglobals.out << "Cannot apply explicit typecast of type `string`.";
               error(lex);                    
           }
           else if(cast == "char")
               return 1;
           else if(cast == "byte")
               return 2;
           else if(cast == "int")
               return 3;
           else if(cast == "short")
               return 4;
           else if(cast == "long")
               return 5;
           else if(cast == "bool" || cast == "bit")
               return 6;
           else if(cast == "float")
               return 7;
           else if(cast == "double")
               return 8;
           else 
           {
               cglobals.out << "Symbol `" << cast << "` does not name a type.";
               error(lex);  
           }
           return i;
       }
       
       void parse_asm_method_args(lexr::parser_helper& lex, ListAdapter<Object> &args)
       {
           lexr::token temp_t;
           bool comma = false;
           args._init_();
           
           for( ;; )
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
               {
				   cglobals.out << "Unexpected end of file.";
                   error(lex);
                   break;
			   }
               if(temp_t.value == "]" || _asm_strtop(temp_t.value) != -33)
               {
                   if(temp_t.value != "]")
                   {
                       cglobals.out << "Expected ']' at end of function decliration before '" << temp_t.value << "'.";
                       error(lex);
                   }
                   
                   if(comma)
                   {
                       cglobals.out << "Expected qualified symbol before ']'.";
                       error(lex);
                   }
                   if(_asm_strtop(temp_t.value) != -33) lex.lexer().token_sz1--;
                   break;
               }
               else if(temp_t.type == temp_t.e_symbol)
               {
                   Object o;
                   comma = false;
                   if(!_typedef(temp_t.value))
                   {
                       cglobals.out << "Expected qualified symbol before '" << temp_t.value << "'.";
                       error(lex);
                   }
                   
                   o.type = ttoint(temp_t.value);
                   
                   temp_t = getNextToken(lex);
                   if(temp_t.value == "[")
                   {
                       temp_t = getNextToken(lex);
                       if(temp_t.type == temp_t.e_number)
                       {
                           cglobals.out << "Cannot have array size in function argument definition '[" << temp_t.value << "]'.";
                           error(lex);
                           temp_t = getNextToken(lex);
                       }
                       
                       if(temp_t.value != "]")
                       { 
						   cglobals.out << "Expected ']' before symbol '" << temp_t.value << "'.";
                           error(lex);   
					   }
                       o.isarray = true;
                   }
                   else
                   {
                       lex.lexer().token_sz1--;
                       o.isarray = false;
                   }
                  args.add(o);   
               }
               else if(temp_t.value == ",")
               {
                   if(comma)
                   {
                       cglobals.out << "Expected qulafied symbol before '" << temp_t.value << "'.";
                       error(lex);
                   }
                   
                   comma = true;
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(lex);
               }
           }
       }
       
       void parse_asm_decliration(long begin, string _asm)
       {
           lexr::token temp_t;
        
           lexr::parser_helper lex;
           lex.init(_asm);
           lex.lexer().line_t = begin;
           
           cout << "asm " << _asm << endl;           
           for( ;; )
           {
               temp_t = getNextToken(lex);
               
               if(cglobals.eof)
                  break;
               if(temp_t.value == "cout")
               {
                   temp_t = getNextToken(lex);
                   
                   if(temp_t.value == "(")
                      parse_cout_decliration(lex);
                   else if(temp_t.type == temp_t.e_string)
                      _cout(temp_t.value);
                   else
                   {
                       cglobals.out << "Expected '(' or string literal before symbol '" << temp_t.value << "'.";
                       error(lex);
                   }
               }
               else if(temp_t.value == "hlt" || temp_t.value == "end" || temp_t.value == "nop" 
                   || temp_t.value == "no" || temp_t.value == "endno")
               {
                   level1(_asm_strtop(temp_t.value));
               }     
               else if(temp_t.value == "iconst" || temp_t.value == "cconst" || temp_t.value == "fconst" || temp_t.value == "dconst"
                     || temp_t.value == "sconst" || temp_t.value == "lconst" || temp_t.value == "bconst" || temp_t.value == "byte_const"
                     || temp_t.value == "str_const" || temp_t.value == "bit_const")
               {
                   string type = temp_t.value;
                   cglobals.ignorestrays = true;
				   temp_t = getNextToken(lex);
                   cglobals.ignorestrays = false;
                   
				   if(temp_t.value != "@")
				   {
					   cglobals.out << "Expected '@' prefix before symbol '" << temp_t.value << "'.";
					   error(lex);
				   }
				   else temp_t = getNextToken(lex);  // possibly some other prefix?
				   
				   if(temp_t.type != temp_t.e_symbol)
				   {
					   cglobals.out << "Expected unqulafied symbol before symbol '" << temp_t.value << "'.";
					   error(lex);
				   }
				   
				   string varname = temp_t.value,buf="";
				   if(reserved(varname) || _asm_strtop(varname) != -33)
				   {
					   cglobals.out << "Symbol '" << varname << "' cannot be named after a builtin symbol.";
					   error(lex);
				   }
				   
				   objecthelper::create(varname, _asm_strttot(type), false, false, access_public, 
                        cglobals.package, "<null>", getparentclass());
                   
                   cglobals.ignorestrays = true;
                   temp_t = getNextToken(lex);
                   cglobals.ignorestrays = false;
                   bool charr = false, str = false;
                   
                   if(temp_t.value != "#")
				   {
					   if(type == "cconst" && temp_t.type == temp_t.e_string)
					   { charr = true; }
					   else if(type == "str_const" && temp_t.type == temp_t.e_string)
					   { str = true; }
					   else
					   {
						   cglobals.out << "Expected '#' prefix before symbol '" << temp_t.value << "'.";
						   error(lex); 
					   }
				   }
				   else temp_t = getNextToken(lex);
				   
				   if(temp_t.type != temp_t.e_number)
				   {
					   if(temp_t.type == temp_t.e_string && type == "cconst" && charr)
					   {
						   if(temp_t.value.size() == 0)
						   {
							    cglobals.out << "Char literal is empty.";
							    error(lex);
						   }
						   if(temp_t.value.size() == 1)
						   {
						      stringstream ss;
						      ss << (int) temp_t.value.at(0);
						      temp_t.value = ss.str();
						   }
						   else if(temp_t.value.size() == 2)
						   {
							   char prefix = temp_t.value.at(0), tok = temp_t.value.at(1);
							   if(prefix != '\\')
							   {
								   cglobals.out << "Expected '\\' prefix before '" << prefix << "' in char literal.";
								   error(lex);
							   }
							   
							   if(tok != 'n' && tok != 'v' && tok != '\\' && tok != 't'
							      && tok != 'b' && tok != 'f' && tok != 'r' && tok != '"'
							      && tok != '\'')
							   {
								   cglobals.out << "Symbol '" << tok << "' in char literal is not an escape sequence.";
								   warning(lex);
							   }
							   stringstream ss;
							   ss << getesc(tok);
							   temp_t.value = ss.str();
						   }
						   else
						   {
							   cglobals.out << "Symbol '" << temp_t.value << "' is not a char literal.";
							   error(lex);
						   }
					   }
					   else if(temp_t.type == temp_t.e_string && type == "str_const" && str)
						   buf = temp_t.value;
					   else 
					   {
						   cglobals.out << "Expected " << ((type == "cconst" || type == "str_const") ? "string" : "numeric") 
						          << " literal before symbol '" << temp_t.value << "'.";
						   error(lex);
					   }
				   }
				   
				   if(type == "str_const")
				   {
				      _string(objecthelper::address(varname, _asm_strttot(type),
				           "<null>", memoryhelper::getparentclass(), false), buf);
				   }
				   else
				   {
				      level3(ttop(_asm_strttot(type)), objecthelper::address(varname, _asm_strttot(type),
				             "<null>", memoryhelper::getparentclass(), false), atof(temp_t.value.c_str()));
				   }
			   }
               else if(temp_t.value == "push" || temp_t.value == "pop" || temp_t.value == "kill" || temp_t.value == "delete"
                      || temp_t.value == "inc" || temp_t.value == "dec" || temp_t.value == "if" || temp_t.value == "jmp"
                      || temp_t.value == "neg" || temp_t.value == "cin")
               {
				   ListAdapter<std::string> breakers;
				   ListAdapter <Object> args;
				   args._init_();
				   breakers._init_();
				   
                   objmap map = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers, true), args);
                   
                   if(map.adr == -1)
                   {
					   cglobals.out << "Expected qualified symbol after '" << temp_t.value << "'.";
					   error(lex);   
			       }
			       
			       level2(_asm_strtop(temp_t.value), map.adr);
			   }
			   else if(temp_t.value == "call" || temp_t.value == "ret")
			   {
			       string op = temp_t.value;
			       ListAdapter<std::string> breakers;
			       breakers._init_();
			       
                   objmap map;
                   wordmap wmap = parse_complex_dot_notation(lex, false, breakers, true);
			       
			      temp_t = getNextToken(lex);
			      if(temp_t.value != "[")
			      {
    				   cglobals.out << "Expected '[' before symbol '" << map.symbol << "'.";
    				   error(lex); 
			      }
			      
			      ListAdapter<Object> args;
			      parse_asm_method_args(lex, args);
			      
			      map = parse_wordmap(lex, wmap, args);
			      
			      if(map.adr == -1)
                   {
					   cglobals.out << "Expected qualified symbol after '" << temp_t.value << "'.";
					   error(lex);   
			       }
			       
			      if(op == "ret" && !cglobals.nativeReturnLock) // this prevents from mis identifying wether or not you are in a function
			      {
			          cglobals.infunction--;
			          cglobals.nativeReturnLock = true;
			      }
			       
				  if(map.type != typedef_function && map.adr != -1)
				  {
					   cglobals.out << "Cannot explicitly " << ((op == "call") ? "call" : "return" ) << " non-function symbol '" << map.symbol << "'.";
					   error(lex);
				  }
				  if(op == "call")
				    level3(OP_CALL, map.adr, lex.lexer().line_t);
				  else
			        level2(OP_RETURN, map.adr);
			   } 
               else if(temp_t.value == "%")
               {
                  temp_t = getNextToken(lex);
                  if(temp_t.value != "%")
                  {
                      cglobals.out << "Expected '%' prefix before symbol '" << temp_t.value << "'.";
					  error(lex);
                  }
                  else temp_t = getNextToken(lex);
                  if(temp_t.value == "label")
				  {
					   string type = temp_t.value;
					   cglobals.ignorestrays = true;
					   temp_t = getNextToken(lex);
					   cglobals.ignorestrays = false;
					   
					   if(temp_t.value != "@")
					   {
						   cglobals.out << "Expected '@' prefix before symbol '" << temp_t.value << "'.";
						   error(lex);
					   }
					   else temp_t = getNextToken(lex);  // possibly some other prefix?
					   
					   if(temp_t.type != temp_t.e_symbol)
					   {
						   cglobals.out << "Expected unqulafied symbol before symbol '" << temp_t.value << "'.";
						   error(lex);
					   }
					   
					   string varname = temp_t.value;
					   if(reserved(varname) || _asm_strtop(varname) != -33)
					   {
						   cglobals.out << "Symbol '" << varname << "' cannot be named after a builtin symbol.";
						   error(lex);
					   }
					   
					   if(!objecthelper::create(varname, typedef_label, false, false, access_public, 
							cglobals.package, "<null>", memoryhelper::getparentclass()))
				       {
                           cglobals.out << "Symbol '" << varname << "' has already been declared.\nPreviously declared here:\n\t" 
                                           << memoryhelper::objecthelper::getobjinfo(varname, typedef_label, "<null>", memoryhelper::getparentclass());
                           error(cglobals.lex);
					   }
					   
					   if(cglobals.infunction == 0)
			           {
			               cglobals.out << "Label '" << varname << "' cannot created outside a function.";
						   error(lex);
			           }
					   
                     level2(OP_LBL, objecthelper::address(varname, typedef_label, "<null>", 
                              memoryhelper::getparentclass(), false));
				  }				      
			      else if(temp_t.value == "mthd")
			      {
			           cglobals.nativeReturnLock = false;
                       cglobals.method_t++;
					   string type = temp_t.value;
					   cglobals.ignorestrays = true;
					   temp_t = getNextToken(lex);
					   cglobals.ignorestrays = false;
					   if(temp_t.value != "@")
					   {
						   cglobals.out << "Expected '@' prefix before symbol '" << temp_t.value << "'.";
						   error(lex);
					   }
					   else temp_t = getNextToken(lex);  // possibly some other prefix?
					   
					   if(temp_t.type != temp_t.e_symbol)
					   {
						   cglobals.out << "Expected unqulafied symbol before symbol '" << temp_t.value << "'.";
						   error(lex);
					   }
					   
					   string functionname = temp_t.value;
					   temp_t = getNextToken(lex);
					   if(temp_t.value != "[")
					   {
						   cglobals.out << "Expected '[' before symbol '" << functionname << "'.";
						   error(lex); 
					   }
			      
				       ListAdapter<Object> args;
					   args._init_();
					   parse_asm_method_args(lex, args);
					   bool methodfound;
					   
					   //cout << "parentclass " << getparentclass() << endl;
					   
					   if(!memoryhelper::methodhelper::create(functionname, false, access_private, 
								cglobals.package, "<null>", getparentclass(), args, true))
					   {
						   methodfound = true;
						   cglobals.out << "Symbol of type `function` has already been declared.\nPreviously declared here:\n\t" 
								<< memoryhelper::methodhelper::getmethodinfo(functionname, "<null>", getparentclass(), args);
						   error(lex);    
					   }
					   
				      Object arg1;
					  arg1.type = typedef_string;
					  arg1.isarray = true;
					 
					  ListAdapter<Object> initargs;
					  initargs.add(arg1);
 
					   if(!methodfound && !cglobals.hasInit && functionname == "__init__" && 
					      memoryhelper::methodhelper::sameargs(initargs, args) && getparentclass() == "Starter")
					   {
						   cglobals.out << "Entry point function:\n\t" 
								<< memoryhelper::methodhelper::getmethodinfo(functionname, "<null>", getparentclass(), args)
								  << "\nCannot be declared as a native Scorpion assembly function.";
						   error(lex); 
					   }
					   
					   temp_t = getNextToken(lex);
					   if(temp_t.value != ":"){
						   cglobals.out << "Expected ':' before '" << temp_t.value << "'.";
						   error(lex);
					   }
					  
					  long madr = memoryhelper::methodhelper::address
									 (functionname, "<null>", getparentclass(), args);
					  
					   if(cglobals.infunction != 0)
			           {
			               cglobals.out << "Native function '" 
			                 << methodhelper::basic_minfo(methods.valueAt(madr), args) 
			                   << "' cannot be nested inside of another function.";
						   error(lex);
			           }
			           else
			           {
			               cglobals.infunction = 1;
			               cglobals.functioninfo = methodhelper::basic_minfo(methods.valueAt(madr), args);
			           }
					  				 
			          cglobals.infunction++;
					  level2(OP_MTHD, madr);  
				  }
				  else if(temp_t.value == "ptr" || temp_t.value == "movp")
				  {
				      string op = temp_t.value;
    				  long obj1,obj2;
    				  ListAdapter<std::string> breakers;
    				  ListAdapter <Object> args;
    				  args._init_();
    				  breakers._init_();
    				   
                      objmap map = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers, true), args);
                      obj1=map.adr;
                      
                      if(map.adr == -1)
                      {
    					  cglobals.out << "Expected qualified symbol after '" << temp_t.value << "'.";
    					  error(lex);   
    			      }
    			       
    			      temp_t = getNextToken(lex);
    			      if(temp_t.value != ",")
    			      {
    					  cglobals.out << "Expected ',' before symbol '" << temp_t.value << "'.";
    					  error(lex);   
    				  }
    			       
    			      objmap map2 = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers, true), args);
                      obj2=map2.adr;
                       
                      if(map2.adr == -1)
                      {
    					  cglobals.out << "Expected qualified symbol after '" << op << "'.";
    					  error(lex);   
    			      }
    			      
    			      if(op == "movp")
    			      {
    			          if(objects.valueAt(map.internal_adr).type !=
    			             objects.valueAt(map2.internal_adr).type)
    			          {
        					  cglobals.out << "Cannot assign pointers of different types.";
        					  error(lex);   
    			          }
    			          
    			          objects.valueAt(map.internal_adr).eadr.byte1 =
    			             objects.valueAt(map2.internal_adr).eadr.byte1;
    			      }
    			      else if(op == "ptr")
    			      {
    			          level3(OP_ICONST, map.adr, objects.valueAt(map2.internal_adr).eadr.byte1);
    			      }
    			       
				  }
				  else 
				  {
				      cglobals.out << "Unknown special instruction '" << temp_t.value << "'.";
					  error(lex); 
				  }
			   }
               else if(temp_t.value == "jit" || temp_t.value == "jif" || temp_t.value == "lsft" || temp_t.value == "rsft"
                      || temp_t.value == "str_apnd" || temp_t.value == "throw" || temp_t.value == "assn"
                      || temp_t.value == "str_x2i" || temp_t.value == "str_x2f")
               {
				   string op = temp_t.value;
				   long obj1,obj2;
				   ListAdapter<std::string> breakers;
				   ListAdapter <Object> args;
				   args._init_();
				   breakers._init_();
				   
                   objmap map = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers, true), args);
                   obj1=map.adr;
                   
                   if(map.adr == -1)
                   {
					   cglobals.out << "Expected qualified symbol after '" << temp_t.value << "'.";
					   error(lex);   
			       }
			       
			       temp_t = getNextToken(lex);
			       if(temp_t.value != ",")
			       {
					   cglobals.out << "Expected ',' before symbol '" << temp_t.value << "'.";
					   error(lex);   
				   }
			       
			       objmap map2 = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers, true), args);
                   obj2=map2.adr;
                   
                   if(map2.adr == -1)
                   {
					   cglobals.out << "Expected qualified symbol after '" << op << "'.";
					   error(lex);   
			       }
			       
			       level3(_asm_strtop(op), obj1, obj2);
			   }
               else if(temp_t.value == "ilt" || temp_t.value == "igt" || temp_t.value == "ile"
                       || temp_t.value == "ige" || temp_t.value == "ieq" || temp_t.value == "ilt"
                       || temp_t.value == "add" || temp_t.value == "sub" || temp_t.value == "mult"
                       || temp_t.value == "div" || temp_t.value == "mod" || temp_t.value == "at"
                       || temp_t.value == "aload" || temp_t.value == "astore" || temp_t.value == "cast")
               {
				   string op = temp_t.value;
				   long obj1,obj2,obj3;
				   ListAdapter<std::string> breakers;
				   ListAdapter <Object> args;
				   args._init_();
				   breakers._init_();
				   
                   objmap map = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers, true), args);
                   obj1=map.adr;
                   
                   if(map.adr == -1)
                   {
					   cglobals.out << "Expected qualified symbol after '" << temp_t.value << "'.";
					   error(lex);   
			       }
			       
			       temp_t = getNextToken(lex);
			       if(temp_t.value != ",")
			       {
					   cglobals.out << "Expected ',' before symbol '" << temp_t.value << "'.";
					   error(lex);   
				   }
			       
			       objmap map2 = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers, true), args);
                   obj2=map2.adr;
                   
                   if(map2.adr == -1)
                   {
					   cglobals.out << "Expected qualified symbol after '" << op << "'.";
					   error(lex);   
			       }
			       
			       temp_t = getNextToken(lex);
			       if(temp_t.value != ",")
			       {
					   cglobals.out << "Expected ',' before symbol '" << temp_t.value << "'.";
					   error(lex);   
				   }
			       
			       if(op == "cast") // process differently
			       {
			           temp_t = getNextToken(lex);
			           if(temp_t.value != "[")
    			       {
    					   cglobals.out << "Expected '[' before symbol '" << temp_t.value << "'.";
    					   error(lex);   
    				   }
    				   
    				   string type = "int";
			           temp_t = getNextToken(lex);
			           if(temp_t.type != temp_t.e_symbol && temp_t.value != "*")
			           {
    					   cglobals.out << "Expected qualified symbol before '" << temp_t.value << "'.";
    					   error(lex);   
			           }
			           else type = temp_t.value;
			           
			           temp_t = getNextToken(lex);
			           if(temp_t.value != "]")
    			       {
    					   cglobals.out << "Expected ']' before symbol '" << temp_t.value << "'.";
    					   error(lex);   
    				   }
    				   
			           level4(OP_CAST, obj1, obj2, castvalue(lex, type));
			           continue;
			       }
			       
			       objmap map3 = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers, true), args);
                   obj3=map3.adr;
                   
                   if(map3.adr == -1)
                   {
					   cglobals.out << "Expected qualified symbol after '" << op << "'.";
					   error(lex);   
			       }
			       
			       level4(_asm_strtop(op), obj1, obj2, obj3);
			   }
			   else if(temp_t.value == "i_aconst" || temp_t.value == "b_aconst" || temp_t.value == "f_aconst"
			       || temp_t.value == "c_aconst" || temp_t.value == "s_aconst" || temp_t.value == "l_aconst"
			       || temp_t.value == "byte_aconst" || temp_t.value == "str_aconst" || temp_t.value == "bit_aconst")
			       {
			           string type = temp_t.value;
			           long obj1, obj2;
                       cglobals.ignorestrays = true;
    				   temp_t = getNextToken(lex);
                       cglobals.ignorestrays = false;
                       
    				   if(temp_t.value != "@")
    				   {
    					   cglobals.out << "Expected '@' prefix before symbol '" << temp_t.value << "'.";
    					   error(lex);
    				   }
    				   else temp_t = getNextToken(lex);  // possibly some other prefix?
    				   
    				   if(temp_t.type != temp_t.e_symbol)
    				   {
    					   cglobals.out << "Expected unqulafied symbol before symbol '" << temp_t.value << "'.";
    					   error(lex);
    				   }
    				   
    				   string varname = temp_t.value;
    				   if(reserved(varname) || _asm_strtop(varname) != -33)
    				   {
    					   cglobals.out << "Symbol '" << varname << "' cannot be named after a builtin symbol.";
    					   error(lex);
    				   }
    				   
    				   objecthelper::create(varname, _asm_strttot(type), false, true, access_public, 
                            cglobals.package, "<null>", getparentclass());
    				   
    				   ListAdapter<std::string> breakers;
    				   ListAdapter<Object> args;
    				   args._init_();
    				   breakers._init_();
    				   
    				   objmap map1 = parse_wordmap(lex, parse_complex_dot_notation(lex, false, breakers, true), args);
                       obj2=map1.adr;
                       
                       if(map1.adr == -1)
                       {
    					   cglobals.out << "Expected qualified symbol after '" << temp_t.value << "'.";
    					   error(lex);   
    			       }
    				   obj1 = objecthelper::address(varname, _asm_strttot(type),
    				           "<null>", memoryhelper::getparentclass(), false);
    				   
    				   level3(tatop(_asm_strttot(type)), obj1, obj2);
			       }
               else {
                   cglobals.out << "Expected assembly instruction before '" << temp_t.value << "'.";
                   error(lex);
               }
           }
           
           cglobals.eof=0;
       }
       
       void parse_method_args(lexr::parser_helper& lex, ListAdapter<Object> &args)
       {
           lexr::token temp_t;
           bool comma = false;
           
           for( ;; )
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
               { 
			      cglobals.out << "Unexpected end of file.";
			      error(lex);
                  break;
			   }
               if(temp_t.value == ")" || (reserved(temp_t.value) && !_typedef(temp_t.value)) 
                  || temp_t.value == "{")
               {
                   if(temp_t.value != ")")
                   {
                       cglobals.out << "Expected ')' at end of function decliration before '" << temp_t.value << "'.";
                       error(lex);
                   }
                   
                   if(comma)
                   {
                       cglobals.out << "Expected qualified symbol before ')'.";
                       error(lex);
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
                       error(lex);
                   }
                   
                   o.type = ttoint(temp_t.value);
                   
                   temp_t = getNextToken(lex);
                   if(temp_t.value == "[")
                   {
                       temp_t = getNextToken(lex);
                       if(temp_t.type == temp_t.e_number)
                       {
                           cglobals.out << "Cannot have array size in function argument definition '[" << temp_t.value << "]'.";
                           error(lex);
                           temp_t = getNextToken(lex);
                       }
                       
                       if(temp_t.value == "]")
                       { temp_t = getNextToken(lex); }
                       o.isarray = true;
                   }
                   else
                   {
                       o.isarray = false;
                   }
                   
                   if(temp_t.type != temp_t.e_symbol)
                   {
                       cglobals.out << "Expected unqualified symbol before '" << temp_t.value << "'.";
                       error(lex);
                   }
                   o.name = temp_t.value;
                  args.add(o);   
               }
               else if(temp_t.value == ",")
               {
                   if(comma)
                   {
                       cglobals.out << "Expected qulafied symbol before '" << temp_t.value << "'.";
                       error(lex);
                   }
                   
                   comma = true;
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(lex);
               }
           }
       }
       
       void parse_method_block(lexr::parser_helper& lex, int block_begin, long address)
       {
           cglobals.block_stack++;
           cglobals.scopelevel++;
           lexr::token temp_t;
           
           while( cglobals.block_stack!=block_begin)
           {
               temp_t = getNextToken(lex);
               if(cglobals.eof)
               { 
			      cglobals.out << "Unexpected end of file.";
			      error(lex);
                  break;
			   }
               if(temp_t.value == "}")
               {
                   run_gc(); // kill all temp vars
	               cglobals.scopelevel--;
                   cglobals.block_stack--;
               }
               else if(temp_t.value == "{")
               {
                 cglobals.scopelevel++;
                 cglobals.block_stack++;
               }
               else if(reserved(temp_t.value))
               {
                   if(accessspecifier(temp_t.value))
                   {
                       object_attrib atribs;
                       atribs.access = strtoaccess(temp_t.value);
                       memoryhelper::helper::parse_access_specifier(lex, atribs, temp_t);
                       if(_typedef(temp_t.value))
                       {
                          //memoryhelper::helper::parse_typedef_block(lex, temp_t.value, atribs);
                       }
                   }
                   else if(temp_t.value == "return")
                           memoryhelper::helper::parse_return_decliration(lex, address);
                   else if(temp_t.value == "asm")
                   {
                      temp_t = getNextToken(lex);
                      if(temp_t.value != "(")
                      {
                          cglobals.out << "Expected '(' before '" << temp_t.value << "'.";
                          error(lex);
                      }
                      
                      temp_t = getNextToken(lex);
                      if(temp_t.type == temp_t.e_string)
                      {
						   asmintro = false;
                           memoryhelper::helper::parse_asm_decliration(lex.lexer().line_t, temp_t.value);
                           
                           for(long i = 0; i < temp_t.value.size(); i++)
                               if(temp_t.value.at(i) == '\n') lex.lexer().line_t++;
                           cout << "\n";
						   asmintro = true;
                      }
                      else {
                          cglobals.out << "Expected string literal before '" << temp_t.value << "'.";
                          error(lex);
                      }
                      
                      temp_t = getNextToken(lex);
                      if(temp_t.value != ")")
                      {
                          cglobals.out << "Expected ')' before '" << temp_t.value << "'.";
                          error(lex);
                      }
                      
                      temp_t = getNextToken(lex);
                      if(temp_t.value != ";")
                      {
                          cglobals.out << "Expected ';' at end of statement.";
                          error(lex);
                      }
                   }
                   else {
                       cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                       error(lex);
                   }
               }
               else if(temp_t.value == ";")
               { }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(lex);
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
               { 
			      cglobals.out << "Unexpected end of file.";
			      error(lex);
                  break;
			   }
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
                           if(temp_t.value == "static" || temp_t.value == "const") lex.lexer().token_sz1--;
                           
                           memoryhelper::helper::parse_access_specifier(lex, atribs, temp_t);
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
                               error(lex);
                           }   
                               
                           
                         //  cout << "def " << temp_t.value << endl;
                           string functionname = temp_t.value;
                           bool methodfound = false;
                           
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "("){
                               cglobals.out << "Expected '(' before '" << temp_t.value << "'.";
                               error(lex);
                           }
                           
                           ListAdapter<Object> args;
                           args._init_();
                           Object arg1;
                           memoryhelper::helper::parse_method_args(lex, args);

                           
                          // cout << "parentclass " << getparentclass() << endl;
                           
                           if(!memoryhelper::methodhelper::create(functionname, atribs.isStatic, atribs.access, 
                                    cglobals.package, "<null>", getparentclass(), args))
                           {
                               methodfound = true;
                               cglobals.out << "Symbol of type `function` has already been declared.\nPreviously declared here:\n\t" 
                                    << memoryhelper::methodhelper::getmethodinfo(functionname, "<null>", getparentclass(), args);
                               error(lex);    
                           }
                           
                          arg1.type = typedef_string;
                          arg1.isarray = true;
                          
                          ListAdapter<Object> initargs;
                          initargs.add(arg1);
     
                           if(!methodfound && !cglobals.hasInit && functionname == "__init__" 
                               && memoryhelper::methodhelper::sameargs(initargs, args) && getparentclass() == "Starter")
                           {
                               
                               if(!atribs.isStatic)
                               {
                                   cglobals.out << "Entry point method 'public static def __init__(string[] args)' must be static.";
                                   error(lex);
                               }
                               
                               if(atribs.access != access_public)
                               {
                                   cglobals.out << "Entry point method 'public static def __init__(string[] args)' must be public.";
                                   error(lex);
                               }
                               
                               cglobals.hasInit = true;
                           }
                           
                           if(atribs.isConst)
                           {
                               cglobals.out << "Method '" << memoryhelper::atostr(atribs.access) << " " 
                                              << ((atribs.isStatic) ? "static " : "") << ((atribs.isConst) ? "const " : "") 
                                                << "def " << functionname << "(" << memoryhelper::methodhelper::argstostr(args) << ")' cannot be const.";
                               error(lex);
                           }
                           
                           temp_t = getNextToken(lex);
                           if(temp_t.value != "{"){
                               cglobals.out << "Expected '{' before '" << temp_t.value << "'.";
                               error(lex);
                           }
                          
                          long madr = memoryhelper::methodhelper::address
                                         (functionname, "<null>", getparentclass(), args);
                          
                          cglobals.infunction = 1;
                          cglobals.scopelevel = 0;
                          cglobals.functioninfo = methodhelper::basic_minfo(methods.valueAt(madr), args);
                          
                          level2(OP_MTHD, madr); 
                          memoryhelper::helper::parse_method_block(lex, cglobals.block_stack, madr);
                          
                          cout<<"\n";
			              cglobals.infunction = 0;
                          
                          level3(OP_ICONST, var_return, 0); 
                          level2(OP_PUSH, var_return); 
                          level2(OP_RETURN, madr); 
                       }
                   }
                   else {
                       cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                       error(lex);
                   }
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(lex);
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
                      string name = memoryhelper::helper::parse_dot_notation(lex, false, "{");
                      cout << "namespace " << name << endl;
                      memoryhelper::helper::parse_namespace_block(lex, cglobals.block_stack);
                   }
                   else if(accessspecifier(temp_t.value) || temp_t.value == "static" || temp_t.value == "const")
                   {
                       object_attrib atribs;
                       atribs.access = ((temp_t.value == "static") ? access_public : strtoaccess(temp_t.value));
                       if(temp_t.value == "static") lex.lexer().token_sz1--;
                       
                       memoryhelper::helper::parse_access_specifier(lex, atribs, temp_t);
                       if(temp_t.value == "class"){
                           
                       }
                       else if(_typedef(temp_t.value)){}
                           //memoryhelper::helper::parse_typedef_block(lex, temp_t.value, atribs);
                       else if(temp_t.value == "def"){
                           
                       }
                       else {
                           cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                           error(lex);
                       }
                   }
                   else {
                       cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                       error(lex);
                   }
               }
               else {
                   cglobals.out << "Unexpected symbol '" << temp_t.value << "'.";
                   error(lex);
               }
           }
       }
       
       bool  reserved(std::string symbol)
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
            || symbol == "package" || symbol == "namespace" || symbol == "asm" || symbol == "bit");
       }
       
       bool _typedef(std::string symbol)
       {
           return (symbol == "char" || symbol == "byte" || symbol == "short"
                   || symbol == "int" || symbol == "long" || symbol == "double"
                   || symbol == "float" || symbol == "string" || symbol == "bool" 
                   || symbol == "bit");
       }
       
       int _strttot(std::string symbol)
       {
            if(symbol == "char") return typedef_char;
            if(symbol == "byte") return typedef_byte;
            if(symbol == "short") return typedef_short;
            if(symbol == "int") return typedef_int;
            if(symbol == "long") return typedef_long;
            if(symbol == "double") return typedef_double;
            if(symbol == "float") return typedef_float;
            if(symbol == "string") return typedef_string;
            if(symbol == "bool") return typedef_boolean;
            if(symbol == "bit") return typedef_bit;
            else return -33;
       }
       
       int _asm_strttot(std::string symbol)
       {
            if(symbol == "cconst" || symbol == "c_aconst") return typedef_char;
            if(symbol == "bconst" || symbol == "b_aconst") return typedef_boolean;
            if(symbol == "sconst" || symbol == "s_aconst") return typedef_short;
            if(symbol == "iconst" || symbol == "i_aconst") return typedef_int;
            if(symbol == "lconst" || symbol == "l_aconst") return typedef_long;
            if(symbol == "dconst" || symbol == "d_aconst") return typedef_double;
            if(symbol == "fconst" || symbol == "f_aconst") return typedef_float;
            if(symbol == "str_const" || symbol == "str_aconst") return typedef_string;
            if(symbol == "byte_const" || symbol == "byte_aconst") return typedef_byte;
            if(symbol == "bit_const" || symbol == "bit_aconst") return typedef_bit;
            else return -33;
       }
      
       int ttop(int type)
       {
            if(type == typedef_char) return OP_CCONST;
            if(type == typedef_byte) return OP_BYTE_CONST;
            if(type == typedef_short) return OP_SCONST;
            if(type == typedef_int) return OP_ICONST;
            if(type == typedef_long) return OP_LCONST;
            if(type == typedef_double) return OP_DCONST;
            if(type == typedef_float) return OP_FCONST;
            if(type == typedef_string) return OP_STRCONST;
            if(type == typedef_boolean) return OP_BCONST;
            if(type == typedef_bit) return OP_BIT_CONST;
            else return -33;
       }
       
       int tatop(int type)
       {
            if(type == typedef_char) return OP_CACONST;
            if(type == typedef_byte) return OP_BYTE_ACONST;
            if(type == typedef_short) return OP_SACONST;
            if(type == typedef_int) return OP_IACONST;
            if(type == typedef_long) return OP_LACONST;
            if(type == typedef_double) return OP_DACONST;
            if(type == typedef_float) return OP_FACONST;
            if(type == typedef_string) return OP_STR_ACONST;
            if(type == typedef_boolean) return OP_BACONST;
            if(type == typedef_bit) return OP_BIT_ACONST;
            else return -33;
       }
      
       int _asm_strtop(string op)
       {
            if(op == "nop") return OP_NOP;
            if(op == "push") return OP_PUSH;
            if(op == "pop") return OP_POP;
            if(op == "jmp") return OP_JMP;
            if(op == "mthd") return OP_MTHD;
            if(op == "ret") return OP_RETURN;
            if(op == "end") return OP_END;
            if(op == "call") return OP_CALL;
            if(op == "ieq") return OP_ISEQ;
            if(op == "ilt") return OP_ISLT;
            if(op == "ile") return OP_ISLE;
            if(op == "igt") return OP_ISGT;
            if(op == "ige") return OP_ISGE;
            if(op == "iconst") return OP_ICONST;
            if(op == "bconst") return OP_BCONST;
            if(op == "cconst") return OP_CCONST;
            if(op == "fconst") return OP_FCONST;
            if(op == "dconst") return OP_DCONST;
            if(op == "sconst") return OP_SCONST;
            if(op == "lconst") return OP_LCONST;
            if(op == "byte_const") return OP_BYTE_CONST;
            if(op == "str_const") return OP_STRCONST;
            if(op == "add") return OP_ADD;
            if(op == "sub") return OP_SUB;
            if(op == "mult") return OP_MULT;
            if(op == "div") return OP_DIV;
            if(op == "mod") return OP_MOD;
            if(op == "lsft") return OP_LSHFT;
            if(op == "rsft") return OP_RSHFT;
            if(op == "cin") return OP_CIN;
            if(op == "cout") return OP_COUT;
            if(op == "hlt") return OP_HLT;
            if(op == "jif") return OP_JIF;
            if(op == "jit") return OP_JIT;
            if(op == "label") return OP_LBL;
            if(op == "no") return OP_NO;
            if(op == "endno") return OP_ENDNO;
            if(op == "if") return OP_IF;
            if(op == "or") return OP_OR;
            if(op == "inc") return OP_INC;
            if(op == "dec") return OP_DEC;
            if(op == "and") return OP_AND;
            if(op == "throw") return OP_THROW;
            if(op == "at") return OP_AT;
            if(op == "str_apnd") return OP_STR_APND;
            if(op == "kill") return OP_KILL;
            if(op == "delete") return OP_DELETE;
            if(op == "aload") return OP_ALOAD;
            if(op == "astore") return OP_ASTORE;
            if(op == "assn") return OP_ASSN;
            if(op == "i_aconst") return OP_IACONST;
            if(op == "str_aconst") return OP_STR_ACONST;
            if(op == "cast") return OP_CAST;
            if(op == "byte_aconst") return OP_BYTE_ACONST;
            if(op == "f_aconst") return OP_FACONST;
            if(op == "d_aconst") return OP_DACONST;
            if(op == "c_aconst") return OP_CACONST;
            if(op == "b_aconst") return OP_BACONST;
            if(op == "l_aconst") return OP_LACONST;
            if(op == "s_aconst") return OP_SACONST;
            if(op == "node") return OP_NODE;
            if(op == "neg") return OP_NEG;
            if(op == "str_x2i") return OP_STR_X2I;
            if(op == "str_x2f") return OP_STR_X2F;
            if(op == "bit_const") return OP_BIT_CONST;
            if(op == "bit_aconst") return OP_BIT_ACONST;
            return -33;
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

short dbl_maxp = 16;
void parse_cmplr_items(stringstream &out_buf)
 {
     int i2 = 0;
     out_buf << std::setprecision(dbl_maxp);
     for(unsigned long i =0; i<cplrfreelist2->c_items.size(); i++)
     {
         cplrfreelist1.add(cplrfreelist2->c_items.valueAt(i));
         unsigned long ins= cplrfreelist1.valueAt(0).item.byte1;
         cres.size_t.byte1++;
         cres.size_t.byte2++;
         
         if(--i2 <= 0)
         {
            i2 = rand_n(127); 
            out_buf<<endl;
         }
         
         if(ins == OP_SCONST || ins == OP_BCONST || ins == OP_CCONST
            || ins == OP_BIT_CONST || ins == OP_BIT_ACONST
            || ins == OP_ICONST || ins == OP_DCONST || ins == OP_FCONST
            || ins == OP_BYTE_CONST || ins == OP_LCONST ||ins == OP_DACONST 
            || ins == OP_IACONST || ins == OP_FACONST || ins == OP_CACONST 
            || ins == OP_BACONST || ins == OP_BYTE_ACONST || ins == OP_SACONST 
            || ins == OP_LACONST || ins == OP_ISEQ || ins == OP_ISLT || ins == OP_ISLE
            || ins == OP_ISGT || ins == OP_ISGE || ins == OP_OR || ins == OP_AND) 
               cres.size_t.byte1++;  // for preprocessor
         
         //cout << "ins " << ins << endl;
         if(ins == OP_MTHD){
                cres.size_t.byte1 += cplrfreelist1.valueAt(0).size_t.byte1;
                Method m= methods.valueAt(cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1);
                
                out_buf << (char) cplr_method << OP_MTHD << (char) 0 << ((m.isnative) ? "~" : "") << m.name << "&" << m.parentclass << "&" << m.package << (char) 0;
                out_buf << m.eadr.byte1 << (char) 0;
                
            //   cout << (char) cplr_method << OP_MTHD << (char) 0 << m.name << "&" << m.parentclass << "&" << m.package << (char) 0 << " `";
            //   cout << m.eadr.byte1 << (char) 0 << endl;
         }
         else if(ins == OP_NODE)
         {
             cres.size_t.byte1--;
             cres.size_t.byte2--;
         }
         else if(ins == OP_COUT || ins == OP_STRCONST){
             cres.size_t.byte1 +=cplrfreelist1.valueAt(0).str.size() + 1;
             out_buf << (char) cplr_string_instr << ins << (char) 0;
             if(ins == OP_STRCONST)
             {
				 cres.size_t.byte1++;
                 out_buf << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << (char) 0;
		     }
               
             out_buf << cplrfreelist1.valueAt(0).str << (char) 0;
         }
         else if(ins == OP_NOP || ins == OP_END || ins == OP_HLT || ins == OP_NO || ins == OP_ENDNO)
             out_buf << (char) cplr_instr <<  ins << (char) 0 ;
         else if( ins == OP_RETURN || ins == OP_CALL)
         {
              cres.size_t.byte1 += cplrfreelist1.valueAt(0).size_t.byte1;
              Method m= methods.valueAt(cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1);
             // cout << "returning/calling " << m.eadr.byte1 << endl;
              out_buf << (char) cplr_instr << ins << (char) 0 << m.eadr.byte1 << (char) 0;
              if(ins == OP_CALL)
                 out_buf << cplrfreelist1.valueAt(0).sub_item.valueAt(1).item.byte1 << (char) 0;
         }
         else if(ins == OP_PUSH || ins == OP_POP || ins == OP_JMP || ins == OP_LBL || ins == OP_IF || ins == OP_INC || ins == OP_DEC
                 || ins == OP_KILL || ins == OP_DELETE || ins == OP_NEG || ins == OP_CIN){ // for instructions that access memory
               cout << ins << " -> " << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << endl;
               cres.size_t.byte1 += cplrfreelist1.valueAt(0).size_t.byte1;
               out_buf << (char) cplr_instr <<  ins << (char) 0  << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << (char) 0;
         }        
         else if(ins == OP_SCONST || ins == OP_BCONST || ins == OP_CCONST || ins == OP_RSHFT 
                   || ins == OP_LSHFT || ins == OP_JIF || ins == OP_JIT || ins == OP_ICONST 
                   || ins == OP_DCONST || ins == OP_FCONST || ins == OP_THROW || ins == OP_LCONST 
                   || ins == OP_BYTE_CONST || ins == OP_STR_APND || ins == OP_ASSN || ins == OP_BYTE_ACONST
                   || ins == OP_FACONST || ins == OP_DACONST || ins == OP_IACONST || ins == OP_LACONST
                   || ins == OP_SACONST || ins == OP_BACONST || ins == OP_STR_ACONST || ins == OP_CACONST
                   || ins == OP_STR_X2F || ins == OP_STR_X2I || ins == OP_BIT_CONST || ins == OP_BIT_ACONST)
         {
                cres.size_t.byte1 += cplrfreelist1.valueAt(0).size_t.byte1;
                out_buf << (char) cplr_instr <<  ins << (char) 0  << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << (char) 0
                << cplrfreelist1.valueAt(0).sub_item.valueAt(1).item.byte1 << (char) 0;   
               cout << ins << " -> " << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << " : " << cplrfreelist1.valueAt(0).sub_item.valueAt(1).item.byte1 << endl;
         }
         else if(ins == OP_ADD || ins == OP_ISEQ || ins == OP_ISLT || ins == OP_ISLE || ins == OP_ISGT 
                   || ins == OP_ISGE || ins == OP_SUB || ins == OP_MULT || ins == OP_DIV || ins == OP_MOD 
                   || ins == OP_OR || ins == OP_AND || ins == OP_AT || ins == OP_ALOAD || ins == OP_ASTORE
                   || ins == OP_CAST)
         {
                cres.size_t.byte1 += cplrfreelist1.valueAt(0).size_t.byte1;
                out_buf << (char) cplr_instr <<  ins << (char) 0  << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << (char) 0
                << cplrfreelist1.valueAt(0).sub_item.valueAt(1).item.byte1 << (char) 0 << cplrfreelist1.valueAt(0).sub_item.valueAt(2).item.byte1 << (char) 0;
         
                 cout << ins << " -> " << cplrfreelist1.valueAt(0).sub_item.valueAt(0).item.byte1 << " : " << cplrfreelist1.valueAt(0).sub_item.valueAt(1).item.byte1 
                       << " : " << cplrfreelist1.valueAt(0).sub_item.valueAt(2).item.byte1 << endl;
         }
         else {
             cglobals.out << "Unknown instruction (" << ins << ").";
             error(cglobals.lex);
             cres.size_t.byte1--;
         }
                
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
     cglobals.ignorestrays = false;
	 cglobals.nativeReturnLock = false; // prevent function confusion
     cglobals.methodadr = 0;
     cglobals.objectadr = 50;
     cglobals.scopelevel = 0;
     cglobals.infunction = 0;
     
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
                          memoryhelper::queuehelper::classhelper::insert(cglobals.classParent.size_t.byte1, classname, 
                                      typedef_class, "<null>", "<null>");
                                      
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
 
 string addpermissions(EsoHeader &header)
 {
    stringstream ss, ss1;
    ss << (char) permissions;
    for(int i = 0; i < header.permission_t; i++)
       ss1 << header.permissions[i] << ";";
    
    Binary::encode_str(ss1.str(), ss);
    return ss.str();
 }

void objto_eso(EsoHeader header, stringstream &out_buf)
 {
     stringstream ss1, __ostream_buf__;
     
     out_buf << (char)0 << "ESO     " << null(11) << (char)header.edian << (char) header.magic.byte1 << (char) header.magic.byte2 << (char) header.magic.byte3 << (char) header.magic.byte4 << (char) 0;
     out_buf << (char) name << header.name << null(3);
     
     ss1 << (long) header.method_size.byte1;
     
     Binary::encode_str(ss1.str(), __ostream_buf__);
     out_buf << (char) method_cap << __ostream_buf__.str()  << null(1);
     __ostream_buf__.str("");
     ss1.str("");
     
     out_buf << (char) minor_version << (char) header.minor_version.byte1 << (char) header.minor_version.byte2 << null(1);
     out_buf << (char) major_version << (char) header.major_version.byte1 << (char) header.major_version.byte2 << null(1);
     
     ss1 << (long) header.filesize.byte1;
    
     Binary::encode_str(ss1.str(), __ostream_buf__);
     ss1.str("");

     out_buf << (char) image_size << __ostream_buf__.str() << null(1);
     __ostream_buf__.str("");
     
     ss1 << header.target.byte1;
     Binary::encode_str(ss1.str(), __ostream_buf__);
     ss1.str("");
     
     out_buf << (char) target << __ostream_buf__.str() << null(1);
     __ostream_buf__.str("");
     
     ss1 << header.base_target.byte1;
     Binary::encode_str(ss1.str(), __ostream_buf__);
     ss1.str("");
     
     out_buf << (char) base_target << __ostream_buf__.str() << null(1);
     __ostream_buf__.str("");
     
     Binary::encode_str(header.version, __ostream_buf__);
     out_buf << (char) version << __ostream_buf__.str() << null(1) << '\n';
     __ostream_buf__.str("");
     
     out_buf << (char) debuggable << (long) header.debug.byte1 << null(1);
     out_buf << (char) loggable << (long) header.loggable.byte1 << null(1);
     out_buf << (char) log_level << (long)header.log_level.byte1 << null(1);
     
     ss1.str("");
     ss1 << header.instruction_cap.byte1;
     Binary::encode_str(ss1.str(), __ostream_buf__);
     out_buf << (char) instruction_cap << __ostream_buf__.str() << null(1);
     __ostream_buf__.str("");
     ss1.str("");
     
     Binary::encode_str(header.log_file, __ostream_buf__);
     out_buf << (char) log_file << __ostream_buf__.str()  << null(1);
     __ostream_buf__.str("");
     
     Binary::encode_str(header.app_id, __ostream_buf__);
     out_buf << (char) app_id << __ostream_buf__.str() << null(1) << "\n" << addpermissions(header);
     __ostream_buf__.str("");
     
     ss1.str("");
     ss1 << (long) header.address_cap.byte1;
    
     Binary::encode_str(ss1.str(), __ostream_buf__);
     
     out_buf << null(1) << (char) address_cap << __ostream_buf__.str() << null(1) << (char)eoh;
     out_buf << null(12) << "\n" << null(20);
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
        cres.size_t.byte2=0;
        cglobals.method_t=0;
        success= Compilr_Compile_Buf(zip_archive, __out_buf__);
        
        if(success==0)
        {
           cout<<"success\n";
           stringstream __new_buf__;
           
           EsoHeader head;
           head.edian = 'e';
           head.magic.byte1=0xd0;
           head.magic.byte2=0xfe;
           head.magic.byte3=0xe8;
           head.magic.byte4=0x2b;
           
           head.minor_version.byte1 = 0x70;
           head.minor_version.byte2 = 0x010;

           head.major_version.byte1 = 0x1;
           head.major_version.byte2 = 0x20;
           
           head.version = "0.1.0.0";
           head.debug.byte1 = 0;
           head.loggable.byte1 = 0;
           head.log_level.byte1 = 2; // verbose
           head.address_cap.byte1 = cglobals.objectadr;
        
           head.log_file = "/usr/share/scorpion/lib/compiler.log";
           head.target.byte1  = 10000000;
           head.base_target.byte1 = 8;
        
           head.app_id = "com.scorpion.microsystems";
           head.permissions = new string[2];
           head.permissions[0] = "scorpion.permission.READ_INTERNAL_STORAGE";
           head.permissions[1] = "scorpion.permission.WRITE_INTERNAL_STORAGE";
           head.name = "scorpionc";
           head.permission_t=2;
           
           head.filesize.byte1 = cres.size_t.byte1;
           head.instruction_cap.byte1 = cres.size_t.byte2; 
           head.method_size.byte1 = cglobals.method_t;
           objto_eso(head, __new_buf__);
           
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
 
 