#include "../lexr.h"
#include <iostream>
#include <string>

using namespace std;
using namespace lexr;

lexr::generator lex;

// Compilr test file
// $ g++ *.c ../*.c ../../*.c ../../zlib/*.c -o text
int main()
{
  
  const std::string sn = "module test;  " 
                   "def init(string** args) "
                   "{  " 
                   "    asm(\"  " 
                   "        cout $\"Hello, World!$\"" 
                   "    \")  " 
                   "} ";
  
  if (!lex.process(sn)) {
      cout << "lexr:  error: Something went wrong while parsing the data.\n";
      return -1;
  }
  
  
   lexr::helper::dump(lex);
  cout << "done.\n";
  return 0;
}
