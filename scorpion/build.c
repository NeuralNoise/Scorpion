#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
using namespace std;

int main()
{
  system("g++ -std=c++11 -o scorpion scorpionvm/Main.c");
  system("sudo cp scorpion /bin");
  system("scorpion");
}