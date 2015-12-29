
#ifndef sfunc_h
#define s_func_h

 #include <string>
 #include <sstream>
 #include <stdint.h>
 #include "arraylist.h"
 
 using namespace std;
 
 struct func
 {
     std::string name, file, _class,
                 package;
     uint64_t line, goto_, ret;
     bool native_func;
 };
 
 class function_tracker
 {
     public:
        void add_func(func _func, uint64_t call_line, bool main = false)
        {
            if(main)
            {
                stringstream ss;
                ss << _func.name << "<init>";
                _func.name = ss.str();
            }
            
            if(function_list.size() > 20)
              function_list.remove(0); // remove first function
            function_list.add(_func);
        }
        
        string get_func_trace()
        {
            stringstream ss;
            ss << "  Traceback (most recent call last):\n";
            for(uint64_t i=0; i < function_list.size(); i++)
            {
                ss << "    at " << function_list.valueAt(i).package 
                    << "." << function_list.valueAt(i)._class << "." 
                      << function_list.valueAt(i).name << "(";
                      
                if(function_list.valueAt(i).native_func)
                  ss << "Native Function)\n";
                else
                {
                  ss << function_list.valueAt(i).file << ":"
                      << function_list.valueAt(i).line << ")\n";
                }
            }
            return ss.str();
        }
        
     private:
       ListAdapter<func> function_list;
 };
 

#endif // s_func_h