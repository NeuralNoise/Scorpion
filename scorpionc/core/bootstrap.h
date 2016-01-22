/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for bootstrapping the 
* compiler
* 
*/
#ifndef bootstrap_h
#define bootstrap_h

#include <stdint.h>
#include <string>
#include <sstream>
#include "string.h"
#include "sio.h"
#include "array_list.h"
#include "bootstrap_core.h"

struct base_header  /* struct for the base contents of libs header */
{
    uint64_t file_size;
    sstring source_files;
};

struct c_status   /* internal bootstrap compiler */
{
    unsigned int errors, 
          warnings;
};

struct source_item  /* struct for containing source files */
{
    sstring src, import,
          name, package;
};

struct bootstrap   /* struct for containing bootstrap data */
{
    int p;  // wether this is allocated
    c_status status;
    base_header lib_header;
    stringstream __eso_buf__, c_buf__;
    
    source_item* source_items;
    
    long fp, main_class_id;     // pointer to which file we are on
    ArrayList<file_state> file_states;
    bool has_main, has_main_class;
    
    uint64_t function_uptr, object_uptr, class_id;
    
    // Bootstrapper heap
    ArrayList<ClassObject> class_list;
    ArrayList<Object> object_list;
    ArrayList<Method> function_list;
    
    
    ArrayList<ClassObject> class_lookup_list; // we use this for nesting and method verificaton stuff
};

/*********** Main compiler bootsrap method ***********
 * We call ths function to get the sourcecode ready for 
 * compilation
 * ***************************************************
*/
int init_core_bootstrap(BIO* io);
int init_core_bootstrap_start();
void core_bootstrap_push(BIO* io);
void core_bootstrap_clean();
void core_exit(int e);
int extract_lib(std::string lib, BIO* io);
void core_print_compilation_status();
int get_s_item(string name);
file_state& get_file();

extern bootstrap core;

#endif // bootstrap_h