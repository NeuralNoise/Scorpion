/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for bootstrapping the 
* compiler
* 
*/
#include "bootstrap.h"
#include "bootstrap_core.h"
#include "binary.h"
#include "zlib.h"
#include <stdlib.h>
#include <sstream>
#include <iostream>

using namespace std;

/* This is the offical compiler bootstrap */
bootstrap core;
sstring core_lib;

extern void std_err_(string n);

void core_print_compilation_status()
{
    sstring succes;
    succes.hash_start();
    
    succes.str((core.status.errors == 0) ? "Succeeded" : "Failed");
    printf("\n===============================================================\n");
    printf("  Bootstrap Compilation %s : Error(s): %u Warning(s): %u\n", succes.str().c_str(), 
         core.status.errors, core.status.warnings);
    printf("===============================================================\n");
}

int init_core_bootstrap(BIO* io)
{
    core.p = 0;
    core.fp = -1;
    core.has_main = false;
    core.has_main_class = false;
    core.status.errors = 0;
    core.status.warnings = 0;
    core.source_items = NULL;
    core.lib_header.file_size = 0;
    core.function_uptr = 0;
    core.object_uptr = 0;
    core.class_id = 0;
    core.main_class_id = -1;
    core.file_states.list_start();
    core.class_lookup_list.list_start();
    core.class_list.list_start();
    core.object_list.list_start();
    core.function_list.list_start();
    core.lib_header.source_files.hash_start();
    core_lib.hash_start();
    core.__eso_buf__.str("");
    core.c_buf__.str("");
    
    if( extract_lib("/usr/share/scorpion/lib/compiler-scorpionc.sar", io) != 0 )
       return 1;
    return 0;
}

int init_core_bootstrap_start()
{
    run_bootstrap();
    core_print_compilation_status();
    return core.status.errors;
}

void core_bootstrap_push(BIO* io)
{
    if(buffS_fill( core.__eso_buf__.str(), io ) != buf_ok)
       std_err_("internal err, could not fill bootstrap file to output stream.");
    core.__eso_buf__.str("");
    
    if(buffS_write( "/usr/share/scorpion/lib/scorpionc", io ) != buf_ok)
       std_err_("could not write to executable file.");
}

file_state& get_file()
{
    return core.file_states.get( core.fp );
}

int read_c(unsigned long* ndx, bool safe_mode = true)
{
    if((*ndx) >= core_lib.size())
    {
        if(!safe_mode)
          std_err_("unexpected end of file in sar library.");
        else return -355;
    }
    return core_lib.at( (*ndx)++ );
}

string read_str(unsigned long* ndx)
{
    stringstream ss;
    char c;
    
    for( ;; )
    {
        c = read_c( ndx, false );
        if(c == 0x0) break;
        ss << c;
    }
    return ss.str();
}

/**
 * remove the '\' or '/' from a file name and replace it
 * with '.'
 * 
 * This will be used to quickly import files from the 
 * source list
 */
sstring* splash_file(string f)
{
    sstring* strings;
    int pushback=0, stop=0, iter=0;
    
    strings = (sstring*)malloc(2*sizeof(sstring));
    if(strings == NULL) return NULL;
    
    strings[0].hash_start();
    strings[1].hash_start();
    for(unsigned int i = f.size() - 1; i > 0; i--)
    {
        if(f.at(i) == '.')
           break;
        else
          pushback++;
    }
    
    stringstream ss;
    for(unsigned int i = 0; i < f.size() - pushback - 1; i++)
    {
        if(f.at(i) == '/' || f.at(i) == '\\'){
            ss << ".";
            stop++;
        }
        else
          ss << f.at(i);
    }
    strings[0].str(ss.str());
    
    stop--;
    ss.str("");
    for(unsigned int i = 0; i < strings[0].size(); i++)
    {
        if(strings[0].at(i) == '.')
        {
            if(!(iter++ < stop)) break;
            ss << ".";
        }
        else
          ss << strings[0].at(i);
    }
    
    strings[1].str(ss.str());
    return strings;
}

int file_content_setup()
{
    unsigned int iter = 0;
    stringstream file;
    
    for(unsigned int i = 0; i < core.lib_header.source_files.size(); i++)
    {
        if(core.lib_header.source_files.at( i ) == ';')
        {
            if(file.str() != "")
            {
                sstring* includes;
                if(iter >= core.lib_header.file_size)
                   std_err_("sar file has more source files than expected ammount.");
                core.source_items[iter].name.hash_start();
                core.source_items[iter].import.hash_start();
                core.source_items[iter].package.hash_start();
                
                core.source_items[iter].name.str( file.str() );
                includes = splash_file( file.str() );
                if(includes == NULL)
                  std_err_("could not allocate resources while extracting library.");
                
                core.source_items[iter].import.str( includes[0].str() );
                core.source_items[iter++].package.str( includes[1].str() );
                file.str("");
            }
        }
        else
          file << core.lib_header.source_files.at( i );
    }
    
    if(iter != core.lib_header.file_size)
       std_err_("source file size in sar file does not match expected ammount.");
                   
    return 0;
}

int get_s_item(string name)
{
    for(unsigned int i = 0; i < core.lib_header.file_size; i++)
    {
        if(core.source_items[i].name.str() == name)
          return i;
    }
    return -1;
}

int extract_lib(std::string lib, BIO* io)
{
    
    int io_response= buffS_read( lib.c_str(), io );
    unsigned long iter=0;
    if(io_response != buf_ok)
    {
        buff_leak( io ); // free io buffer
        if(io_response == buf_trunk)
           std_err_("file truncated, file is not readable.");
        else if(io_response == buf_fail)
        {
           stringstream ss;
           ss << "cannot read file `" << lib << "':  No such file.";
           std_err_(ss.str());
        }
    }
    
    if(core_lib.str( buf_data( io ).str() ) != 0)
      std_err_("could not pull file from input stream, file too large!.");
    buff_leak( io );
    
    if(!(read_c( &iter ) == 0x0 && read_str( &iter ) == "SAR     "))
       std_err_("invalid char found in file, file format err.");
    iter +=10;
    
    if(!(read_c( &iter ) == 0x2b && read_c( &iter ) == 0x33 &&
        read_c( &iter ) == 0x00 && read_c( &iter ) == 0x4d))
       std_err_("invalid magic number in file, file format err.");
    
    int header_tracker=0,sizeof_header=5,flag=0;
    for( ;; )
    {
        flag =  read_c( &iter , false );
        header_tracker++;
        switch( flag )
        {
            case 0x1: {
                iter+=3;
                continue;
            }
            case 0x2: {
                iter+=3;
                continue;
            }
            case 0x3: {
                stringstream __ostream_buf__;
                Binary::decode_str(read_str( &iter ), __ostream_buf__);
                core.lib_header.file_size = atol( __ostream_buf__.str().c_str() );
                continue;
            }
            case 0x4: {
                core.lib_header.source_files.str( read_str( &iter ) );
                continue;
            }
            case 0x7: {
                break;
            }
            default: {
                stringstream ss;
                ss << "Invalid flag found in sar library " << flag << endl;
                std_err_( ss.str() );
            }
        }
        if(flag == 0x7) break;
    }
    
    iter+=33;
    if(header_tracker!=sizeof_header)
       std_err_("header in sar file could not be processed correctly, file format err.");
    if(core.lib_header.file_size == 0)
       std_err_("filesize in sar file cannot be 0.");
    
    stringstream __buf__,__ostream_buf__;
    int c;
    Zlib zlib;
    
    while ( (c = read_c( &iter )) != -355 )
         __buf__ << (char)c;
    core_lib.str("");
    
    zlib.Decompress_Buffer2Buffer(__buf__.str(), __ostream_buf__);
    __buf__.str("");
    
    if(core_lib.str( __ostream_buf__.str() ) != 0)
      std_err_("could not pull decompressed file from input stream, file too large!.");
    __ostream_buf__.str("");
    
    if(zres._warnings_.str() != "")
        cout << zres._warnings_.str();
           
    if(zres.response == ZLIB_FAILURE){
        cout << "\n" << zres.reason.str() << "Error is not recoverable, exiting with exit status: -1.\n";
        zlib.Cleanup();
        core_exit(-1);
    }
  
    core.source_items = (source_item*)malloc(core.lib_header.file_size*sizeof(source_item));
    if(core.source_items == NULL) std_err_("could not allocate resources to process bootstrap.");
    
    if(file_content_setup() != 0)
      return 1;
    
    iter=0;
    if(core_lib.size() == 0 || core_lib.at(0) != (char)0x6)
      std_err_("this does not look like a packaged file.");
    
    for(unsigned int i = 1; i < core_lib.size(); i++)
    {
        if(core_lib.at(0) != (char)0x6 || !((i+1) < core_lib.size()))
        {
            Binary::decode_str(__buf__.str(), __ostream_buf__);
            __buf__.str("");
            
            zlib.Compress_Buffer2Buffer(__ostream_buf__.str(), __buf__, ZLIB_LAST_SEGMENT);
            __ostream_buf__.str("");
            
            if(zres._warnings_.str() != "")
                cout << zres._warnings_.str();
                   
            if(zres.response == ZLIB_FAILURE){
                cout << "\n" << zres.reason.str() << "Error is not recoverable, exiting with exit status: -1.\n";
                zlib.Cleanup();
                core_exit(-1);
            }
            
            core.source_items[iter].src.hash_start();
            if(core.source_items[iter++].src.str( __buf__.str() ) != 0)
              std_err_("error setting lib data, low memory.");
            
            __buf__.str("");
            __ostream_buf__.str("");
        }
        else __buf__ << core_lib.at(i);
    }
    
    core_lib.str("");
    return 0;
}

/**
 * Exit core 
 * We create this method so that we can
 * free up used memory before exiting
 */
void core_exit(int e)
{
    core_bootstrap_clean();
    exit( e );
}


void core_bootstrap_clean()
{
    if(core.p == 1) return;
    if(core.source_items != NULL)
    {
        for(unsigned int i = 0; i < core.lib_header.file_size; i++)
        {
            core.source_items[i].src.str("");
            core.source_items[i].import.str("");
            core.source_items[i].name.str("");
        }
        free( core.source_items );
    }
    
    for(unsigned int i = 0; i < core.file_states.size(); i++)
    {
        core.file_states.get( i ).clear();
    }
    core.file_states.clear();
    core.class_list.clear();
    core.object_list.clear();
    core.function_list.clear();
    core.class_lookup_list.clear();
}

