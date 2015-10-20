//
// Created by bnunnall on 8/19/2015.
//

#include "filestream.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

bool FileStream::exists(const char *file)
{
    std::ifstream infile(file);
       return infile.good();
}

int FileStream::out(const char *file, string data)
{
    ofstream f (file);
     if (f.is_open())
    {
       f << data;
       f.close();
       return 0;
    }
     else
       return -1;
}

string FileStream::getfile(const char *file)
{
     string tmp;
     stringstream f;
    ifstream input(file);

    while(!input.eof()) {
        tmp = "";
        getline(input, tmp);
        f << tmp;
        f << "\n";
    }
    return f.str();
}

bool FileStream::endswith(string ext, string file)
{
    int ext_t = ext.length();
    stringstream extension;
    for(int i = file.length() - ext_t; i < file.length(); i++){
        extension << file.at(i);
    }
    
    if(extension.str() == ext)
      return true;
    return false;
}
