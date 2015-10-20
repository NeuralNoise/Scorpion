//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_ARCH_FILESTREAM_H
#define SCORPION_ARCH_FILESTREAM_H

#include <string>
using namespace std;

class FileStream {
      public:
        static string getfile(const char *file);
        static bool exists(const char *file);
        static bool endswith(string ext, string end);
        static int out(const char *file, string data);
};



#endif //SCORPION_ARCH_FILESTREAM_H
