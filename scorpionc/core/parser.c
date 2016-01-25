/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for parsing a scorpion 
* source file
* 
*/
#include "parser.h"
#include <sstream>

ArrayList<parser> parser_stack;
ArrayList<ast> ast_stack;
std::stringstream syntax_error, errno;

void parser::parse_error() {
  if(errno.str() == "") {
    errno << file.str() << ":" << lex_.line << ":" << lex_.col 
        << "  error: " <<  syntax_error.str() << endl;
  }
  syntax_error.str("");
}

bool parser::parse()
{ 
    int lex_modes = (0 << mode_ignore_strays) | 
           (0 << mode_ignore_errors);
    
    cout << "parsing: \n" << source.str() << endl; 
    lex_.init(source.str(), lex_modes);
    
    base_grammar();
    return true;
}

bool reserved_id(std::string symbol)
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
            || symbol == "private" || symbol == "protected" || symbol == "import"
            || symbol == "package" || symbol == "asm");
} 

ast_node parser::identifier() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.kind == k_symbol) {
      n.atom = ast_identifier;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::float_literal() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.is_float) {
      n.atom = ast_float_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::int_literal() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.kind == k_number || t.kind == k_hex_number) {
      n.atom = ast_int_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::string_literal() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.kind == k_string) {
      n.atom = ast_string_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::char_literal() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.kind == k_char) {
      n.atom = ast_char_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::bool_literal() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.text.str() == "true" || t.text.str() == "false") {
      n.atom = ast_bool_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::object_type() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.text.str() == "object") {
      n.atom = ast_object_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::int_type() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.text.str() == "int" || t.text.str() == "short" || t.text.str() == "byte"
       || t.text.str() == "long") {
      n.atom = ast_int_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::float_type() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.text.str() == "float" || t.text.str() == "double") {
      n.atom = ast_float_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::bool_type() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.text.str() == "bool" || t.text.str() == "bit") {
      n.atom = ast_bool_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::string_type() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.text.str() == "string") {
      n.atom = ast_string_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::char_type() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.text.str() == "char") {
      n.atom = ast_char_literal;
      n.value = t.text;
      return n;
  }
  return n;
}

ast_node parser::id_type() {
    return identifier();
}

ast_node parser::type(bool static_) {
     ast_node ast_t(ast_failed);
     unsigned long iter = lex_.iter;
     
     if((ast_t = int_type()).atom != ast_failed) {
         return ast_t;
     }
     else lex_.iter = iter;
     
     if((ast_t = float_type()).atom != ast_failed) {
         return ast_t;
     }
     else lex_.iter = iter;
     
     if((ast_t = bool_type()).atom != ast_failed) {
         return ast_t;
     }
     else lex_.iter = iter;
     
     if((ast_t = string_type()).atom != ast_failed) {
         return ast_t;
     }
     else lex_.iter = iter;
     
     if((ast_t = char_type()).atom != ast_failed) {
         return ast_t;
     }
     else lex_.iter = iter;
     
     if((ast_t = object_type()).atom != ast_failed) {
         return ast_t;
     }
     else lex_.iter = iter;
     
     if((ast_t = id_type()).atom != ast_failed) {
         if(static_ && reserved_id(ast_t.value.str()))
           ast_t.atom = ast_failed;
         return ast_t;
     }
    
     // error 
     return ast_t;
}

ast_node parser::begin_paren() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.text.str() == "(") {
      n.atom = ast_left_paren;
      n.value = t.text;
      return n;
  }
  else {
      syntax_error << "expected '(' before `" << t.text.str() << "'.";
      parse_error();
  }
  return n;
}

ast_node parser::end_paren() {
  ast_node n(ast_failed);
  token t = lex_.next_token();
  
  if (t.text.str() == ")") {
      n.atom = ast_right_paren;
      n.value = t.text;
      return n;
  }
  else {
      syntax_error << "expected ')' before `" << t.text.str() << "'.";
      parse_error();
  }
  return n;
}

ast_node parser::func_call_params() {
    ast_node ast__(ast_func_params), ast_t(ast_failed);
   
   param:
     if((ast_t = expression()).atom != ast_failed)
       ast__.new_child( ast_t );
     else {
         return ast_t;
     } 
     
     if(lex_.peek_next_token().text.str() == ",") {
         lex_.iter++;
         goto param;
     }
     return ast__;
}

ast_node parser::function_call() {
   ast_node ast__(ast_function_call), ast_t(ast_failed);
   
   if((ast_t = identifier()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected identifier before `" << lex_.tok.text.str() << "'.";
       parse_error();
       return ast_t;
   }
   
   if((ast_t = begin_paren()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       return ast_t;
   }
   
   if(lex_.peek_next_token().text.str() != ")") {
       if((ast_t = func_call_params()).atom != ast_failed)
           ast__.new_child( ast_t );
       else {
           return ast_t;
       }
   }
   
   if((ast_t = end_paren()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       return ast_t;
   }
   return ast__;
}

ast_node parser::member_access() {
   ast_node ast__(ast_member_access), ast_t(ast_failed);
   
   if((ast_t = identifier()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected identifier before `" << lex_.tok.text.str() << "'.";
       parse_error();
       return ast_t;
   }
   
   member:
   if(lex_.peek_next_token().text.str() == ".") {
       lex_.iter++;
       
       if((ast_t = identifier()).atom != ast_failed)
           ast__.new_child( ast_t );
       else {
           syntax_error << "expected identifier before `" << lex_.tok.text.str() << "'.";
           parse_error();
           return ast_t;
       }
       goto member;
   }
   return ast__;
}

ast_node parser::literal_value() {
     ast_node ast_t(ast_failed);
     unsigned long iter = lex_.iter;
     
     if((ast_t = float_literal()).atom != ast_failed) {
         return ast_t;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast_t = int_literal()).atom != ast_failed) {
         return ast_t;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast_t = string_literal()).atom != ast_failed) {
         return ast_t;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast_t = char_literal()).atom != ast_failed) {
         return ast_t;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast_t = bool_literal()).atom != ast_failed) {
         return ast_t;
     }
     
     // error
     return ast_t;
}


ast_node parser::value() {
     ast_node ast__(ast_failed), ast_t;
     unsigned long iter = lex_.iter;
     
     if((ast_t = function_call()).atom != ast_failed) {
         return ast_t;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast_t = member_access()).atom != ast_failed) {
         return ast_t;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast_t = literal_value()).atom != ast_failed) {
         return ast_t;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if(lex_.peek_next_token().text.str() == "(") {
         ast__.new_child( (ast_t = begin_paren()) );
         lex_.iter++;
         
         if((ast_t = expression()).atom != ast_failed) {
            ast__.new_child( ast_t );
         }
         else {
               return ast_t;
         }
         
         if((ast_t = end_paren()).atom != ast_failed) {
            ast__.new_child( ast_t );
         }
         else {
               return ast_t;
         }
         
         ast__.atom = ast_value_expression;
         return ast__;
     }
     
     // error
     return ast__;
}

ast_node parser::logical_not_expr() {
     ast_node ast__(ast_not_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "!") {
           ast_t.atom = ast_exclamation;
           ast_t.value.str("!");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '!' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = unary_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::positive_expr() {
     ast_node ast__(ast_plus_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "+") {
           ast_t.atom = ast_plus;
           ast_t.value.str("+");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '+' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = unary_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::negative_expr() {
     ast_node ast__(ast_min_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "-") {
           ast_t.atom = ast_minus;
           ast_t.value.str("-");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '-' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = unary_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}


ast_node parser::unary_expr() {
     ast_node ast__(ast_failed);
     unsigned long iter = lex_.iter;
     
     if((ast__ = logical_not_expr()).atom != ast_failed) {
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast__ = positive_expr()).atom != ast_failed) {
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast__ = negative_expr()).atom != ast_failed) {
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast__ = value()).atom != ast_failed) {
         goto end;
     }
     
     // error
     end:
       return ast__;
}

ast_node parser::mul_op() {
     ast_node ast__(ast_mult_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "*") {
           ast_t.atom = ast_asterisk;
           ast_t.value.str("*");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '*' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = unary_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::div_op() {
     ast_node ast__(ast_div_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "/") {
           ast_t.atom = ast_slash;
           ast_t.value.str("/");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '/' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = unary_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::mul_expr() {
     ast_node ast__(ast_mult_expr), ast_t(ast_failed);
     unsigned long iter;

     if((ast_t = unary_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     
     op_expr:
       iter = lex_.iter;
       if((ast_t = mul_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
         lex_.iter = iter;
         errno.str("");
       }
     
       if((ast_t = div_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
         lex_.iter = iter;
         errno.str("");
         goto end;
       }
     
     end:
       return ast__;
}

ast_node parser::add_op() {
     ast_node ast__(ast_plus_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "+") {
           ast_t.atom = ast_plus;
           ast_t.value.str("+");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '+' before `" << lex_.tok.text.str() << "'.";
         return ast_t;
     }
     
     if((ast_t = mul_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::sub_op() {
     ast_node ast__(ast_sub_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "-") {
           ast_t.atom = ast_minus;
           ast_t.value.str("-");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '-' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = mul_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::add_expr() {
     ast_node ast__(ast_add_expr), ast_t(ast_failed);
     unsigned long iter;
     
     if((ast_t = mul_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     
     op_expr:
       iter = lex_.iter;
       if((ast_t = add_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
       }
     
       if((ast_t = sub_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
           goto end;
       }
     
     end:
       return ast__;
}


ast_node parser::lt_op() {
     ast_node ast__(ast_lt_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "<") {
           ast_t.atom = ast_lt;
           ast_t.value.str("<");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '<' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = add_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::lte_op() {
     ast_node ast__(ast_lte_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "<=") {
           ast_t.atom = ast_lte;
           ast_t.value.str("<=");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '<=' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = add_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}


ast_node parser::gt_op() {
     ast_node ast__(ast_gt_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == ">") {
           ast_t.atom = ast_gt;
           ast_t.value.str(">");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '>' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = add_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::gte_op() {
     ast_node ast__(ast_gte_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == ">=") {
           ast_t.atom = ast_gte;
           ast_t.value.str(">=");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '>=' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = add_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::cmp_expr() {
     ast_node ast__(ast_compare_expr), ast_t(ast_failed);
     unsigned long iter;
     
     if((ast_t = add_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     
     op_expr:
       iter = lex_.iter;
       if((ast_t = lt_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
       }
     
       if((ast_t = lte_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
       }
       
       if((ast_t = gt_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
       }
       
       if((ast_t = gte_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
           goto end;
       }
     
     end:
       return ast__;
}

ast_node parser::eq_op() {
     ast_node ast__(ast_eq_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "==") {
           ast_t.atom = ast_eq_eq;
           ast_t.value.str("==");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '==' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = cmp_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::diff_op() {
     ast_node ast__(ast_diff_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "!=") {
           ast_t.atom = ast_not_eq;
           ast_t.value.str("!=");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '!=' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = cmp_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::eq_expr() {
     ast_node ast__(ast_eq_expr), ast_t(ast_failed);
     unsigned long iter;
     
     if((ast_t = cmp_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected expression before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     op_expr:
       iter = lex_.iter;
       if((ast_t = eq_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
       }
       
       if((ast_t = diff_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
           goto end;
       }
     
     end:
       return ast__;
}

ast_node parser::log_and_op() {
     ast_node ast__(ast_log_and_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "&&") {
           ast_t.atom = ast_log_and;
           ast_t.value.str("&&");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '&&' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = eq_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}

ast_node parser::log_or_op() {
     ast_node ast__(ast_log_or_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "||") {
           ast_t.atom = ast_log_or;
           ast_t.value.str("||");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '||' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = eq_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     return ast__;
}


ast_node parser::logical_expr() {
     ast_node ast__(ast_log_expr), ast_t(ast_failed);
     unsigned long iter;
     
     if((ast_t = eq_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected expression before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     op_expr:
       iter = lex_.iter;
       if((ast_t = log_and_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
       }
       
       if((ast_t = log_or_op()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto op_expr;
       }
       else {
           lex_.iter = iter;
           errno.str("");
           goto end;
       }
     
     end:
       return ast__;
}

ast_node parser::cond_op() {
     ast_node ast__(ast_condition_expr), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "?") {
           ast_t.atom = ast_eq_eq;
           ast_t.value.str("?");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '?' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = cond_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     
     if(lex_.next_token().text.str() == ":") {
           ast_t.atom = ast_col;
           ast_t.value.str(":");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected ':' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     if((ast_t = cond_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     
     return ast__;
}

ast_node parser::cond_expr() {
     ast_node ast__(ast_condition_expr), ast_t(ast_failed);
     unsigned long iter;
     
     if((ast_t = logical_expr()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     
     iter = lex_.iter;
    
     if((ast_t = cond_op()).atom != ast_failed) {
         ast__.new_child( ast_t );
     }
     else {
         lex_.iter = iter;
         errno.str("");
     }
       
     return ast__;
}

ast_node parser::expression() {
   return cond_expr();
}

ast_node parser::block_begin() {
     ast_node ast__(ast_left_bracket), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "{") {
           ast_t.atom = ast_left_bracket;
           ast_t.value.str("{");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '{' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     return ast__;
}

ast_node parser::block_end() {
     ast_node ast__(ast_right_bracket), ast_t(ast_failed);
     
     if(lex_.next_token().text.str() == "}") {
           ast_t.atom = ast_right_bracket;
           ast_t.value.str("}");
           
           ast__.new_child( ast_t );
     }
     else {
         syntax_error << "expected '}' before `" << lex_.tok.text.str() << "'.";
         parse_error();
         return ast_t;
     }
     
     return ast__;
}

ast_node parser::block_stm() {
     ast_node ast__(ast_gte_expr), ast_t(ast_failed);
     unsigned long iter;
     
     cout << "block begin " << lex_.peek_next_token().text.str() << "\n";
     if((ast_t = block_begin()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
     cout << "new block\n";
     statement_:
        iter = lex_.iter;
        if((ast_t = statement()).atom != ast_failed) {
            if(ast_t.atom == ast_syntax_fail) {
                return ast_t;
            }
     
            ast__.new_child( ast_t );
            goto statement_;
        }
        else {
            lex_.iter = iter;
            errno.str("");
        }
     
     cout << "end " << lex_.peek_next_token().text.str() << endl;
     if((ast_t = block_end()).atom != ast_failed) {
            ast__.new_child( ast_t );
     }
     else {
         return ast_t;
     }
       
     return ast__;
}

ast_node parser::var_stm() {
    return var_def();
}

ast_node parser::for_init() {
     ast_node ast__(ast_for_init_expr), ast_t(ast_failed);
     unsigned long iter = lex_.iter;
     
       if((ast_t = var_stm()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto end;
       }
       else {
           lex_.iter = iter;
           errno.str("");
       }
       
       iter = lex_.iter;
       if((ast_t = assignment_stm()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto end;
       }
       else {
           lex_.iter = iter;
       }
     
     // error
     ast__.atom = ast_failed;
     end:
       return ast__;
}

ast_node parser::for_cond() {
    return expression();
}

ast_node parser::for_step() {
    return assignment_stm();
}

ast_node parser::for_stm() {
   ast_node ast__(ast_for_stm_expr), ast_t(ast_failed);
   syntax_error.str("");
   cout << "for " << lex_.peek_next_token().text.str() << endl;
   if(lex_.peek_next_token().text.str() != "for") {
       return ast_t;
   }
   else {
       lex_.next_token();
   }
   
   if((ast_t = begin_paren()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_syntax_fail; 
       return ast__;
   }
   
   if(lex_.peek_next_token().text.str() != ";") {
       if((ast_t = for_init()).atom != ast_failed)
           ast__.new_child( ast_t );
       else {
           ast__.atom = ast_syntax_fail; 
           return ast__;
       }
   }
   
   if(lex_.next_token().text.str() == ";")
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected ';' before `" << lex_.tok.text.str() << "'.";
       parse_error();
       ast__.atom = ast_syntax_fail; 
       return ast__;
   }
   
   if(lex_.peek_next_token().text.str() != ";") {
       if((ast_t = for_cond()).atom != ast_failed)
           ast__.new_child( ast_t );
       else {
           ast__.atom = ast_syntax_fail; 
           return ast__;
       }
   }
   
   if(lex_.next_token().text.str() == ";")
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected ';' before `" << lex_.tok.text.str() << "'.";
       parse_error();
       ast__.atom = ast_syntax_fail; 
       return ast__;
   }
   
   if(lex_.peek_next_token().text.str() != ";") {
       if((ast_t = for_step()).atom != ast_failed)
           ast__.new_child( ast_t );
       else {
           ast__.atom = ast_syntax_fail; 
           return ast__;
       }
   }
   
   if(lex_.next_token().text.str() == ";")
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected ';' before `" << lex_.tok.text.str() << "'.";
       parse_error();
       ast__.atom = ast_syntax_fail; 
       return ast__;
   }
   
   if((ast_t = end_paren()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_syntax_fail; 
       return ast__;
   }
   
   if((ast_t = block_stm()).atom != ast_failed) {
       if(ast_t.atom == ast_syntax_fail) {
           ast__.atom = ast_syntax_fail; 
           return ast__;
       }
       ast__.childs().get( ast__.childs().size() - 1 )
         .childs() = ast_t.childs();
   }
   else  {
       ast__.atom = ast_syntax_fail; 
       return ast__;
   }
   
   return ast__;
}

ast_node parser::while_stm() {
   ast_node ast__(ast_for_stm_expr), ast_t(ast_failed);
   syntax_error.str("");
   
   if(lex_.peek_next_token().text.str() != "while") {
       return ast_t;
   }
   else {
       lex_.next_token();
   }
   
   if((ast_t = begin_paren()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if((ast_t = expression()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if((ast_t = end_paren()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if((ast_t = block_stm()).atom != ast_failed) {
       if(ast_t.atom == ast_syntax_fail) {
           ast__.atom = ast_failed; 
           return ast__;
       }
       ast__.childs().get( ast__.childs().size() - 1 )
         .childs() = ast_t.childs();
   }
   else  {
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   return ast__;
}

ast_node parser::else_if_stm() {
    return if_stm();
}

ast_node parser::else_block_stm() {
    return block_stm();
}

ast_node parser::if_stm() {
   ast_node ast__(ast_for_stm_expr), ast_t(ast_failed);
   syntax_error.str("");
   unsigned long iter;
   
   if(lex_.peek_next_token().text.str() != "if") {
       return ast_t;
   }
   else {
       lex_.next_token();
   }
   
   if((ast_t = begin_paren()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if((ast_t = expression()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if((ast_t = end_paren()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if((ast_t = block_stm()).atom != ast_failed) {
       if(ast_t.atom == ast_syntax_fail) {
           ast__.atom = ast_failed; 
           return ast__;
       }
       ast__.childs().get( ast__.childs().size() - 1 )
         .childs() = ast_t.childs();
   }
   else  {
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if(lex_.peek_next_token().text.str() == "else") {
       lex_.iter++;
       ast_t.atom = ast_else_expr;
       iter = lex_.iter;
     
       if((ast_t = else_if_stm()).atom != ast_failed) {
           ast__.new_child( ast_t );
           goto end;
       }
       else {
           lex_.iter = iter;
           errno.str("");
       }
       
       if((ast_t = else_block_stm()).atom != ast_failed) {
           if(ast_t.atom == ast_syntax_fail) {
               ast__.atom = ast_failed; 
               return ast__;
           }
           ast__.new_child( ast_t );
           goto end;
       }
       else {
           lex_.iter = iter;
           return ast_t;
       }
       
       // error
   }
   
   end:
     return ast__;
}

ast_node parser::return_stm() {
   ast_node ast__(ast_return_expr), ast_t(ast_failed);
   unsigned long iter;
   
   if(lex_.peek_next_token().text.str() != "return") {
       return ast_t;
   }
   else {
       lex_.next_token();
   }
   
   iter = lex_.iter;  
   if((ast_t = expression()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       lex_.iter = iter;
       errno.str("");
   }
       
   return ast__;
}

ast_node parser::assignment_stm() {
   ast_node ast__(ast_return_expr), ast_t(ast_failed);
   
   if((ast_t = expression()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_failed; 
       return ast__;
   }
    
   if(lex_.next_token().text.str() == "=") {
       ast__.new_child( ast_t );
   }
   else {
       syntax_error << "expected '=' before `" << lex_.tok.text.str() << "'.";
       parse_error();
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if((ast_t = expression()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       ast__.atom = ast_failed; 
       return ast__;
   }
       
   return ast__;
}

ast_node parser::expression_stm() {
    return expression();
}

ast_node parser::statement() {
     ast_node ast__(ast_syntax_fail);
     unsigned long iter = lex_.iter;
     bool semi = false;
     
     if((ast__ = block_stm()).atom != ast_failed) {
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast__ = for_stm()).atom != ast_failed) {
     cout << "for statement\n";
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast__ = while_stm()).atom != ast_failed) {
     cout << "while statement\n";
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast__ = if_stm()).atom != ast_failed) {
     cout << "if statement\n";
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast__ = return_stm()).atom != ast_failed) {
     cout << "return statement\n";
         semi = true;
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str(""); 
     }
     
     if((ast__ = assignment_stm()).atom != ast_failed) {
     cout << "assignment statement\n";
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast__ = var_stm()).atom != ast_failed) {
     cout << "var statement\n";
         semi = true;
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if((ast__ = expression_stm()).atom != ast_failed) {
     cout << "expression statement\n";
         semi = true;
         goto end;
     }
     else {
       lex_.iter = iter;
       errno.str("");
     }
     
     if(lex_.peek_next_token().text.str() == ";") {
         lex_.iter++;
         ast__.atom = ast_semicolon;
         ast__.value.str(";");
         goto end;
     }
     
     
     // error
     end:
       if(semi && lex_.next_token().text.str() != ";") {
           syntax_error << "expected ';' before `" << lex_.tok.text.str() << "'.";
           parse_error();
           ast__.atom = ast_syntax_fail;
       }
       return ast__;
}

ast_node parser::var_inst() {
   ast_node ast__(ast_return_expr), ast_t(ast_failed);
   syntax_error.str("");
   
   if((ast_t = type(true)).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected type before `" << lex_.tok.text.str() << "'.";
       parse_error();
       return ast_t;
   }
   
   if((ast_t = identifier()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected identifier before `" << lex_.tok.text.str() << "'.";
       parse_error(); 
       return ast_t;
   }
       
   return ast__;
}

ast_node parser::var_init() {
   ast_node ast__(ast_var_init_expr), ast_t(ast_failed);
   syntax_error.str("");
    
   if(lex_.next_token().text.str() == "=") {
       ast__.new_child( ast_t );
   }
   else {
       syntax_error << "expected '=' before `" << lex_.tok.text.str() << "'.";
       ast__.atom = ast_failed; 
       parse_error();
       return ast__;
   }
   
   if((ast_t = expression()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       return ast__;
   }
       
   return ast__;
}

ast_node parser::var_def() {
   ast_node ast__(ast_return_expr), ast_t(ast_failed);
   unsigned long iter;
   
   if((ast_t = var_inst()).atom != ast_failed)
       ast__.new_child( ast_t );
   else { 
       return ast_t;
   }
       
    iter = lex_.iter;   
   if((ast_t = var_init()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       lex_.iter = iter;
       errno.str("");
   }
   
   return ast__;
}

ast_node parser::struct_member() {
     ast_node ast__(ast_failed);
     unsigned long iter = lex_.iter;
     
     if((ast__ = var_inst()).atom != ast_failed) {
         goto end;
     }
     else {
        lex_.iter = iter;
        errno.str("");
     }
     
     if(lex_.peek_next_token().text.str() == ";") {
         lex_.iter++;
         goto end;
     }
     
     // error
     end:
       return ast__;
}

ast_node parser::func_args() {
   ast_node ast__(ast_struct), ast_t(ast_failed);
   syntax_error.str("");
   
   new_arg:
   if((ast_t = type()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected type before `" << lex_.tok.text.str() << "'.";
       parse_error();
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if((ast_t = identifier()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected identifier before `" << lex_.tok.text.str() << "'.";
       parse_error();
       ast__.atom = ast_failed; 
       return ast__;
   }
   
   if(lex_.peek_next_token().text.str() == ",") {
           lex_.iter++;
           goto new_arg;
   }
   else {
       goto end;
   }
   
   end:
     return ast__;
}

ast_node parser::struct_decl() {
   ast_node ast__(ast_struct), ast_t(ast_failed);
   syntax_error.str("");
   
   if(lex_.peek_next_token().text.str() != "node") {
       return ast_t;
   }
   else {
       lex_.next_token();
   }
   return ast__;
}

ast_node parser::var_decl() {
   ast_node ast__(ast_var), ast_t(ast_failed);
   syntax_error.str("");
   unsigned iter = lex_.iter;
   
   if((ast_t = type(true)).atom != ast_failed) {
      lex_.iter = iter;
      return var_def();
   }
   else {
       lex_.iter = iter;
       return ast_t;
   }
}

ast_node parser::func_decl() {
   ast_node ast__(ast_function), ast_t(ast_failed);
   syntax_error.str("");
   
   if(lex_.peek_next_token().text.str() != "def") {
       return ast_t;
   }
   else {
       lex_.next_token();
   }
   
   if((ast_t = identifier()).atom != ast_failed)
       ast__.new_child( ast_t );
   else {
       syntax_error << "expected identifier before `" << lex_.tok.text.str() << "'.";
       parse_error();
       ast__.atom = ast_syntax_fail; 
       return ast__;
   }
   
   if(lex_.next_token().text.str() == "(") {
       ast_t.new_();
       ast_t.atom = ast_func_args;
       ast_t.value.str("(");
       
       ast__.new_child( ast_t );
   }
   else {
       syntax_error << "expected '(' before `" << lex_.tok.text.str() << "'.";
       ast__.atom = ast_syntax_fail;
       return ast__;
   }
   
   if(lex_.peek_next_token().text.str() != ")") {
       if((ast_t = func_args()).atom != ast_failed) {
           ast__.childs().get( ast__.childs().size() - 1 )
             .childs() = ast_t.childs();
       }
       else  {
            ast__.atom = ast_syntax_fail; 
            return ast__;
       }
   }
   
   if(lex_.next_token().text.str() == ")") {
       ast_t.new_();
       ast_t.atom = ast_block_stm;
       ast_t.value.str("{");
       
       ast__.new_child( ast_t );
   }
   else  {
       syntax_error << "expected ')' before `" << lex_.tok.text.str() << "'.";
       parse_error();
       ast__.atom = ast_syntax_fail; 
       return ast__;
   }
   
   if((ast_t = block_stm()).atom != ast_failed) {
       if(ast_t.atom == ast_syntax_fail) {
           ast__.atom = ast_syntax_fail; 
           return ast__;
       }
       
       ast__.childs().get( ast__.childs().size() - 1 )
         .childs() = ast_t.childs();
   }
   else  {
       ast__.atom = ast_syntax_fail; 
       return ast__;
   }
   
   return ast__;
}

ast_node parser::declaration() {
     ast_node ast__;
     if(((ast__ = struct_decl()).atom != ast_failed)
        || ((ast__ = var_decl()).atom != ast_failed)
        || ((ast__ = func_decl()).atom != ast_failed))
        return ast__;
     else {
         ast__.atom = ast_failed;
         return ast__;
     }
     return ast__;
}

ast parser::base_grammar() {
    ast prog_ast;
    prog_ast.construct();
    
    for( ;; )
    {
        ast_node ast__ = declaration();
        if(ast__.atom == ast_failed) {
            if(lex_.end())
              break;
            else {
                syntax_error << "expected node, class, function, or variable decliration.";
                parse_error();
                
                cout << errno.str();
                // error
                break;
            }
        }
        else {
            if(ast__.atom == ast_syntax_fail) {
              cout << "failure" << endl;
              cout << errno.str() << endl;
              break;
            }
            
            cout << "success" << endl;
            break;
            if(ast__.atom == ast_syntax_fail){}
              // error
            else
              prog_ast.new_member( ast__ ); // we have our ast members
        }
    }
    
    return prog_ast;
}
