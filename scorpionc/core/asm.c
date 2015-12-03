#include <iostream>
using namespace std; int main() {
  static void* array[] = { &&foo, &&bar ,
                                  &&hack};
    int i = 2;
     goto *(array[i]);
   foo:
     cout << "foo";
   bar:
     cout << "bar";
   hack:
     cout << "hack";
  return 0;
}
