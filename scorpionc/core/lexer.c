/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for lexing a scorpion 
* source file
* 
*/
#include "lexer.h"
#include "bootstrap_core.h"
#include "bootstrap.h"
#include <sstream>
#include <ctype.h>
#include <iostream>

std::stringstream t;
void std_err_(string n);

bool is_whitespace(const char c)
{
     return (' '  == c) || ('\n' == c) ||
            ('\r' == c) || ('\t' == c) ||
            ('\b' == c) || ('\v' == c) ||
            ('\f' == c) ;
}

bool is_operator_char(const char c)
{
     return ('+' == c) || ('-' == c) ||
            ('*' == c) || ('/' == c) ||
            ('^' == c) || ('<' == c) ||
            ('>' == c) || ('=' == c) ||
            (',' == c) || ('!' == c) ||
            ('(' == c) || (')' == c) ||
            ('[' == c) || (']' == c) ||
            ('{' == c) || ('}' == c) ||
            ('%' == c) || (':' == c) ||
            ('?' == c) || ('&' == c) ||
            ('|' == c) || (';' == c) ||
            ('!' == c) || ('.' == c) ;
}

bool is_letter(const char c)
{
    return (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z'));
}

bool is_digit(const char c)
{
    return ('0' <= c) && (c <= '9');
}

bool is_letter_or_digit(const char c)
{
    return is_letter(c) || is_digit(c);
}

bool is_left_bracket(const char c)
{
    return ('(' == c) || ('[' == c) || ('{' == c);
}

bool is_right_bracket(const char c)
{
    return (')' == c) || (']' == c) || ('}' == c);
}

bool is_bracket(const char c)
{
    return is_left_bracket(c) || is_right_bracket(c);
}

bool is_sign(const char c)
{
    return ('+' == c) || ('-' == c);
}


bool c_match(const char c1, const char c2)
{
 return std::tolower(c1) == std::tolower(c2);
}

bool comment_start(char a, char b, int& mode)
{
    if(a == '/')
    {
        if ('/' == b) { mode = 1; }
        else if ('*' == b) { mode = 2; }
    }
    return (mode != 0);
}

bool comment_end(char a, char b, int mode)
{
    return ((1 == mode) && ('\n' == a)) ||
            ((2 == mode) && ( '*' == a) && ('/' == b));
}

void lexer::skip_comments()
{
    if(is_end() || ((iter+1) > source.size()))
      return;
    
    token t;
    int c_mode=0, closed=0;  
    if(!comment_start(source.at(iter), source.at(iter+1), c_mode))
      return;
    iter+=2;
    
    while(!is_end())
    {
        col++;
        if(comment_end(source.at(iter), source.at(iter+1), c_mode))
        {
            closed = 1;
            if(c_mode == 1)
            {
                line++;
                col=0;
            }
            
            iter+=c_mode;
            break;
        }
        
        if(source.at(iter) == '\n')
        {
            line++;
            col=0;
        }
        iter++;
    }
    
    if(closed==0)
    {
        t.text.hash_start();
        t.kind = k_unterm_com;
        token_list_.add( t );
    }
    
    if(!is_end())
    {
         skip_whitespace();
         skip_comments();
    }
}

void lexer::skip_whitespace()
{
    while(!is_end() && is_whitespace(source.at( iter )))
    {
        col++;
        if(source.at( iter ) == '\n')
        {
            line++;
            col=0;
        }
        iter++;
    }
}

void lexer::add_token(unsigned long begin, unsigned long end, int kind, bool is_float)
{
    token temp_t;
    unsigned long temp_iter = begin;
    temp_t.text.hash_start();
    
    temp_t.kind = kind;
    temp_t.col = col;
    temp_t.line = line;
    temp_t.is_float = is_float;
    t.str("");
    
    while(temp_iter<end)
       t << source.at( temp_iter++ );
    
    // cout << "adding token " << t.str() << " line " << line << " col " << col << " begin " << begin << "end " << end << endl;
    temp_t.text.str( t.str() );
    token_list_.add( temp_t );
}

void lexer::parse_char()
{
    unsigned long begin = ++iter;
    
    if(is_end()) add_token(begin, iter, k_char_error);
    if(source.at( iter ) == '\\')
    {
        iter+=2;
        if(is_end()) add_token(begin, iter, k_char_error);
        if(source.at( iter ) == '\'')
           add_token(begin, iter, k_char);
        else add_token(begin, iter, k_char_error);
    }
    else
    {
        if(source.at( ++iter ) == '\'')
           add_token(begin, iter, k_char);
        else add_token(begin, iter, k_char_error);
    }
    iter++;
}

void lexer::parse_string()
{
     bool terminated = false;
     bool escaped = false;
     unsigned long begin = ++iter;
     
     while(!is_end() || source.at( iter ) != '"')
     {
        if (!escaped && ('\\' == source.at( iter )))
        {
           escaped = true;
           iter++;
           continue;
        }
        else if (!escaped)
        {
           if ('"' == source.at( iter ))
           {
               terminated = true;
               break;
           }
        }
        else if (escaped)
           escaped = false;
           
        iter++;
        col++;
        if(source.at( iter ) == '\n')
        {
            line++;
            col=0;
        }
     }
     
     col++;
     if(!terminated)
        add_token(begin, iter, k_string_error);
     else
        add_token(begin, iter, k_string);
     iter++;
}
     
void lexer::parse_hex_number(unsigned long start) // use strtol(str, NULL, 16);
{
     bool has_digit = false;
     unsigned long begin = start;
     
     while(!is_end())
     {
         if(is_letter(source.at( iter )) || is_digit(source.at( iter )))
         {
             if( is_letter(source.at( iter )) && 
                !((('a' <= source.at( iter )) && (source.at( iter ) <= 'f')) 
                  || ((('A' <= source.at( iter ))) && (source.at( iter ) <= 'F'))) )
             {
                 add_token(begin, iter, k_hex_number);
                 return;
             }
             col++;
             iter++;
             has_digit = true;
         }
         else
         {
             if(!has_digit)
               add_token(begin, iter, k_hex_error);
             else
               add_token(begin, iter, k_hex_number);
             return;
         }
     }
}

void lexer::parse_number()
{
     unsigned long begin    = iter;
     bool dot_found         = false;
     bool e_found           = false;
     bool post_e_sign_found = false;
     bool negative          = false;
     bool needs_number      = false;
     
     char a = source.at( iter ), b = source.at( iter+1 );
     if(a == '-')
     {
       iter++;
       col+=2;
       a = source.at( iter );
       b = source.at( ++iter );
       iter--;
     }
     
     if(a == '0' && b == 'x')
     {
         iter+=2;
         parse_hex_number(begin);
         return;
     }
     
     
     if(negative) iter--;
     while(!is_end())
     {
         if(source.at( iter ) == '.')
         {
             if(dot_found)
                break;
             
             dot_found = true;
             iter++;
             col++;
             continue;
         }
         else if(c_match('e', source.at( iter )))
         {
             col++;
             char c = source.at( ++iter );
             
             if(is_end() || (('+' != c) &&
               ('-' != c) && !is_digit(c)))
             {
                add_token(begin, iter, k_number_error);
                return;
             }
             
             col++;
             iter++;
             needs_number = true;
             continue;
         }
         else if(e_found && is_sign(source.at( iter )))
         {
              if (post_e_sign_found)
              {
                add_token(begin, iter, k_number_error);
                return;
              }

              post_e_sign_found = true;
              iter++;
              col++;
              continue;
         }
         else if(source.at( iter ) != '.' && !is_digit(source.at( iter )))
           break;
         else
         {
             iter++;
             col++;
             if(needs_number)
             {
                 if(is_digit(source.at( iter )))
                   needs_number = false;
             }

         }
     }
    
    if(needs_number)
      add_token(begin, iter, k_number_error);
    else
      add_token(begin, iter, k_number);
}

void lexer::parse_symbol()     
{
    unsigned long begin = iter;
    while(!is_end() && (is_letter_or_digit(source.at( iter )) 
        || source.at( iter ) == '_'))
    {
        iter++;
        col++;
    }
    
    add_token(begin, iter, k_symbol);
}
     
void lexer::parse_operator()
{
    token temp_t;
    temp_t.kind = -1;
    
    char a = source.at( iter ),
         b = source.at( iter+1 );

    if (((a == '<') && (b == '=')) || ((a == '>') && (b == '='))
       || ((a == '!') && (b == '=')) || ((a == '=') && (b == '='))
       || ((a == '<') && (b == '<'))
       || ((a == '>') && (b == '>')) || ((a == '*') && (b == '*'))) temp_t.kind = k_sign;
    
    if(temp_t.kind != -1)
    {
        col+=2;
        add_token(iter, iter+2, k_sign);
        iter+=2;
        return;
    }
    
    col++;
    if(a == '-')
    { 
        if(is_digit(b))
        {
            parse_number();
            return;
        }
    }
    add_token(iter, iter+1, k_sign);
    iter++;
}

bool lexer::is_end()
{ return (iter >= (source.size()-1)); }

bool lexer::token_end()
{ return (iter >= token_list_.size()); }

bool lexer::end()
{ return token_end(); }

void lexer::back()
{ iter--; }
     
token lexer::next_token(bool safe)
{ 
    token temp_t;
    if(token_end())
    {
        if(!safe)
        {
            core.c_buf__ << "unexpected end of file.";
            core_error( get_file() );
        }
        line = token_list_.get( token_list_.size() - 1 ).line;
        tok = token_list_.get( token_list_.size() - 1 );
        return token_list_.get( token_list_.size() - 1 );
    }
    
    temp_t = token_list_.get( iter++ );
    line = temp_t.line;
    col = temp_t.col;
    if(!MODE(mode_ignore_strays) && temp_t.kind == k_unk)
    {
        core.c_buf__ << "stray `" << (int)temp_t.text.at(0) << "' in file.";
        core_error( get_file() );
        return next_token(safe);
    }
    
    if(!MODE(mode_ignore_errors))
    {
         if(temp_t.kind == k_hex_error)
         {
             temp_t.kind = k_hex_number;
             core.c_buf__ << "malformed hexadeciaml literal '" << temp_t.text.str() << "'.";
             core_error( get_file() );
         }
         else if(temp_t.kind == k_number_error)
         {
             temp_t.kind = k_number;
             core.c_buf__ << "malformed numeric literal '" << temp_t.text.str() << "'.";
             core_error( get_file() );
         }
         else if(temp_t.kind == k_string_error)
         {
             temp_t.kind = k_string;
             core.c_buf__ << "unterminated string literal.";
             core_error( get_file() );
         }
         else if(temp_t.kind == k_char_error)
         {
             temp_t.kind = k_char;
             core.c_buf__ << "unterminated char literal.";
             core_error( get_file() );
         }
         else if(temp_t.kind == k_unterm_com)
         {
             core.c_buf__ << "unclosed comment.";
             core_error( get_file() );
             return next_token(safe);
         }   
    }
    
    tok = temp_t;
    return temp_t;
}

token lexer::peek_next_token()
{
    token temp_t;
    int mode_t = mode;
    unsigned long iter_t = iter, line_t = line, col_t = col;
    
    
    SET_MODE(mode_ignore_errors, 1);
    SET_MODE(mode_ignore_strays, 1);
    
    temp_t = next_token(true);
    mode = mode_t;
    iter = iter_t;
    line = line_t;
    col = col_t;
    return temp_t;
}

void lexer::set_mode(int m, bool state)
{
    SET_MODE(m,state);
}   

void lexer::clear()
{
    iter = 0;
    while(iter<token_list_.size())
        token_list_.get( iter++ ).clear();
    token_list_.clear();
    source.clear();
}

void lexer::init(std::string src, int lex_mode)
{
    source.hash_start();
    if(source.str(src) != 0)
        std_err_("could not prepare file for lexing.\n");
    
    mode = lex_mode;
    iter=0;
    line=1;
    col=0;
    token_list_.list_start();
    for(;;)
    {
         skip_whitespace();
         skip_comments();
         
         if(is_end())
           break;
         else if(is_operator_char(source.at( iter )))
           parse_operator();
         else if(is_letter(source.at( iter )) || source.at( iter ) == '_')
           parse_symbol();
         else if(is_digit(source.at( iter )) || source.at( iter ) == '-')
           parse_number();
         else if(source.at( iter ) == '"')
           parse_string();
         else if(source.at( iter ) == '\'')
           parse_char();
         else
         {
            col++;
            add_token(iter, iter+1, k_unk);
            iter++;
         }
    }
    
    token t_eof;
    t_eof.text.hash_start();
    t_eof.text.str("eof");
    t_eof.col = 0;
    t_eof.kind = k_eof;
    t_eof.is_float = false;
    token_list_.add( t_eof );
    iter= 0;
    line=0;
    col=0;
}
