//
// Created by bnunnall on 8/19/2015.
//

#include "scorpionheader.h"
#include "clib/filestream.h"
#include "clib/arraylist.h"
#include "buildscript.h"
#include "clib/binary.h"
#include <iostream>
#include <string>
using namespace std;

header h;
stringstream spk;
long SIZE;
bool isdup(string,int);
    
/*
* ***************************************
* ***************************************
*       Header File Symbols
*
*         minor version: 0xAF
*         major version: 0xAE
*             file size: 0xFF
*    target dev version: 0xBC
*      minimum_dev_vers: 0xFD
*        version_number: 0xCE
*                 debug: 0xCC
*               logging: 0xDB
*        log precedence: 0xFA
*              log file: 0x3C
*        application id: 0xB5
*           permissions: 0xC1
*
* ***************************************
* ***************************************
*/
int _minor_version     = 0xAF;
int _major_version     = 0xCE;
int file_size          = 0xEF;
int target_dev_vers    = 0xBC;
int minimum_dev_vers   = 0xFD;
int version_number     = 0xCE;//
int debugging          = 0xEC;
int logging            = 0xFC;
int log_precedence     = 0xFA;
int log_file           = 0x3C;//
int application_id     = 0xB5; //
int permissions        = 0xC1;//
int nameflag           = 0x7A;
int methodsize         = 0xD9;

// TODO: encode header strings

string fsize()
{
    stringstream ss, ss1;
    bool restore = Binary::show;
    Binary::show = false;
    ss1 << (long) h.filesize.byte1;
    ss << (char) file_size << Binary::encode_str(ss1.str());
    Binary::show = restore;
    return ss.str();
}

string appname()
{
    stringstream ss;
    bool restore = Binary::show;
    Binary::show = false;
    ss << (char) nameflag << h.name;
    Binary::show = restore;
    return ss.str();
}

string methodsz()
{
    stringstream ss, ss1;
    bool restore = Binary::show;
    Binary::show = false;
    ss1 << (long) h.method_size.byte1;
    ss << (char) methodsize << Binary::encode_str(ss1.str());
    Binary::show = restore;
    return ss.str();
}

string tgt_dev_vers()
{
    stringstream ss;
    ss << (char) target_dev_vers << (char) h.target_dev_vers.byte1;
    return ss.str();
}

string min_dev_vers()
{
    stringstream ss;
    ss << (char) minimum_dev_vers << (char) h.minimum_dev_vers.byte1;
    return ss.str();
}

string vers_num()
{
    stringstream ss;
    bool restore = Binary::show;
    Binary::show = false;
    ss << (char) version_number << Binary::encode_str(h.version_number);
    Binary::show = restore;
    return ss.str();
}

string debg()
{
    stringstream ss;
    ss << (char) debugging << (long) h.debug.byte1;
    return ss.str();
}

string loggingg()
{
    stringstream ss;
    ss << (char) logging << (long) h.logging.byte1;
    return ss.str();
}

string logpd()
{
    stringstream ss;
    ss << (char) log_precedence << h.log_precedence.byte1;
    return ss.str();
}

string logfl()
{
    stringstream ss;
    bool restore = Binary::show;
    Binary::show = false;
    ss << (char) log_file << Binary::encode_str(h.log_file);
    Binary::show = restore;
    return ss.str();
}

string appid()
{
    stringstream ss;
    bool restore = Binary::show;
    Binary::show = false;
    ss << (char) application_id << Binary::encode_str(h.application_id);
    Binary::show = restore;
    return ss.str();
}

string prmns()
{
    stringstream ss;
    bool restore = Binary::show;
    Binary::show = false;
    ss << (char) permissions;
    for(int i = 0; i < permission_size; i++){
       stringstream ss1;    
       ss1 << h.permissions[i] << ";";
       ss << Binary::encode_str(ss1.str());
    }
    Binary::show = restore;
    return ss.str();
}

string majvers()
{
    stringstream ss;
    ss << (char) _major_version << (char) h.major_version.byte1;
    ss << (char) h.major_version.byte2;
    return ss.str();
}

string minvers()
{
    stringstream ss;
    ss << (char) _minor_version << (char) h.minor_version.byte1;
    ss << (char) h.minor_version.byte2;
    return ss.str();
}

string magic()
{
    stringstream ss;
    ss << (char)  h.magic.byte1;
    ss << (char) h.magic.byte2;
    ss << (char) h.magic.byte3;
    ss << (char) h.magic.byte4;
    return ss.str();
}

string offset(int offset)
{
    stringstream ss;
    char nill = 0;
    for(int i = 0; i < offset; i++)
        ss << (char) nill;
   return ss.str();
}

extern string application_file;
void encodeheader()
{
    spk << magic() << offset(1) << appname() << offset(3) << methodsz() << offset(1) << minvers() 
            << offset(1) << majvers() << offset(1) << fsize() << offset(1) << tgt_dev_vers() 
            << offset(1) << min_dev_vers() << offset(1) << vers_num() << offset(1) << debg() 
            << offset(1) << loggingg() << offset(1) << logpd() << offset(1) << logfl() << "\n" 
            << offset(1) << appid() << offset(1) << "\n" << prmns() << offset(1) << offset(12) 
            << "\n" << offset(20);
    stringstream ss;
    ss << appdir << "/build/outputs/exe/" << application_file;
    FileStream::out(ss.str().c_str(), spk.str());
}

void add_permissions()
{
    for(int i = 0; i < permission_size; i++){
        h.permissions[i] = io_permissions[i];
    }
}

int btoi(bool value)
{
   if(value == true)
      return 1;
  return 0;
}

int sheader::makeheader()
{  

        // set magic number
        h.magic.byte1 = 0xdf;
        h.magic.byte2 = 0x4e;
        h.magic.byte3 = 0xfa;
        h.magic.byte4 = 0x2b;

        h.minor_version.byte1 = 0x70;
        h.minor_version.byte2 = 0x010;

        h.major_version.byte1 = 0x7;
        h.major_version.byte2 = 0x10;

        h.target_dev_vers.byte1 = targetDevVersion;
        h.minimum_dev_vers.byte1 = minDevVersion;
        
        h.version_number = version_num;
        h.debug.byte1 = btoi(debug);
        h.logging.byte1 = btoi(loog);
        h.log_precedence.byte1 = logprecedence;
        
        stringstream ss;
        ss << appdir << "/build/outputs/logs/" << logfile;
        h.log_file = ss.str();
        
        h.application_id = applicationid;
        h.permissions = new string[permission_size];
        add_permissions();
        h.name = name;
        
        h.filesize.byte1 = 900; // istore 0x2 33   : int i = 33;
        h.method_size.byte1 = 1;
        
        encodeheader();
        return 0;
}

/*bool isdup(string f, int size)
{
    if(hasstring(f, h.sourcefiles, size))
       return true;
   return false;
}*/

