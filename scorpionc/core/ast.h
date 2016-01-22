/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for parsing a scorpion 
* source file
* 
*/
#ifndef ast_h
#define ast_h

#include "array_list.h"
#include "string.h"

enum // ast_base_types
{
    ast_function_call = 100, ast_func_params = 101,
    ast_struct = 102, ast_var = 103, ast_function = 104,
    ast_member_access = 105, ast_expression = 106, 
    ast_not_expr = 107, ast_plus_expr = 108,
    ast_min_expr = 109, ast_mult_expr = 110,
    ast_div_expr = 111, ast_add_expr = 112, 
    ast_sub_expr = 113, ast_lt_expr = 114,
    ast_lte_expr = 115, ast_gt_expr = 116,
    ast_gte_expr = 117, ast_compare_expr = 118,
    ast_eq_expr = 119, ast_diff_expr =120, 
    ast_log_and_expr = 121, ast_log_or_expr = 122,
    ast_type_expr = 123, ast_condition_expr = 124,
    ast_log_expr = 125
};

enum // ast_failures
{
    ast_failed = 0, ast_syntax_fail = 1
};

enum // ast_types
{
    ast_identifier = 2, ast_func_args = 3, 
    ast_block_stm = 4, ast_float_literal = 5,
    ast_int_literal = 6, ast_string_literal = 7,
    ast_char_literal = 8, ast_bool_literal = 9,
    ast_left_paren = 10, ast_right_paren = 11, 
    ast_object_literal = 12, ast_exclamation = 13,
    ast_plus = 14, ast_minus = 15, ast_asterisk = 16,
    ast_slash = 17, ast_lt = 18, ast_lte = 19, ast_gt = 20,
    ast_gte = 21, ast_eq_eq = 22, ast_not_eq = 23, ast_log_and = 24,
    ast_log_or = 25, ast_col = 26, ast_left_bracket = 27, 
    ast_right_bracket = 28
};

class ast_node
{
    public:
       int atom; // the type of node
       sstring value; // the node's value

       ast_node()
       {
       }
       
       ast_node(int a)
       {
           atom =a;
           value.hash_start();
       }

       void ast_start();
       void new_child(ast_node&);
       ArrayList<ast_node>& childs();
       void destruct();
       void new_();
    
    private:
      ArrayList<ast_node> child_nodes;
};

class ast_container
{
    public:
       ast_node& member(unsigned long);
       void new_member(ast_node&);
       void construct();
       void destruct();
    
     private:
       ast_node* ast_members; // our parent members for the ast
       unsigned long member_t; // the size of our member array
};

// out base class for our ast
class ast : public ast_container
{
};

// this will hold all our ast's for or entire program
class prog_ast
{
    public:
      ast& base_ast_();
    
    private:
      ast base_ast;
};

class struct_ast
{
     
};

class var_ast
{
     
};

class func_decl_ast
{
     
};
#endif // ast_h