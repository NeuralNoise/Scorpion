/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for parsing a scorpion 
* source file
* 
*
#include "grammar.h"
#include "bootstrap_core.h"
#include "parser.h"

ArrayList<sstring> rule_ids, udef_rules; // defined and undefined rules

rule grammar_rule;
int flag = 0, rule_ptr = 0, sub_rule=0, has_rule=0;
void new_rule();

void rule_cpy(rule& r, rule& r2)
{
    r.flags = r2.flags;
    r.name = r2.name;
    r._rule = r2._rule;
    r.bind = r2.bind;
}

void bind_rule(grammar& g)     
{
    grammar_rule.flags = flag;
    if(rule_ptr == 0)
    {
        rule_ptr++;
        rule_cpy(g.grammar_rule , grammar_rule);
        // cout << "adding inital rule\n";
    }
    else 
    {
        if(sub_rule != 0)
        {
            g.grammar_rule.binded_rules.get( sub_rule - 1 )
               .binded_rules.add( grammar_rule );
            // cout << "\t\tadding sub rule in parent_bind: " << rule_ptr << " sub_bind " << sub_rule << endl;
        }
        else
        {
            rule_ptr++;
            g.grammar_rule.binded_rules.add( grammar_rule );
            // cout << "\tadding regular rule in parent_bind: " << rule_ptr << endl;
        }
    }
    new_rule();
}

void new_rule()
{
    grammar_rule.flags = 0;
    grammar_rule.name.str("");
    grammar_rule._rule.str("");
    grammar_rule.bind = ' ';
    grammar_rule.binded_rules.clear();
    has_rule = 0;
}

bool grammar_set(std::string name)
{
    for(unsigned long i = 0; i < rule_ids.size(); i++)
    {
        if(rule_ids.get( i ).str() == name)
          return true;
    }
    return false;
}
            
bool grammar::grammar_parser::parse(std::string gram, grammar& grammars)
{
    token t;
    sstring name;
    stringstream ss;
    bool ok = true;
    
    int lex_modes = (1 << mode_ignore_strays) | 
           (1 << mode_ignore_errors);
           
    name.hash_start();
    grammar_lexer.init(gram, lex_modes);
    t = grammar_lexer.next_token();
    
    name.str(t.text.str());
    if(t.kind != k_symbol)
    {
      cout << "grammar_parser:  error: expected IDENTIFIER before `" << t.text.str() << "'.\n";    
      return false;
    }
    else if(grammar_set(name.str()))
    {
      cout << "grammar_parser:  error: rdefinition of grammar rule `" << name.str() << "'.\n";    
      return false;
    }
    
    t = grammar_lexer.next_token();
    ss << t.text.str() << grammar_lexer.next_token().text.str();
    if(ss.str() != ":=")
    {
       cout << "grammar_parser:  error: expected ASSIGNMENT_OPERATOR before `" << ss.str() << "'.\n";    
       return false;
    }
    
    //cout << "new grammar " << name.str() << endl;
    rule_ids.add( name );
    
    rule_ptr = 0;
    sub_rule = 0;
    bool require_reset = false;
    bool flag_set = false;
    bool bind_set = false;
    bool has_identifier = false;
    
    for( ;; )
    {
        t = grammar_lexer.next_token(true);
        if(t.kind == k_eof)
          break;
          
        if(t.text.str() == "@")
        {
            flag_set = false;
            if(require_reset)
            {
                ok = false;
                cout << "grammar_parser:  error: expected '|', '&', or '!' before `" << t.text.str() << "'.\n";    
                break;
            }
            
            t = grammar_lexer.next_token();
            if(!binder(t.text.str()))
            {
                ok = false;
                cout << "grammar_parser:  error: expected FUNCTION_ID before `" << t.text.str() << "'.\n";    
                break;
            }
            
            bind_set = false;
            grammar_rule._rule = t.text;
            set_flag(flag_function, 1, flag);
            has_rule = 1;
        }
        else if(t.text.str() == "+" || t.text.str() == "-" || t.text.str() == "*")
        {
            if(flag_set)
            {
                ok = false;
                cout << "grammar_parser:  error: expected IDENTIFIER or FUNCTION_ID before `" << t.text.str() << "'.\n";    
                break;
            }
            
            bind_set = false;
            if(t.text.at(0) == '+')
              set_flag(flag_plus, 1, flag);
            if(t.text.at(0) == '-')
              set_flag(flag_min, 1, flag);
            if(t.text.at(0) == '*')
              set_flag(flag_mult, 1, flag);
            flag_set = true;
            has_rule = 1;
        }
        else if(t.text.str() == "|" || t.text.str() == "&" || t.text.str() == "!"
            || t.text.str() == ">>")
        {
            if(bind_set)
            {
                ok = false;
                cout << "grammar_parser:  error: expected IDENTIFIER or FUNCTION_ID or OPERATOR before `" << t.text.str() << "'.\n";    
                break;
            }
            
            if(t.text.at(0) == '|')
              grammar_rule.bind = (int)'&';
            if(t.text.at(0) == '&' || t.text.str() == ">>")
              grammar_rule.bind = (int)'&';
            if(t.text.at(0) == '!')
              grammar_rule.bind = (int)'!';
            bind_rule(grammars);  
              
            flag = 0;
            bind_set = true;
            require_reset = false;
            has_identifier = false;
        }
        else if(t.kind == k_symbol)
        {
            bind_set = false;
            flag_set = false;
            
            if(has_identifier)
            {
                ok = false;
                cout << "grammar_parser:  error: expected FUNCTION_ID or OPERATOR before `" << t.text.str() << "'.\n";    
                break;
            }
            
            if(!grammar_set(t.text.str()))
                udef_rules.add( t.text );
            
            grammar_rule.name = t.text;
            set_flag(flag_function, 0, flag);
            has_identifier = true;
            has_rule = 1;
        }
        else if(t.text.str() == ")" || t.text.str() == "(")
        {
            flag_set = false;
            if(t.text.at(0) == '(')
            {
                sub_rule++;
                set_flag(flag_sub_rule, 0, flag);
                grammars.grammar_rule.binded_rules.add( grammar_rule );
                new_rule();
                
                grammars.grammar_rule.binded_rules.get( sub_rule - 1 )
                    .binded_rules.list_start();
            }
            else
            {
                if(sub_rule == 0)
                {
                    ok = false;
                    cout << "grammar_parser:  error: expected '(' before `" << t.text.str() << "'.\n";    
                    break;
                }
                else
                {
                    if(has_rule)
                      bind_rule(grammars);  
                    sub_rule--;
                }
            }
        }
        else 
        {
            ok = false;
            cout << "grammar_parser:  error**: expected IDENTIFIER or FUNCTION_ID or OPERATOR before `" << t.text.str() << "'.\n";    
            break;
        }
    }
    
    if(has_rule == 1) bind_rule(grammars);  
    return ok;
}

void grammar::set_grammar(sstring g)
{
    gram = g;
    error_list.list_start();
    if(!parse(gram.str(), *this))
    {
        cout << "bad grammar!\n";
        core_exit(1);
    }
}

void grammar_init()
{
   sstring gram;
   gram.hash_start();
   rule_ids.list_start();
   udef_rules.list_start();
   
   grammar_rule.flags = 0;
   grammar_rule.name.hash_start();
   grammar_rule._rule.hash_start();
   grammar_rule.bind = ' ';
   grammar_rule.binded_rules.list_start();
   
   gram.str(
          "identifier := @identifier"
          "\n"
   );
   identifier.set_grammar( gram );
   
   gram.str(
          "float_literal := @float_literal"
          "\n"
   );
   float_literal.set_grammar( gram );
   
   gram.str(
          "int_literal := @integer_literal"
          "\n"
   );
   int_literal.set_grammar( gram );
   
   gram.str(
          "string_literal := @string_literal"
          "\n"
   );
   string_literal.set_grammar( gram );
   
   gram.str(
          "char_literal := @char_literal"
          "\n"
   );
   char_literal.set_grammar( gram );
  
   gram.str(
          "bool_literal := @bool_literal"
          "\n"
   );
   bool_literal.set_grammar( gram ); 
   
   gram.str(
          "object_type := @object"
          "\n"
   );
   object_type.set_grammar( gram );
   
   gram.str(
          "int_type := @int"
          "\n"
   );
   int_type.set_grammar( gram );
   
   gram.str(
          "float_type := @float"
          "\n"
   );
   float_type.set_grammar( gram );
   
   gram.str(
          "boolean_type := @bool"
          "\n"
   );
   bool_type.set_grammar( gram );
   
   gram.str(
          "string_type := @string"
          "\n"
   );
   string_type.set_grammar( gram );
   
   gram.str(
          "character_type := @char"
          "\n"
   );
   char_type.set_grammar( gram );
   
   gram.str(
          "id_type := identifier"
          "\n"
   );
   id_type.set_grammar( gram );
   
   gram.str(
          "type := int_type"
          "      | float_type "
          "      | bool_type "
          "      | string_type "
          "      | char_type "
          "      | id_type"
          "\n"
   );
   type.set_grammar( gram );
   
   gram.str(
          "begin_paren := @begin_paren"
          "\n"
   );
   begin_paren.set_grammar( gram );
   
   gram.str(
          "end_paren := @end_paren"
          "\n"
   );
   end_paren.set_grammar( gram );
   
   gram.str(
          "function_call := identifier "
          "               & begin_paren "
          "               & -( expression >> *(@comma & expression) )"
          "\n"
   );
   function_call.set_grammar( gram );
   
   gram.str(
          "member_access := identifier >> *(@dot & identifier)"
          "\n"
   );
   member_access.set_grammar( gram );
   
   gram.str(
          "literal_value := float_literal "
          "               | int_literal"
          "               | string_literal"
          "               | char_literal"
          "               | bool_literal"
          "\n"
   );
   literal_value.set_grammar( gram );
   
   gram.str(
          "value := function_call "
          "       | member_access "
          "       | @begin_paren >> expression >> @end_paren"
          "\n"
   );
   value.set_grammar( gram );
   
   gram.str(
          "logical_not_expr := @not >> unary_expr"
          "\n"
   );
   logical_not_expr.set_grammar( gram );
   
   gram.str(
          "positive_expr := @plus >> unary_expr"
          "\n"
   );
   positive_expr.set_grammar( gram );
   
   gram.str(
          "negative_expr := @minus >> unary_expr "
          "\n"
   );
   negative_expr.set_grammar( gram );
   
   gram.str(
          "unary_expr := logical_not_expr "
          "            | positive_expr "
          "            | negative_expr"
          "            | value"
          "\n"
   );
   unary_expr.set_grammar( gram );
   
   gram.str(
          "mul_op := @mult >> unary_expr"
          "\n"
   );
   mul_op.set_grammar( gram );
   
   gram.str(
          "div_op := @div >> unary_expr "
          "\n"
   );
   div_op.set_grammar( gram );
   
   gram.str(
          "mul_expr := unary_expr >> *(mul_op | div_op)"
          "\n"
   );
   mul_expr.set_grammar( gram );
   
   gram.str(
          "add_op := @plus >> mul_expr"
          "\n"
   );
   add_op.set_grammar( gram );
   
   gram.str(
          "sub_op := @minus >> mul_expr "
          "\n"
   );
   sub_op.set_grammar( gram );
   
   gram.str(
          "add_expr := mul_expr >> *(add_op | sub_op)"
          "\n"
   );
   add_expr.set_grammar( gram );
   
   gram.str(
          "lt_op := @lt >> add_expr"
          "\n"
   );
   lt_op.set_grammar( gram );
   
   gram.str(
          "lte_op := @lte_op >> add_expr "
          "\n"
   );
   lte_op.set_grammar( gram );
   
   gram.str(
          "gt_op := @gt >> add_expr "
          "\n"
   );
   gt_op.set_grammar( gram );
   
   gram.str(
          "gte_op := @gte_op >> add_expr "
          "\n"
   );
   gte_op.set_grammar( gram );
   
   gram.str(
          "cmp_expr := add_expr >> *(lt_op | lte_op | gt_op | gte_op) "
          "\n"
   );
   cmp_expr.set_grammar( gram );
   
   gram.str(
          "eq_op := @eq_eq >> cmp_expr "
          "\n"
   );
   eq_op.set_grammar( gram );
   
   gram.str(
          "diff_op := @neq >> cmp_expr"
          "\n"
   );
   diff_op.set_grammar( gram );
   
   gram.str(
          "eq_expr := cmp_expr >> *(eq_op | diff_op) "
          "\n"
   );
   eq_expr.set_grammar( gram );
   
   gram.str(
          "log_and_op := @log_and >> eq_expr "
          "\n"
   );
   log_or_op.set_grammar( gram );
   
   gram.str(
          "log_or_op := @log_or >> eq_expr"
          "\n"
   );
   function_call.set_grammar( gram );
   
   gram.str(
          "logical_expr := eq_expr >> *(log_or_op | log_or_op) "
          "\n"
   );
   logical_expr.set_grammar( gram );
   
   gram.str(
          "cond_op := @question >> cond_expr >> @col >> cond_expr "
          "\n"
   );
   cond_op.set_grammar( gram );
   
   gram.str(
          "cond_expr := logical_expr >> -cond_op "
          "\n"
   );
   cond_expr.set_grammar( gram );
   
   gram.str(
          "expression := cond_expr "
          "\n"
   );
   expression.set_grammar( gram );
   
   gram.str(
          "block_stm := @block_begin >> *statement >> block_end "
          "\n"
   );
   block_stm.set_grammar( gram );
   
   gram.str(
          "var_stm := var_def "
          "\n"
   );
   var_stm.set_grammar( gram );
   
   gram.str(
          "for_init := var_stm | assignment_stm "
          "\n"
   );
   for_init.set_grammar( gram );
   
   gram.str(
          "for_cond := expression "
          "\n"
   );
   for_cond.set_grammar( gram );
   
   gram.str(
          "for_step := assignment_stm "
          "\n"
   );
   for_step.set_grammar( gram );
   
   gram.str(
          "for_stm := @for >> @begin_paren >> -for_init >> @statement_end >> -for_cond "
          "               & @statement_end >> -for_step >> @end_paren >> block_stm"
          "\n"
   );
   for_stm.set_grammar( gram );
   
   gram.str(
          "while_stm := @while >> @begin_paren >> expression >> @end_paren >> block_stm"
          "\n"
   );
   while_stm.set_grammar( gram );
   
   gram.str(
          "else_if_stm := if_stm "
          "\n"
   );
   else_if_stm.set_grammar( gram );
   
   gram.str(
          "else_block_stm := block_stm "
          "\n"
   );
   else_block_stm.set_grammar( gram );
   
   gram.str(
          "if_stm := @if >> @begin_paren >> expression >> @end_paren >> block_stm"
          "               & -(@else >> (else_if_stm | else_block_stm)) "
          "\n"
   );
   if_stm.set_grammar( gram );
   
   gram.str(
          "assignment_stm := expression >> @eq >> expression "
          "\n"
   );
   assignment_stm.set_grammar( gram );
   
   gram.str(
          "expression_stm := expression "
          "\n"
   );
   expression_stm.set_grammar( gram );
   
   gram.str(
          "statement := block_stm"
          "           | var_stm"
          "           | for_stm"
          "           | while_stm"
          "           | if_stm"
          "           | return_stm"
          "           | assignment_stm"
          "           | expression_stm"
          "           | @statement_end"
          "\n"
   );
   statement.set_grammar( gram );
   
   gram.str(
          "var_inst := type >> identifier"
          "\n"
   );
   var_inst.set_grammar( gram );
   
   gram.str(
          "var_init := @eq >> expression "
          "\n"
   );
   var_init.set_grammar( gram );
   
   gram.str(
          "var_def := var_inst >> -var_init "
          "\n"
   );
   var_def.set_grammar( gram );
   
   gram.str(
          "var_decl := var_def "
          "\n"
   );
   var_decl.set_grammar( gram );
   
   gram.str(
          "struct_member := var_inst "
          "               | @statement_end"
          "\n"
   );
   struct_member.set_grammar( gram );
   
   gram.str(
          "struct_decl := @node >> identifier >> @block_begin >> *struct_member >> @block_end "
          "\n"
   );
   struct_decl.set_grammar( gram );
   
   gram.str(
          "func_decl := @def >> identifier >> @begin_paren >> "
          "              -(var_inst >> *(@comma >> var_inst)) >> @end_paren >> block_stm "
          "\n"
   );
   func_decl.set_grammar( gram );
   
   gram.str(
          "declaration := struct_decl"
          "             | var_decl"
          "             | func_decl"
          "             |  @statement_end"
          "\n"
   );
   declaration.set_grammar( gram );
   
   gram.str(
          "base_grammar := *declaration /* entire language grammar  "
          "\n"
   );
   base_grammar.set_grammar( gram );
}

ArrayList<sstring>& grammar::error_list_()
{ return error_list; }

bool accept(int bind, int a, int b)
{
    if(bind == '|')
      return ((a == 1) || (b == 1));
    else return false;
}

ast grammar::parse_grammar()
{
    error_check = false;
    error_list.clear();
    sstring str;
    str.hash_start();
    
    grammar_ast.destruct();
    grammar_ast.construct();
    
    
    ast_node node;
    node.ast_start();
    
    for(unsigned int i = 0; i < grammar_rule.binded_rules.size()+1; i++)
    {
        rule r = (i == 0) ? grammar_rule : grammar_rule.binded_rules.get( i - 1 );
        
        if(flag(flag_function, r.flags))
        {
            cout << "function!\n";
        }
        else if(flag(flag_sub_rule, r.flags))
        {
            cout << "sub_rule!\n";
        }
        else {
            cout << "rule!\n";
        }
    }
    
    cout << "\n\n";
    return grammar_ast;
}

bool binder(std::string s)
{
    return( s == "identifier" || s == "string_literal"
       || s == "integer_literal" || s == "float_literal"
       || s == "char_literal" || s == "bool_literal"
       || s == "object" || s == "int" || s == "float" 
       || s == "bool" || s == "char" || s == "string"
       || s == "begin_paren" || s == "end_paren"
       || s == "comma" || s == "dot" || s == "statement_end"
       || s == "block_begin" || s == "block_end" || s == "not"
       || s == "eq" || s == "plus" || s == "minus" || s == "mult"
       || s == "div" || s == "lt" || s == "gt"
       || s == "lte_op" || s == "gte_op" || s == "eq_eq"
       || s == "neq" || s == "log_and" || s == "log_or"
       || s == "question" || s == "col" || s == "for" || s == "while"
       || s == "node" || s == "if" || s == "else" || s == "def");
}

/**************** Grammar Binded functions ****************

bool grammar::identifier()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.kind == k_symbol);
}

bool grammar::float_literal()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.is_float);
}

bool grammar::integer_literal()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.kind == k_number);
}

bool grammar::string_literal()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.kind == k_string);
}

bool grammar::char_literal()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.kind == k_char);
}

bool grammar::bool_literal()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "true" || t.text.str() == "false");
}

bool grammar::object_type()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "object");
}

bool grammar::int_type()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "int");
}

bool grammar::float_type()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "float");
}

bool grammar::bool_type()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "bool");
}

bool grammar::char_type()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "char");
}

bool grammar::string_type()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "string");
}

bool grammar::begin_paren()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "(");
}

bool grammar::end_paren()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == ")");
}

bool grammar::comma()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == ",");
}

bool grammar::dot()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == ".");
}

bool grammar::statement_end()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == ";");
}

bool grammar::block_begin()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "{");
}

bool grammar::block_end()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "}");
}

bool grammar::not_()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "!");
}

bool grammar::eq()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "=");
}

bool grammar::plus()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "+");
}

bool grammar::minus()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "-");
}

bool grammar::mult()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "*");
}

bool grammar::divide()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "/");
}

bool grammar::lt()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "<");
}

bool grammar::gt()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == ">");
}

bool grammar::lte_op()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "<=");
}

bool grammar::gte_op()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == ">=");
}

bool grammar::eq_eq()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "==");
}

bool grammar::neq()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "!=");
}

bool grammar::log_and()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "&");
}

bool grammar::log_or()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "|");
}

bool grammar::question()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "?");
}

bool grammar::col()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == ":");
}

bool grammar::for_()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "for");
}

bool grammar::while_()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "while");
}

bool grammar::node()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "node");
}

bool grammar::if_()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "if");
}

bool grammar::else_()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "else");
}

bool grammar::def()
{
    token t;
    t = parser_stack.get( parser_stack.size() - 1 ).lexer_().next_token();
    
    return (t.text.str() == "def");
}


/**************** All grammar types ****************

grammar identifier;
grammar float_literal;
grammar int_literal;
grammar string_literal;
grammar char_literal;
grammar bool_literal;
grammar object_type;
grammar int_type;
grammar float_type;
grammar bool_type;
grammar string_type;
grammar char_type;
grammar id_type;
grammar type;
grammar begin_paren;
grammar end_paren;
grammar function_call;
grammar member_access;
grammar logical_not_expr;
grammar positive_expr;
grammar negative_expr;
grammar mul_op;
grammar div_op;
grammar add_op;
grammar sub_op;
grammar eq_op;
grammar diff_op;
grammar lt_op;
grammar lte_op;
grammar gt_op;
grammar gte_op;
grammar log_and_op;
grammar log_or_op;
grammar cond_op;
grammar var_stm;
grammar block_stm;
grammar for_init;
grammar for_cond;
grammar for_step;
grammar for_stm;
grammar while_stm;
grammar else_if_stm;
grammar else_block_stm;
grammar if_stm;
grammar print_stm;
grammar return_stm;
grammar assignment_stm;
grammar expression_stm;
grammar struct_decl;
grammar var_inst;
grammar var_init;
grammar var_def;
grammar var_decl;
grammar func_decl;
grammar literal_value;
grammar value;
grammar unary_expr;
grammar mul_expr;
grammar add_expr;
grammar cmp_expr;
grammar eq_expr;
grammar logical_expr;
grammar cond_expr;
grammar expression;
grammar statement;
grammar struct_member;
grammar declaration;
grammar base_grammar;
*/
