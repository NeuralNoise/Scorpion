/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nullally, see full copyright licence in main.c
* 
* Standard Routines for processing a Scorpion
* image.
* 
*/
#include "eso.h"
#include "binary.h"
#include "zlib.h"
#include <iostream>
#include <iomanip>

extern void std_err_(string n);
extern int revision;

int readc(std::string &image, uint64_t *ndx)
{
    if((*ndx) >= image.size())
      std_err_("unexpected end of file in eso image.");
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
    
    image = buf_data( io ).str();
    buff_leak( io ); // free io buffer
    stringstream magic_edian;
    magic_edian << 'e' << (char)0xd0 << (char)0xfe << (char)0xe8 << (char)0x2b;
    
    if(readc( image, &n ) != 0)
       std_err_("invalid char found in file, file format err.");
    if(lreadc( image, 3, &n ) != "ESO")
       std_err_("invalid char found in file, file format err.");
       
    n+=16;
    int header_tracker=0,sizeof_header=16,flag=0; // at end of loop, this should be initalized to the size of the header
    
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
                header.debug = readc( image, &n );
                continue;
            }
            case flag_loggable: {
                header.logging = readc( image, &n );
                continue;
            }
            case flag_log_level: {
                header.log_precedence = readc( image, &n );
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
        cout << zres.reason.str();
        zip.Cleanup();
        std_err_("error decompressing eso file, error is not recioverable.\nexiting with status code 1");
    }
    else if(zres._warnings_.str() != "")
        cout << zres._warnings_.str();
     
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
       << " 0x" << (int) header.major_version[2] << dec << endl;
    ss << "app name: " << header.name.str() << endl;
    ss << "app id: " << header.application_id.str() << endl;
    ss << "target platform: " << header.target_plat_vers << endl;
    ss << "base target platform: " << header.minimum_plat_vers << endl;
    ss << "build version: " << header.version_number.str() << endl;
    ss << "log endabled: " << ((header.logging) ? "true" : "false") << endl;
    ss << "log file: " << header.log_file.str() << endl;
    ss << "log level: " << header.log_precedence << endl;
    ss << "debugging endabled: " << ((header.debug) ? "true" : "false") << endl;
    ss << "method size: " << header.method_size << endl;
    ss << "image size: " << header.file_size << " bytes" << endl;
    return ss.str();
}
