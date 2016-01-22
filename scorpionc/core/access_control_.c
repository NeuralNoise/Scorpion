/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for providing access control
* to Scorpion objects, classes, and node structs
* 
*/
#include "access_control_.h"
#include "bootstrap.h"
#include "bootstrap_core.h"
#include <string>

using namespace std;

void parse_access_spec(access_priv& priv)
{
    int w = 0;
    token t;
    t = get_file().s_lexer.next_token();
        
    for(int i = 0; i < ACCESS_SPEC_SIZE; i++)
    {
        if(t.text.str() == access_spec_itstr(i))
        {
            if(i <= access_protected && priv.accessibility != 0)
            {
                if(w == 0)
                {
                    core.c_buf__ << "multiple access specifiers not allowed `" << t.text.str() << "'.";
                    core_warning(get_file(), "access-control");
                }
                w = 1;
                SET_ASPEC(i, 0, priv.accessibility);
                t = get_file().s_lexer.next_token();
                continue;
            }
            else
            {
                SET_ASPEC(i, 1, priv.accessibility);
                t = get_file().s_lexer.next_token();
            }
        }
    }
    get_file().s_lexer.back();
}

string access_spec_strinfo(int a)
{
    stringstream ss;
    
    if(ACCESS_SPEC(access_public, a))
       ss << "public ";
    else if(ACCESS_SPEC(access_private, a))
       ss << "private ";
    else ss << "protected ";
    if(ACCESS_SPEC(access_static, a))
       ss << "static ";
    if(ACCESS_SPEC(access_read_only, a))
       ss << "const";
    return ss.str();
}

string access_spec_itstr(int a)
{
    switch( a )
    {
        case access_public:
           return "public";
        case access_private:
           return "private";
        case access_protected:
           return "protected";
        case access_read_only:
           return "const";
        case access_static:
           return "static";
        default:
           return "public";
    }
}
