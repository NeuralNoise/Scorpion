//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_BINARY_H
#define SCORPION_BINARY_H

#include <string>
using namespace std;

/*
* This is a text encoding algorithm 
* that encodes any text into binary format.
*
* This small algorithm only encodes basic text 
* and symbols(only the chars that are on thr keyboard)
*
* This does not encode any chars such as NULL or EOF or ESC etc.
*/
class Binary {
     public:
        static std::string encode_str( const std::string &binary );
        static std::string decode_str( const std::string &text );
        static bool show;
        static long bytestatus;
        static long bytesize;
        static long sourcecount;
};

#endif //SCORPION_BINARY_H