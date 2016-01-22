/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for parsing a scorpion 
* source file
* 
*/
#ifndef grammar_h
#define grammar_h

#include "string.h"
#include "ast.h"
#include "array_list.h"
#include "lexer.h"

enum // grammar_flags
{
    flag_mult = 1, flag_min = 2, 
    flag_plus = 3, flag_function = 3,
    flag_sub_rule = 4
};

#define flag(s,i) (1 & (i >> s))
#define set_flag(s,x,i) ((i) ^= (-x ^ i) & (1 << s)) // s = grammar_flag, x = on/off, i = flag

class rule
{
    public: 
      int flags; // * | - | + | function | sub_rule
      sstring name, _rule;
      int bind; // |, &, !
      
      ArrayList<rule> binded_rules;
};

class grammar;

/**
* Grammar parser
* rule example: 
*   pre-difiled values
*   <rule> := (+|-|*)<name>
* ***************************
* 
*   <name> := <rule>((&|!|\|)<rule>
*/
class grammar_parser
{
    protected:
      lexer grammar_lexer;
      bool parse(std::string gram, grammar& grammars);
};

class grammar : public grammar_parser
{
    public:
      grammar()
      {
          error_check = false;
          gram.hash_start();
      }
      void set_grammar(sstring gram);
      ArrayList<sstring>& error_list_();
      ast parse_grammar();
      
      sstring gram;
      bool error_check;
      rule grammar_rule;
      
    
    private:
      ast grammar_ast;
      ArrayList<sstring> error_list;
      bool identifier(); 
      bool float_literal(); 
      bool integer_literal(); 
      bool string_literal();
      bool char_literal();
      bool bool_literal();
      bool object_type();
      bool int_type();
      bool float_type();
      bool bool_type();
      bool char_type();
      bool string_type();
      bool begin_paren();
      bool end_paren();
      bool comma();
      bool dot();
      bool statement_end();
      bool block_begin();
      bool block_end();
      bool not_();
      bool eq();
      bool plus();
      bool minus();
      bool mult();
      bool divide();
      bool lt();
      bool gt();
      bool lte_op();
      bool gte_op();
      bool eq_eq();
      bool neq();
      bool log_and();
      bool log_or();
      bool question();
      bool col();
      bool for_();
      bool while_();
      bool node();
      bool if_();
      bool else_();
      bool def();
};


/**************** Grammar types **********************/
void grammar_init();
bool binder(std::string s);

extern grammar identifier;
extern grammar float_literal;
extern grammar int_literal;
extern grammar string_literal;
extern grammar char_literal;
extern grammar bool_literal;
extern grammar object_type;
extern grammar int_type;
extern grammar float_type;
extern grammar bool_type;
extern grammar string_type;
extern grammar char_type;
extern grammar id_type;
extern grammar type;
extern grammar begin_paren;
extern grammar end_paren;
extern grammar function_call;
extern grammar member_access;
extern grammar logical_not_expr;
extern grammar positive_expr;
extern grammar negative_expr;
extern grammar mul_op;
extern grammar div_op;
extern grammar add_op;
extern grammar sub_op;
extern grammar eq_op;
extern grammar diff_op;
extern grammar lt_op;
extern grammar lte_op;
extern grammar gt_op;
extern grammar gte_op;
extern grammar log_and_op;
extern grammar log_or_op;
extern grammar cond_op;
extern grammar var_stm;
extern grammar block_begin;
extern grammar block_end;
extern grammar block_stm;
extern grammar for_init;
extern grammar for_cond;
extern grammar for_step;
extern grammar for_stm;
extern grammar while_stm;
extern grammar else_if_stm;
extern grammar else_block_stm;
extern grammar if_stm;
extern grammar print_stm;
extern grammar return_stm;
extern grammar assignment_stm;
extern grammar expression_stm;
extern grammar struct_decl;
extern grammar var_inst;
extern grammar var_init;
extern grammar var_def;
extern grammar var_decl;
extern grammar func_decl;
extern grammar literal_value;
extern grammar value;
extern grammar unary_expr;
extern grammar mul_expr;
extern grammar add_expr;
extern grammar cmp_expr;
extern grammar eq_expr;
extern grammar logical_expr;
extern grammar cond_expr;
extern grammar expression;
extern grammar statement;
extern grammar struct_member;
extern grammar declaration;
extern grammar base_grammar;
 
#endif // grammar_h