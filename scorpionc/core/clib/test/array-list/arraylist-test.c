
#include "../../arraylist.h"
#include <assert.h>

ListAdapter<int> numbers;

void printinfo()
{
    for(int i = 0; i < numbers.size(); i++)
    {
        cout << "numbers[" << i << "] := " << numbers.valueAt(i) << endl; 
    }
}

int main()
{
    numbers.add(1);
    numbers.add(2);
    numbers.add(3);
    numbers.add(4);
    numbers.add(5);
    
    numbers.replace(99,3);
    numbers.pushback();
    
    assert(numbers.contains(1));
    
    printinfo();
    cout << "\n\n";
    
    numbers.insert(111, 0);
    printinfo();
    cout << "\n\n";
    numbers.remove(numbers.size() - 1);
    numbers.remove(1);
    printinfo();
    cout << "\n\n";
    numbers.remove(0);
    numbers.remove(1);
    printinfo();
    cout << "\ndone.\n\n";
    return 0;
}
