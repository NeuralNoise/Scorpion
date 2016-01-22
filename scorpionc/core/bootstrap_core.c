/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for bootstrapping the 
* compiler
* 
*/
#include "bootstrap_core.h"
#include "bootstrap.h"
#include "parser.h"
#include "zlib.h"
#include <stdlib.h> 
#include <iostream>

using namespace std;


void core_error(file_state& f_state){}
void core_warning(file_state& f_state, std::string type){}


void parse_source(stringstream &source)
{
    get_file().package.str("null");
    get_file().s_lexer.init( source.str(), get_file().lex_mode );
    source.str("");
    
    
    
    get_file().s_lexer.clear();
}

void run_bootstrap()
{
    ast_stack.list_start();
    parser_stack.list_start();
    
    for(unsigned int i = 0; i < core.lib_header.file_size; i++)
    {
        parser_stack.clear();
        printf("compiling %s\n", core.source_items[i].name.str().c_str());
        core.fp++;
        
        file_state new_state;
        new_state.state_start();
        new_state.name.str(core.source_items[i].name.str());
        
        core.file_states.add( new_state );
        
        stringstream source;
        Zlib zippy;
        zippy.Decompress_Buffer2Buffer(core.source_items[i].src.str(), source);
        
        if(zres.response == ZLIB_FAILURE){
             cout << "\n" << zres.reason.str() << "Error is not recoverable, exiting with exit status: -1.\n";
             core_exit(-1);
        }
        else if(zres._warnings_.str() != "")
            cout << zres._warnings_.str();
        
        zippy.Cleanup();
        parser new_parser(source.str());
        
        parser_stack.add( new_parser );
        new_parser.parse();
    }
}

