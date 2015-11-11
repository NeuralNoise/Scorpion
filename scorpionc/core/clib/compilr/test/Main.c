#include "../compilr.h"
#include <iostream>
#include <string>

using namespace std;

Compilr compilr;

// Compilr test file
// $ g++ *.c ../*.c ../../*.c ../../zlib/*.c -o text
int main()
{
  compilr.Compile("main.sn", "test");
  
  if(cres._warnings_.str() != "")
      cout << cres._warnings_.str();
                               
  if(cres.response == COMPILR_FAILURE){
    cout << "\n" << cres.reason.str() << "Error is not recoverable, exiting with exit status: -1.\n";
    compilr.Cleanup(true);
    return -1;
  }
    
  compilr.Cleanup(true);
  cout << "done.\n";
  return 0;
}
