#ifndef SCORPION_VM_PERMISSION
#define SCORPION_VM_PERMISSION

 #include <sstream>
 #include <iostream>
 #include <string>
 using namespace std;
 
    class Permission {
        string mPermission;
        
        public:
           void setPermission(string perm){
               mPermission = perm;
           }
           string getPermission(){
               return mPermission;
           }
    };

#endif