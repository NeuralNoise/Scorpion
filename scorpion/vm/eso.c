/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for processing a Scorpion
* image.
* 
*/
#include "eso.h"
#include "binary.h"
#include "zlib.h"
#include "log.h"
#include "sopcodes.h"
#include <iosfwd>
#include <iomanip>

extern void std_err_(string n);
extern int revision;

unsigned int eso_buf_limit = (64*1024)*(8*156);
/* Compiler flags */
#define flag_method 0x7f 
#define flag_ins  0x5a
#define flag_str_ins 0x3b

/**
 * This array holds the size for each
 * Scorpion opcode, this is very useful
 * for quick opcode processing
 */
int op_t[] = {
  0, /* OP_NOP */
  1, /* OP_PUSH */
  1, /* OP_POP */
  1, /* OP_JMP */
  1, /* OP_MTHD */
  1, /* OP_RETURN */
  0, /* OP_END */
  2, /* OP_CALL */
  3, /* OP_ISEQ */
  3, /* OP_ISLT */
  3, /* OP_ISLE */
  3, /* OP_ISGT */
  3, /* OP_ISGE */
  2, /* OP_ICONST */
  2, /* OP_SCONST */
  2, /*OP_DCONST */
  2, /*OP_FCONST */
  2, /*OP_CCONST */
  2, /*OP_BCONST */
 -1, /*OP_STRCONST */  // this must be speciall processed
  3, /*OP_ADD */
  3, /*OP_SUB */
  3, /*OP_MULT */
  3, /*OP_DIV */
  3, /*OP_MOD */
  2, /*OP_LSHFT */
  2, /*OP_RSHFT */
  1, /*OP_CIN */
 -1, /*OP_COUT */  // this must be speciall processed
  0, /*OP_HLT */
  2, /*OP_JIT */
  2, /*OP_JIF */
  1, /*OP_LBL */
  0, /*OP_NO */
  0, /*OP_ENDNO */
  1, /*OP_IF */
  3, /*OP_OR */
  1, /*OP_INC */
  1, /*OP_DEC */
  3, /*OP_AND */
  2, /*OP_THROW */
  3, /*OP_AT */
  2, /*OP_STR_APND */
  1, /*OP_KILL */
  1, /*OP_DELETE */
-10, /* unused */
  3, /*OP_ALOAD */
  3, /*OP_ASTORE */
  2, /*OP_ASSN */
  2, /*OP_IACONST */
  2, /*OP_STR_ACONST */
  3, /*OP_CAST */
  2, /*OP_BYTE_CONST */
  2, /*OP_LCONST */
  2, /*OP_SACONST */
  2, /*OP_BYTE_ACONST */
  2, /*OP_FACONST */
  2, /*OP_DACONST */
  2, /*OP_CACONST */
  2, /*OP_LACONST */
  2, /*OP_BACONST */
  1, /*OP_NODE */
  1, /*OP_NEG */
  2, /*OP_STR_X2I*/
  2,  /*OP_STR_X2F*/
  2,  /*OP_BIT_CONST*/
  2  /*OP_BIT_ACONST*/
};

bool op_const( int op )
{
    return (op == OP_SCONST || op == OP_BCONST || op == OP_CCONST
            || op == OP_ICONST || op == OP_DCONST || op == OP_FCONST
            || op == OP_BYTE_CONST || op == OP_LCONST || op == OP_BIT_CONST);
}

bool op_aconst( int op )
{
    return (op == OP_DACONST || op == OP_IACONST || op == OP_FACONST 
            || op == OP_CACONST || op == OP_BACONST || op == OP_BYTE_ACONST 
            || op == OP_SACONST || op == OP_LACONST || op == OP_BIT_ACONST);
}

bool op_compare( int op )
{
    return (op == OP_ISEQ || op == OP_ISLT || op == OP_ISLE
            || op == OP_ISGT || op == OP_ISGE || op == OP_OR 
            || op == OP_AND);
}
    
int readc(std::string &image, uint64_t *ndx, bool safe_mode = false)
{
    if((*ndx) >= image.size())
    {
        if(!safe_mode)
          std_err_("unexpected end of file in eso image.");
        else
          return 356;
    }
   //   std::cout << " read byte " << (int)image.at( (*ndx) ) << endl;
    return (int)image.at( (*ndx)++ );
}

string lreadc(std::string& image, int length, uint64_t* ndx)
{
    stringstream ss;
    for(int i = 0; i < length; i++)
    {
        if((*ndx) >= image.size())
          std_err_("unexpected end of file in eso image.");
        ss << image.at( (*ndx)++ );
    }
    return ss.str();
}

string readstr(std::string& image, uint64_t* ndx, bool decode_ = true) // TODO: add boolean to decode image text "ndx, bool decode_ = false)""
{
    char c;
    stringstream ss, buf;
    for(unsigned int i = 0; i < image.size(); i++)
    {
        c = readc( image, ndx );
        if(c == 0) { break; }
        ss << c;
    }
    if(decode_)
    {
        Binary::decode_str( ss.str(), buf );
        ss.str("");
        
        return buf.str();
    }
    else
      return ss.str();
}

string no_nln(string data)
{
   stringstream ss;
   for(unsigned long i2 = 0; i2 < data.size(); i2++){
       if(data.at(i2) == '\n'){ }
       else
         ss << data.at(i2);
   }
   return ss.str();
}

string _image_section( string &image, uint64_t *ndx )
{
    stringstream ss;
    for(unsigned long i = (*ndx); i < image.size(); i++)
      ss << image.at(i);
    return ss.str();
}

void add_instruction(int op, ARG a, ARG ax, ARG b, double c, string str, scorpion_state* v_state, uint64_t* p)
{
    if((*p) > v_state->iheap_t)
    {
        log.LOGV( "Image size bigger than specified." );
        std_err_( "Eso preprocessing failure." );
    }
    v_state->i_heap[ (*p) ].data.hash_start();
    
    v_state->i_heap[ (*p) ].O = op;
    v_state->i_heap[ (*p) ].A = a;
    v_state->i_heap[ (*p) ].Ax = ax;
    v_state->i_heap[ (*p) ].B = b;
    v_state->i_heap[ (*p) ].data.str( str );
    v_state->i_heap[ (*p) ].C = c;
    
    //cout << "add " << op << " A:" << a << " Ax:" << ax << " B:" << b << " C:" << c << endl;
    (*p)++;
}

void create_method(scorpion_state* v_state, std::string method_info, slong addr, slong p)
{
    stringstream m_name, class_name, 
                package, file_name;
     int index = 0;
     bool native = false;
     if(method_info.at(0) == '~'){
       index = 1;
       native = true;
     }

     for(unsigned int i2 = index; i2 < method_info.size(); i2++){
        if(method_info.at(i2) == '&'){
            index++;
            break;
        }
        else {
          m_name << "" << method_info.at(i2);
        }
        index++;
     }
     
     for(unsigned int i2 = index; i2 < method_info.size(); i2++){
        if(method_info.at(i2) == '&'){
            index++;
            break;
        }
        else
          class_name << "" << method_info.at(i2);
        index++;
     }
    
     for(unsigned int i2 = index; i2 < method_info.size(); i2++){
        if(method_info.at(i2) == '&'){
            index++;
            break;
        }
        else
          package << "" << method_info.at(i2);
        index++;
     }
    
     for(unsigned int i2 = index; i2 < method_info.size(); i2++)
          file_name << "" << method_info.at(i2);
     
     func m;
     m.name.hash_start();
     m._class.hash_start();
     m.package.hash_start();
     m.file.hash_start();
     
     m.name.str(m_name.str());  
     m._class.str(class_name.str());
     m.package.str(package.str());
     m.native_func = native;
     m.file.str(" ");
     m.goto_ = p;
     
     v_state->static_methods[addr] = m;
}
     

int Eso::process(scorpion_state* v_state)
{
    if(header.file_size >= eso_buf_limit)
    {
        stringstream ss;
        ss << "Image size too large. Note: size = " << header.file_size 
           << " >= max size (" << eso_buf_limit << ").";
        log.LOGV( ss.str() );
        return 1;
    }
    
    n=0;
    p=0;
    v_state->i_heap = (Instruction*)malloc(header.instruction_cap*sizeof(Instruction));
    if(v_state->i_heap == NULL)
    {
        log.LOGV("Could not allocate resources for eso image.");
        return 1;
    }
    v_state->iheap_t = header.instruction_cap;
    for(uint64_t i = 0; i < header.address_cap; i++)
      v_state->heap[i].alloc = false;
    
    slong byte = readc( image, &n, true );
    for( ;; )
    {
        switch( byte )
        {
            case 0: { byte = readc( image, &n, true ); }
            case flag_method: {
                 byte=atof(readstr( image, &n, false ).c_str());
                 if(byte != OP_MTHD)
                   return 1;
                   
                 string info = readstr( image, &n, false );
                 slong arg1 = atol(readstr( image, &n, false ).c_str());
                 
                 add_instruction( byte, arg1, 0, 0, 0, "", v_state, &p );
                 
                 if((unsigned int) arg1 >= v_state->method_t)
                 {
                     log.LOGV("There are more methods than specified.");
                     return 1;
                 }
                 
                 create_method(v_state, info, arg1, p);
                 byte = readc( image, &n, true );
                 continue; 
            }
            case flag_ins: {
                 byte=atol(readstr( image, &n, false ).c_str());
                 if(!(byte > 0) && !(byte <= sMaxOpcodeLimit)
                    && !(byte <= sMaxOpLen))
                   return 1;
                 
                 if(op_t[ byte ] == 0)
                 {
                    add_instruction( byte, 0, 0, 0, 0, "", v_state, &p );
                    byte = readc( image, &n, true );
                    continue; 
                 }
                 else if(op_t[ byte ] == 1)
                 {
                    slong arg1 = atol(readstr( image, &n, false ).c_str());
                    add_instruction( byte, arg1, 0, 0, 0, "", v_state, &p );
                    
                    if(byte == OP_LBL)
                    {
                        create_object(primitive_long, &v_state->heap[ arg1 ]);
                        sSet(&v_state->heap[ arg1 ], p);
                    }
                    byte = readc( image, &n, true );
                    continue; 
                 }
                 else if(op_t[ byte ] == 2)
                 {
                    double arg1 = atol(readstr( image, &n, false ).c_str()),
                           arg2 = atol(readstr( image, &n, false ).c_str());
                    
                    if(op_const( byte ))
                    {
                        add_instruction( OP_CONST, byte, 0, arg1, 
                         arg2, "", v_state, &p );
                       
                        byte = readc( image, &n, true );
                        continue; 
                    }
                    else if(op_aconst( byte ))
                    {
                        add_instruction( OP_ACONST, byte, 0, arg1, 
                         arg2, "", v_state, &p );
                       
                        byte = readc( image, &n, true );
                        continue; 
                    }
                       
                    add_instruction( byte, arg1, 0, arg2, 0, "", v_state, &p );
                    byte = readc( image, &n, true );
                    continue; 
                 }
                 else if(op_t[ byte ] == 3)
                 {
                    if(op_compare( byte ))
                    {
                        add_instruction( OP_CMP, byte, atof(readstr( image, &n, false ).c_str()), 
                       atof(readstr( image, &n, false ).c_str()), 
                       atof(readstr( image, &n, false ).c_str()), "", v_state, &p );
                       
                        byte = readc( image, &n, true );
                        continue; 
                    }
                      
                    add_instruction( byte, atof(readstr( image, &n, false ).c_str()), 0,
                       atof(readstr( image, &n, false ).c_str()), 
                       atof(readstr( image, &n, false ).c_str()), "", v_state, &p );
                    byte = readc( image, &n, true );
                    continue; 
                 }
                 else
                   return 1;
            }
            case flag_str_ins: {
                 byte=atol(readstr( image, &n, false ).c_str());
                 if(byte == OP_COUT){
                     string msg = readstr( image, &n, false );
                     
                     add_instruction(OP_COUT, 0, 0, 0, 0, msg, v_state, &p);  
                     byte = readc( image, &n, true);
                     continue;
                 }
                 else if(byte == OP_STRCONST){
                     long a = atol(readstr( image, &n, false ).c_str());
                     string msg = readstr( image, &n, false );
                    
                     add_instruction(OP_STRCONST, a, 0, 0, 0, msg, v_state, &p);       
                     byte = readc( image, &n, true);
                     continue;
                 }
                 else
                    return 1;
            }
            case 356: {
                image = "";
                if(p != header.instruction_cap)
                {
                    log.LOGD("Eso image bytes processed does not match expected length.");
                    return 1;
                }
                return 0;
            }
            default: {
                stringstream ss;
                ss << "Invalid flag found in eso image (" << byte << ")";
                log.LOGV( ss.str() );
                return 1;
            }
        }
    }
    return ( 0 );
}
      
int Eso::read(const char* file, BIO* io)
{
    n=0;
    buff_leak( io );
    int io_response= buffS_read( file, io );
    if(io_response != buf_ok)
    {
        buff_leak( io ); // free io buffer
        if(io_response == buf_trunk)
           std_err_("file truncated, file is not readable.");
        else if(io_response == buf_fail)
        {
           stringstream ss;
           ss << "cannot read file `" << file << "':  No such file.";
           std_err_(ss.str());
        }
    }
    
    header.magic.hash_start();
    header.version_number.hash_start();
    header.log_file.hash_start();
    header.application_id.hash_start();
    header.permissions.hash_start();
    header.name.hash_start();
    
    image = buf_data( io ).str();
    buff_leak( io ); // free io buffer
    stringstream magic_edian;
    magic_edian << 'e' << (char)0xd0 << (char)0xfe << (char)0xe8 << (char)0x2b;
    
    if(readc( image, &n ) != 0)
       std_err_("invalid char found in file, file format err.");
    if(lreadc( image, 3, &n ) != "ESO")
       std_err_("invalid char found in file, file format err.");
       
    n+=16;
    int header_tracker=0,sizeof_header=17,flag=0;
    
    std::string magic = lreadc( image, 5, &n );
    if(magic != magic_edian.str())
       std_err_("invalid magic number in eso file, file format err.");
    header.magic.str( magic );
    n++;
    
    for( ;; )
    {
        flag = readc( image, &n );
        header_tracker++;
        
        switch( flag )
        {
            case 0: { header_tracker--; break;} // ignore stray nulls
            case 10: { header_tracker--; break;} // ignore stray \n's
            case flag_minor_version: {
                header.minor_version[0] = readc( image, &n );
                header.minor_version[1] = readc( image, &n );
                continue;
            }
            case flag_major_version: {
                header.major_version[0] = readc( image, &n );
                header.major_version[1] = readc( image, &n );
                continue;
            }
            case flag_target: {
                header.target_plat_vers = atol(readstr( image, &n ).c_str());
                continue;
            }
            case flag_base_target: {
                header.minimum_plat_vers = atol(readstr( image, &n ).c_str());
                continue;
            }
            case flag_version: {
                header.version_number.str( readstr( image, &n ) );
                continue;
            }
            case flag_debuggable: {
                header.debug = (bool)atol(readstr( image, &n, false ).c_str());
                continue;
            }
            case flag_loggable: {
                header.logging = (bool)atol(readstr( image, &n, false ).c_str());
                continue;
            }
            case flag_log_level: {
                header.log_precedence = atol(readstr( image, &n, false ).c_str());
                continue;
            }
            case flag_log_file: {
                header.log_file.str( readstr(image, &n) );
                continue;
            }
            case flag_app_id: {
                header.application_id.str( readstr(image, &n) );
                continue;
            }
            case flag_permissions: {
                header.permissions.str( readstr(image, &n) );
                continue;
            }
            case flag_image_size: {
                header.file_size = atol( readstr(image, &n).c_str() );
                continue;
            }
            case flag_name: {
                header.name.str( readstr(image, &n, false) );
                continue;
            }
            case flag_address_cap: {
                header.address_cap = atol( readstr(image, &n).c_str() );
                continue;
            }
            case flag_method_cap: {
                header.method_size = atol( readstr(image, &n).c_str() );
                continue;
            }
            case flag_instruction_cap: {
                header.instruction_cap = atol( readstr(image, &n).c_str() );
                continue;
            }
            case flag_eoh: {
                break;
            }
            default: {
                stringstream ss;
                ss << "unknown flag " << flag << " in eso file, file format err.";
                std_err_( ss.str() );
            }
        }
        if(flag == flag_eoh) break;
    }
    
    n+=33;
    if(header_tracker!=sizeof_header)
       std_err_("header in eso file could not be processed correctly, file format err.");
    if(header.major_version[0] != 1 && header.major_version[1] != 20)
       std_err_("invalid major version number in eso file, file is not readable.");
    if(header.minimum_plat_vers > revision)
       std_err_("base target platform version number is greatern than the current platforn build, file is not runnable.");
    
    Zlib zip;
    stringstream __img_buf__;
     
    zip.Decompress_Buffer2Buffer(_image_section( image, &n ), __img_buf__);
    if(zres.response == ZLIB_FAILURE){
        printf("%s", zres.reason.str().c_str());
        zip.Cleanup();
        std_err_("error decompressing eso file, error is not recioverable.\nexiting with status code 1");
    }
    else if(zres._warnings_.str() != "")
        printf("%s", zres._warnings_.str().c_str());
     
    image = no_nln(__img_buf__.str());
    __img_buf__.str("");
    zip.Cleanup();
    
    return 0;
}

string Eso::header_info()
{
    stringstream ss;
    ss << "magic e -- 0xd0 0xfe 0xe8 0x2b\n";
    ss << "Eso format (minor--major) version: 0x" << hex << (int) header.minor_version[0] 
       << " 0x" << (int) header.minor_version[1] << " -- 0x" << (int) header.major_version[0]
       << " 0x" << (int) header.major_version[1] << dec << endl;
    ss << "app name: " << header.name.str() << endl;
    ss << "app id: " << header.application_id.str() << endl;
    ss << "target platform: " << header.target_plat_vers << endl;
    ss << "base target platform: " << header.minimum_plat_vers << endl;
    ss << "build version: " << header.version_number.str() << endl;
    ss << "log enabled: " << ((header.logging==1) ? "true" : "false") << endl;
    ss << "log file: " << header.log_file.str() << endl;
    ss << "log level: " << (int)header.log_precedence << endl;
    ss << "debugging enabled: " << ((header.debug==1) ? "true" : "false") << endl;
    ss << "method size: " << header.method_size << endl;
    ss << "image size: " << header.file_size << " bytes" << endl;
    return ss.str();
}
