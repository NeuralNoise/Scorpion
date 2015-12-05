//
// Created by bnunnall on 8/19/2015.
//

#include "sarheader.h"
#include "clib/binary.h"
#include "clib/filestream.h"
#include "clib/arraylist.h"
#include <iostream>
#include <string>
using namespace std;

header h;
stringstream archive;
int SIZE;
bool isdup(string,int);

/*
* ***************************************
* ***************************************
*       Header File Symbols
*
*    minor version: 0xAF
*    major version: 0xAE
*     source count: 0xFC
*     source files: 0xBD
*        file size: 0xFF
*        next file: 0xAA
*
* ***************************************
* ***************************************
*/
int _minor_version = 0xAF;
int _major_version = 0xCE;
int _source_count  = 0xFC;
int _source_files  = 0xBD;
int file_size      = 0xEF;
int _next_file     = 0xAA;

string sfiles()
{
    stringstream ss;
    ss << (char) _source_files;
    for(int i = 0; i < SIZE; i++)
       ss << h.sourcefiles[i] << ";";
    return ss.str();
}


string scount()
{
    stringstream ss, ss1;
    ss1 << (long) h.sourcecount.byte1;
    stringstream __ostreambuf__;
    Binary::encode_str(ss1.str(), __ostreambuf__);
        
    ss << (char) _source_count << __ostreambuf__.str();
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

void encodeheader()
{
    archive << magic() << offset(1) << minvers() << offset(1) << majvers() << offset(1) 
            << scount() << offset(1) << sfiles() << offset(1) << offset(12) << "\n" << offset(20);
}

int sarheader::makeheader(int size, string *files)
{
    if(size != 0){
        SIZE = size;
        if(size >= MAX_LIMIT){
            cout << "Error:  There are too many input files" << endl;
            return -1;
        }

        // set magic number
        h.magic.byte1 = 0xaf;
        h.magic.byte2 = 0x6c;
        h.magic.byte3 = 0xbe;
        h.magic.byte4 = 0xf3;

        h.minor_version.byte1 = 0x15;
        h.minor_version.byte2 = 0x203;

        h.major_version.byte1 = 0x10;
        h.major_version.byte2 = 0x11;
        
        h.sourcefiles = new string[size];
        h.filesize.byte1=0;
        h.sourcecount.byte1=0;

        string contents; 
        int ret = 0;
        for(int i = 0; i < size; i++){
            if(FileStream::exists(files[i].c_str())){
               if(FileStream::endswith(".scorpion", files[i]) || FileStream::endswith(".xso", files[i]) 
                  || FileStream::endswith(".sn", files[i])){
                   contents = FileStream::getfile(files[i].c_str());
                   if(!(contents.size() >= MAX_SIZE)){
                       if(!isdup(files[i], size)){
                          if (files[i].find(";") != string::npos) {
                               cout << "Error:  file: " << files[i] << " cannot contain a ';' character." << endl;
                               ret = -1;
                          }
                          else{
                              h.sourcecount.byte1++;
                              h.sourcefiles[i] = files[i];
                              h.filesize.byte1 += contents.size(); // increment
                          }
                       }
                       else
                         cout << "sar:  warning: diplicate file: " << files[i] << " skipping." << endl;
                   }
                   else {
                      cout << "Error:  file: " << files[i] << " is too large!" << endl;
                      ret = -1;
                   }
                   contents ="";
               }
               else {
                   cout << "Error:  file: " << files[i] << " is not a scorpion source file." << endl;
                   ret = -1;
               }
            }
            else {
               cout << "Error:  file: " << files[i] << " is no such file." << endl;
               ret = -1;
            }
        }
        
        if(ret != 0)
           return ret;
        
        h.filesize.byte1 += 1;   
        encodeheader();
        return 0;
    }
    else {
       cout << "Error: no input file!" << endl;
       return -1;
    }
}

bool isdup(string f, int size)
{
    if(hasstring(f, h.sourcefiles, size))
       return true;
   return false;
}