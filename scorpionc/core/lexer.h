/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for lexing a scorpion 
* source file
* 
*/

#ifndef lexer_h
#define lexer_h

#include <string>
#include "string.h"
#include "array_list.h"

using namespace std;

enum // token_kinds
{
   k_unk    = 0, k_eof    = 1, k_symbol = 2,
   k_number = 3, k_string = 4, k_char   = 5,
   k_sign   = 6, k_line   = 7, k_hex_error = 8,
   k_number_error = 9, k_hex_number = 10, 
   k_string_error = 11, k_char_error = 12,
   k_unterm_com = 13
};

enum // lex_modes
{
  mode_ignore_strays = 0x1, mode_ignore_errors = 0x2
};

#define MODE(m) (1 & (mode >> m))
#define SET_MODE(m,x) ((mode) ^= (-x ^ mode) & (1 << m)) 

struct token
{
   sstring text;
   int col, kind, line;
   bool is_float;
   void clear()
   {
       text.clear();
       col = 0;
       kind = 0;
       line = 0;
       is_float = false;
   }
};

/* lexer helper functions */
bool is_whitespace(const char c);
bool is_operator_char(const char c);
bool is_letter(const char c);
bool is_digit(const char c);
bool is_letter_or_digit(const char c);
bool is_left_bracket(const char c);
bool is_right_bracket(const char c);
bool is_bracket(const char c);
bool is_sign(const char c);
bool c_match(const char c, const char d);
      
class lexer 
{
   public:
     token tok;
     unsigned long iter;
     unsigned long line, col, mode;
     void init(std::string source, int lex_mode);
     token next_token(bool safe = false);
     token peek_next_token();
     void set_mode(int m, bool state);
     bool end();
     void back();
     void clear();
     
   private:
     bool is_end();
     bool token_end();
     void skip_whitespace();
     void skip_comments();
     void parse_operator();
     void parse_symbol();
     void parse_number();
     void parse_string();
     void parse_char();
     void parse_hex_number(unsigned long start);
     void add_token(unsigned long begin, unsigned long end, int kind, bool is_float = false);
     
     sstring source;
     ArrayList<token> token_list_;
};

#endif // lexer_h
