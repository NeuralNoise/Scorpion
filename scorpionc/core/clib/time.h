//
// Created by bnunnall on 8/19/2015.
//

#ifndef SCORPION_TIME_H
#define SCORPION_TIME_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

/*
* Timing library
*
* Get the time of execution based on
* the inital time recorded
*/
struct c_time_ {
    clock_t time_s;
};

class Timer {
     private:
        c_time_ c_time;
        double timeinseconds(){ 
             return ((double)((double) clock() - c_time.time_s)/CLOCKS_PER_SEC);
        }
        
     public:
         double gettime() { return timeinseconds(); }
         void reset(){ c_time.time_s = clock(); }
};

#endif //SCORPION_TIME_H