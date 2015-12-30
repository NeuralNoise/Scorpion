
#ifndef sfunc_h
#define s_func_h

 #include <string>
 #include <sstream>
 #include <stdint.h>
 #include "stypes.h"
 
 using namespace std;
 
 struct func
 {
     sstring name, file, _class,
                 package;
     uint64_t line, goto_, ret;
     bool native_func;
 };
 
 class function_tracker
 {
     public:
        void init()
        {
            function_list = new func[20];
            n=0;
        }
        void push_back()
        {
            n = 19;
            for(int i = 0; i < 19; i++)
            {
                function_list[i] = function_list[i+1];
            }
        }
        void add_func(func f)
        {
            if(n >=19)
            {
                push_back();
                function_list[19] = f;
            }
            else
                function_list[n++] = f;
        }
        void add_func(func _func, uint64_t call_line, bool main = false)
        {
            if(main)
            {
                stringstream ss;
                ss << _func.name.str() << "<init>";
                _func.name.str(ss.str());
            }
            
            _func.line = call_line;
            add_func( _func );
        }
        
        string get_func_trace()
        {
            stringstream ss;
            ss << "  Traceback (most recent call last):\n";
            for(int i=0; i < n; i++)
            {
                ss << "    at " << function_list[i].package.str() 
                    << "." << function_list[i]._class.str() << "." 
                      << function_list[i].name.str() << "(";
                      
                if(function_list[i].native_func)
                  ss << "Native Function)\n";
                else
                {
                  ss << function_list[i].file.str() << ":"
                      << function_list[i].line << ")\n";
                }
            }
            return ss.str();
        }
        
     private:
       func* function_list;
       int n;
 };
 

#endif // s_func_h