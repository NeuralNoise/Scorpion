#include <string>
#include <iostream>

int i = 0;
std::string source_text[] = 
   {
     "int",
     "i",
     "=",
     "7"
   };
   
bool identifier()
{ return source_text[i++] == "i"; }
   
bool type()
{ return source_text[i++] == "int"; }   

bool eq()
{ return source_text[i++] == "="; }

bool number()
{ return source_text[i++] == "7"; }

bool int_decl()
{
    // type_ast t_ast;
    if(type()) {
        // do stuff
    }
    else std::cout << "err\n";
    
    if(identifier()) {
        // do stuff
    }
    else std::cout << "err\n";
    
    if(eq()) {
        // do stuff
    }
    else std::cout << "err\n";
    
    if(number()) {
        // do stuff
    }
    else std::cout << "err\n";
}
   
int main()
{
    int_decl();
}
   