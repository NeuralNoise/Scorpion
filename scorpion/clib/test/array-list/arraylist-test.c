
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
    
    ListAdapter<int*> numbers2;
    
    int* i;
    i = new int[1];
    i[0] = 55;
    numbers2.add(i);
    cout << "numbers2 @1 = " << numbers2.valueAt(0)[0] << endl;
    cout << "done.\n";
    return 0;
}
