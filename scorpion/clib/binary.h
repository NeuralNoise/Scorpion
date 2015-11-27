//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_BINARY_H
#define SCORPION_BINARY_H

#include <string>
#include <sstream>
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
        static void encode_str( const std::string &binary, stringstream &__outbuf__);
        static void decode_str( const std::string &text, stringstream &__outbuf__);
};

#endif //SCORPION_BINARY_H