
#include <string>
#include <iostream>
#include <cstddef> 
#include <stdio.h>
#include <string.h>
using namespace std;


bool hasstring(string str, string *arry, int size)
{
    for(int i = 0; i < size; i++){
        if(arry[i] == str)
           return true;
    }
   return false;
}
