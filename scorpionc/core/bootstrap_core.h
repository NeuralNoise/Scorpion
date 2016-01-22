/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for bootstrapping the 
* compiler
* 
*/
#ifndef boostrap_core_h
#define boostrap_core_h

#include "access_control_.h"
#include "array_list.h"
#include "string.h"
#include "lexer.h"
#include <string>
#include <stdint.h>
#include <limits>
 
struct file_state
{
  lexer s_lexer;
  long long scope_level, package_status;
  int class_depth, namespace_depth, lex_mode,
      in_function;
  sstring name, function_info, package;
  bool asm_message;
  
  void clear()
  {
      s_lexer.clear();
      name.str("");
      function_info.str("");
      package.str("");
  }
  void state_start()
  {
     lex_mode = (0 << mode_ignore_strays) | (0 << mode_ignore_errors);
     scope_level = 0;
     class_depth = 0;
     package_status = 0;
     namespace_depth = 0;
     name.hash_start();
     function_info.hash_start();
     package.hash_start();
     in_function = 0;
     asm_message = true;
  }
};

void core_error(file_state& f_state);
void core_warning(file_state& f_state, std::string type);

void run_bootstrap();

// Core data structs
struct ClassObject
{
   access_priv priv;
   sstring name, parent_class,
        package, file;
   uint64_t id, parent_class_id;
};

enum // types
{
  t_bit    = 1, t_bool   = 2, t_byte = 3,
  t_short  = 4, t_int    = 5, t_long = 6,
  t_float  = 7, t_double = 8, t_char = 9,
  t_string = 10
};

#define bit_max 1
#define bit_min 0

#define byte_min std::numeric_limits<int8_t>::min();
#define byte_max std::numeric_limits<int8_t>::max();

#define short_max std::numeric_limits<int16_t>::max();
#define short_min std::numeric_limits<int16_t>::min();

#define int_min std::numeric_limits<int32_t>::min();                                                                                                                                                                                                                                                                                             
#define int_max std::numeric_limits<int32_t>::max();

#define long_min std::numeric_limits<int64_t>::min();                                                                                                                                                                                                                                                                                             
#define long_max std::numeric_limits<int64_t>::max();

#define float_min std::numeric_limits<float>::min();                                                                                                                                                                                                                                                                                              
#define float_max std::numeric_limits<float>::max();

#define double_min std::numeric_limits<double>::min();                                                                                                                                                                                                                                                                                              
#define double_max std::numeric_limits<double>::max();

#define char_max short_max                                                                                                                                                                                                                                                                                              
#define char_min short_min

struct Object
{ 

   int type, scope;
   int64_t unique_ptr, class_id; // pointer index in heap ram
   access_priv priv;
   
   bool object_array;
   sstring name, package, file,
        parent_class;
};

struct Method
{
   uint64_t unique_ptr, parent_class_id;
   access_priv priv;
    
   sstring name, package, file,
       parent_class;
   
   ArrayList<Object> args;
};

#endif // boostrap_core