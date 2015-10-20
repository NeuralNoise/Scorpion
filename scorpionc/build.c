#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>
using namespace std;

int main()
{
  system("g++ -std=c++11 -o scorpionc *.cpp clib/*.cpp");
  system("sudo cp scorpionc /bin");
  system("scorpionc");
  system("cd test/.dev/");
  system("scorpionc --build settings.dev ../app/src/scorpion/application/HelloWorld.scorpion");
}
