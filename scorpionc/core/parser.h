/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for parsing a scorpion 
* source file
* 
*/
#ifndef parser_h
#define parser_h

#include "lexer.h"
#include "ast.h"
#include "string.h"

class parser
{
    public:
      parser(string src)
      {
          source.hash_start();
          source.str(src);
      }
      bool parse();
      
    private:
      sstring source;
      lexer lex_;
      
      ast_node identifier();
      ast_node float_literal();
      ast_node int_literal();
      ast_node string_literal();
      ast_node char_literal();
      ast_node bool_literal();
      ast_node object_type();
      ast_node int_type();
      ast_node float_type();
      ast_node bool_type();
      ast_node string_type();
      ast_node char_type();
      ast_node id_type();
      ast_node type();
      ast_node begin_paren();
      ast_node end_paren();
      ast_node function_call();
      ast_node member_access();
      ast_node logical_not_expr();
      ast_node positive_expr();
      ast_node negative_expr();
      ast_node mul_op();
      ast_node div_op();
      ast_node add_op();
      ast_node sub_op();
      ast_node eq_op();
      ast_node diff_op();
      ast_node lt_op();
      ast_node lte_op();
      ast_node gt_op();
      ast_node gte_op();
      ast_node log_and_op();
      ast_node log_or_op();
      ast_node cond_op();
      ast_node var_stm();
      ast_node block_begin();
      ast_node block_end();
      ast_node block_stm();
      ast_node for_init();
      ast_node for_cond();
      ast_node for_step();
      ast_node for_stm();
      ast_node while_stm();
      ast_node else_if_stm();
      ast_node else_block_stm();
      ast_node if_stm();
      ast_node return_stm();
      ast_node assignment_stm();
      ast_node expression_stm();
      ast_node struct_decl();
      ast_node var_inst();
      ast_node var_init();
      ast_node var_def();
      ast_node var_decl();
      ast_node func_decl();
      ast_node literal_value();
      ast_node value();
      ast_node unary_expr();
      ast_node mul_expr();
      ast_node add_expr();
      ast_node cmp_expr();
      ast_node eq_expr();
      ast_node logical_expr();
      ast_node cond_expr();
      ast_node func_cal_params();
      ast_node func_args();
      ast_node expression();
      ast_node statement();
      ast_node struct_member();
      ast_node declaration();
      ast base_grammar();
};

extern ArrayList<parser> parser_stack;
extern ArrayList<ast> ast_stack;


#endif // parser_h