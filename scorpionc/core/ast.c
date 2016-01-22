/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for parsing a scorpion 
* source file
* 
*/
#include "ast.h"
#include <sstream>
void std_err_(string n);

void ast_node::ast_start()
{
    child_nodes.list_start();
    atom =ast_failed;
    value.hash_start();
}
       
ArrayList<ast_node>& ast_node::childs()
{
    return child_nodes;
}

void ast_node::new_child(ast_node& n)
{
    n.child_nodes.list_start();
    child_nodes.add( n );
}

void ast_node::new_()
{
    destruct();
    ast_start();
}

ast_node& ast_container::member(unsigned long ndx)
{
    if(ndx >= member_t)
    {
        stringstream ss;
        ss << "access to ast member " << ndx << " is invalid.";
        std_err_(ss.str());
    }
    
    return ast_members[ ndx ];
}

void ast_container::new_member(ast_node& node)
{
    ast_node* ast_ptr = NULL;
    ast_ptr = (ast_node*)malloc((++member_t)*sizeof(ast_node));
    if(ast_ptr == NULL)
        std_err_("failure to allocate internal memory for compiler.");
        
    for(unsigned long i = 0; i < member_t-1; i++)
        ast_ptr[i] = ast_members[i];
    
    ast_ptr[member_t-1] = node;
    if(ast_members != NULL) free( ast_members );
    ast_members = &ast_ptr[0];
}

void ast_node::destruct()
{
    atom =0;
    value.str("");
    child_nodes.clear();
}

void ast_container::destruct()
{
    if(ast_members != NULL)
    {
        for(unsigned int i = 0; i < member_t; i++)
           ast_members[i].destruct();
           
        free( ast_members );
        ast_members =NULL;
    }
    member_t =0;
}

void ast_container::construct()
{
   ast_members = NULL;
   member_t = 0;
}

ast& prog_ast::base_ast_()
{ return base_ast; }
