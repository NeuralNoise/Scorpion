
 #include "u4.h"
 #include "u2.h"
 #include "u1.h"
 #include <sstream>
 #include <string>
 
 using namespace std;

 bool cmp_u4(u4 a, u4 b)
 {
     if((a.byte1 == b.byte1) && (a.byte2 == b.byte2) &&
         (a.byte3 == b.byte3) && (a.byte4 == b.byte4))
         return true;
    return false;
 }
 
  bool cmp_u2(u2 a, u2 b)
 {
     if((a.byte1 == b.byte1) && (a.byte2 == b.byte2))
         return true;
    return false;
 }
 
  bool cmp_u1(u1 a, u1 b)
 {
     if((a.byte1 == b.byte1) )
         return true;
    return false;
 }