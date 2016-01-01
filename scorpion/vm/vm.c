/**
* Copyright (C) 2015 The Scorpion Programming Language
* Braxton Nunnally, see full copyright licence in main.c
* 
* Standard Routines for the scorpion
* virtual machine
* 
*/

#include "vm.h"
#include "eso.h"
#include "log.h"
#include "sopcodes.h"
#include <iosfwd>
#include <stdint.h>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <limits.h>
#include <iomanip>
#include <termios.h>
#include <unistd.h>

using namespace std;

void _return(slong ptr, scorpion_state* state){
     if((unsigned int)ptr >= state->method_t){
         stringstream ss;
         ss << "Pointer to method at address #" << ptr << " is outside of allocated range."
             << " Note: method size: " << state->method_t;
         runtime_err(state, "MethodIndexOutOfBoundsException", ss.str());
     }
     
     state->pc = state->static_methods[ptr].ret;
}

void scorpion_Vassign(slong ptr, slong ptr2, scorpion_state* v_state) 
{ 
    if((unsigned int)ptr>=v_state->size_t || (unsigned int)ptr2>=v_state->size_t) 
           runtime_err(v_state, "OutOfMemoryErr", "Scorpion heap space"); 
           
    if(isnumber(&v_state->heap[ptr]) && isnumber(&v_state->heap[ptr2])) 
      sSet(&v_state->heap[ptr], sValue(&v_state->heap[ptr2])); 
    else if(isarray(&v_state->heap[ptr]) && isarray(&v_state->heap[ptr2])) 
      v_state->heap[ptr] = v_state->heap[ptr2]; 
    else if(isstring(&v_state->heap[ptr]) && isstring(&v_state->heap[ptr2])) 
      s_strSet(&v_state->heap[ptr], s_strValue(&v_state->heap[ptr2])); 
} 

#define str_append(v_state) \
        { \
            stringstream ss; \
            slong ptr = v_state->i_heap[v_state->pc++], \
                  ptr2 = v_state->i_heap[v_state->pc++]; \
            ss << s_strValue(&v_state->heap[ptr]); \
            s_strSet(&v_state->heap[ptr], ""); \
             \
            if(isnumber(&v_state->heap[ptr2])) \
            {  \
                ss << (char)sValue(&v_state->heap[ptr2]); \
                s_strSet(&v_state->heap[ptr], ss.str()); \
            } \
            else if(isstring(&v_state->heap[ptr2])) \
            { \
                ss << s_strValue(&v_state->heap[ptr2]);  \
                s_strSet(&v_state->heap[ptr], ss.str()); \
            }\
        }

double cast(int c, double v)
{
    switch( c )
    {
        case 1: return (schar)v;
        case 2: return (byte)v;
        case 3: return (sint)v;
        case 4: return (sshort)v;
        case 5: return (slong)v;
        case 6: return (sbool)v;
        case 7: return (sfloat)v;
        case 8: return (double)v;
        default: return -1;
    }
}

bool s_numcompare(int op, double v, double v2)
{
    if(op == OP_ISLT)
       return (v<v2);
    else if(op == OP_ISGT)
       return (v>v2);   
    else if(op == OP_ISGE)
       return (v>=v2);   
    else if(op == OP_ISLE)
       return (v<=v2);   
    else if(op == OP_OR)
       return (v||v2);   
    else if(op == OP_AND)
       return (v&&v2);   
    else if(op == OP_ISEQ)
       return (v==v2); 	
    else return false;
}

bool s_strcompare(int op, string v, string v2)
{
    if(op == OP_ISLT)
       return (v<v2);
    else if(op == OP_ISGT)
       return (v>v2);   
    else if(op == OP_ISGE)
       return (v>=v2);   
    else if(op == OP_ISLE)
       return (v<=v2);   
    else if(op == OP_ISEQ)
       return (v==v2); 	
    else return false;
}

void stack_pop(scorpion_state* v_state)
{
   scorpion_Vassign(v_state->i_heap[v_state->pc++],v_state->stack[v_state->sp--],v_state);
}

void stack_push(scorpion_state* v_state)
{
   v_state->stack[++v_state->sp] = v_state->i_heap[v_state->pc++];
}

int OPTYPE(int op)
{
    if(op == OP_ICONST || op == OP_IACONST)
       return primitive_int;
    else if(op == OP_SCONST || op == OP_SACONST)
       return primitive_short;
    else if(op == OP_BYTE_CONST || op == OP_BYTE_ACONST)
       return primitive_byte;
    else if(op == OP_LCONST || op == OP_LACONST)
       return primitive_long;
    else if(op == OP_FCONST || op == OP_FACONST)
       return primitive_float;
    else if(op == OP_DCONST || op == OP_DACONST)
       return primitive_double;
    else if(op == OP_CCONST || op == OP_CACONST)
       return primitive_char;
    else if(op == OP_BCONST || op == OP_BACONST)
       return primitive_boolean;
    else return 1111;
}

#define func_return(i, v_state) { \
            slong ptr = i; \
            --v_state->func_depth; \
            if((ptr == 0) && (v_state->func_depth <= 0)) { \
              log.LOGA("Exiting main() function: __init__"); \
              quit( v_state ); \
            } \
            else _return(ptr, v_state); \
        }

#define v_stack_op(v_state, i) \
        { \
            slong op = i; \
            if(op == OP_PUSH) \
              stack_push(v_state); \
            else \
              stack_pop(v_state); \
        } 

#define do_jump(i,v_state) \
        { \
          slong ptr = v_state->i_heap[v_state->pc++]; \
          slong ptr2 = v_state->i_heap[v_state->pc++]; \
          if(sValue(&v_state->heap[ptr2]) == ((i==OP_JIT) ? 1 : 0)) \
             v_state->pc = sValue(&v_state->heap[ptr]); \
           goto exe_; \
        }
        
bool op_compare(int op, scorpion_state* v_state)
{
    slong ptr = v_state->i_heap[v_state->pc++];
    slong ptr2 = v_state->i_heap[v_state->pc++];
    
    if(isnumber(&v_state->heap[ptr]) && isnumber(&v_state->heap[ptr2]))
      return s_numcompare(op,sValue(&v_state->heap[ptr]), sValue(&v_state->heap[ptr2]));
    else if(isstring(&v_state->heap[ptr]) && isstring(&v_state->heap[ptr2]))
      return s_strcompare(op,s_strValue(&v_state->heap[ptr]), s_strValue(&v_state->heap[ptr2]));
    else return false;
}

slong ARG_t(scorpion_state* v_state, slong i)
{
    if(op_t[ i ] != -1)
      return op_t[ i ];
    else
    {
        if(i == OP_COUT)
          return v_state->i_heap[v_state->pc++]+1;
        else if(i == OP_STRCONST)
        {
          v_state->pc++;
          return v_state->i_heap[v_state->pc++]+1;
        }
        return 0;
    }
}

void protect(scorpion_state* v_state, slong i)
{
    if(v_state->pc>=v_state->iheap_t)
       log.LOGV("Attempting to close program unexpectingly.");
    else
    {
       stringstream ss;
       ss << "Attempting to execute invalid instruction(" << i << ").";
       log.LOGV(ss.str());
    }
    
    segfault(v_state);
}

void Invoke_Method(scorpion_state* state)
{
    unsigned long ptr = state->i_heap[state->pc++], line = state->i_heap[state->pc++];
    if(ptr >= state->method_t)
     {
         stringstream ss;
         ss << "Pointer to method at address #" << ptr << " is outside of allocated range."
             << " Note: method size: " << state->method_t;
         runtime_err(state, "MethodIndexOutOfBoundsException", ss.str());
     }
     
     state->func_tracker.add_func(state->static_methods[ptr], line);
         
     state->static_methods[ptr].ret = state->pc;
     state->pc = state->static_methods[ptr].goto_;
}

void do_if(scorpion_state* v_state)
{
    long ptr = v_state->i_heap[v_state->pc++];
              
    if(v_state->state == state_normal &&
        ((bool)sValue(&v_state->heap[ptr])) == 0)
    {
         v_state->if_depth = v_state->if_count;
         v_state->state = state_passive_frozen;
    }
      
    v_state->if_count++;
}

void segfault(scorpion_state* v_state)
{
    printf("Segmentation fault\n");
    
    stringstream err;
    err << "FATAL EXCEPTION:  main\nRuntimeException: Caused by SegmentationFaultErr: Scorpion vm state.\n" 
        << v_state->func_tracker.get_func_trace();
    log.LOGV( err.str() );
    
    destroy_state( v_state, 1);
    log.LOGV("--- vm is down, process exiting.");
    exit(EXIT_FAILURE);
}

void quit(scorpion_state* v_state)
{
    slong exitval = 0, ptr;
    if(v_state->sp >= 0)
    {
        ptr = v_state->stack[v_state->sp--];
        if(!((unsigned int)ptr >= v_state->size_t))
          exitval = sValue(&v_state->heap[ptr]);
        
        stringstream ss;
        ss << "image exiting with value: " << exitval;
        log.LOGI( ss.str() );
    }
    
    if(destroy_state( v_state, 1) != 0)
      printf("Warning, the Scorpion VM did not shutdown cleanly.\n");
    log.LOGV("--- vm is down, process exiting.");
    exit( exitval );
}

void runtime_err(scorpion_state* v_state, string what, string err)
{
     printf("RuntimeException: Caused by %s: %s\n%s", what.c_str(), 
        err.c_str(), v_state->func_tracker.get_func_trace().c_str());
     destroy_state( v_state, 1);
     log.LOGV("--- vm is down, process exiting.");
     exit(EXIT_FAILURE);
}

string data(slong size_t, scorpion_state* state)
{
     stringstream ss;
     for(long i = 0; i < size_t; i++)
         ss << (char)state->i_heap[state->pc++];
     
     return ss.str();
}

void _printf_obj(slong ptr, char form, scorpion_state* state)
 {
     if((unsigned int)ptr>=state->size_t) segfault(state);
     if(!state->heap[ptr].alloc) { cout << LLONG_MAX; return; }
     if(state->heap[ptr].array || form == 'p')
     {
         printf("%#x", (unsigned int) ptr);
         return;
     }
     
     sdouble v = sValue(&state->heap[ptr]);
     if(form == 'i')
       cout << (sdouble) ((sint) v);
     else if(form == 's')
       cout << (sdouble) ((sshort) v);
     else if(form == 'B')
       cout << (sdouble) ((byte) v);
     else if(form == 'l')
       cout << (sdouble) ((slong) v);
     else if(form == 'S' || state->heap[ptr].o->type == primitive_string)
       _stdout(s_strValue(&state->heap[ptr]), state);
     else if(form == 'c' || state->heap[ptr].o->type == primitive_char)
       cout << (char) v;
     else if(form == 'b' || state->heap[ptr].o->type == primitive_boolean)
       cout << (((sbool) v == 1) ? "true" : "false");
     else if(form == 'f' || state->heap[ptr].o->type == primitive_float)
       cout << std::setprecision((DMAX_P/2) - 1) << (sfloat) v;
     else if(form == 'd' || state->heap[ptr].o->type == primitive_double)
       cout << std::setprecision(DMAX_P) << (double) v;
     else
       cout << v;
 }
 
 void _stdout(string out, scorpion_state* state)
 {
    // cout << "outputting " << out << endl;
   for(unsigned long i = 0; i < out.size(); i++){
     if(out.at(i) != '\\'){
       cout << out.at(i);
       continue;
     }
     else {
        if(!((i + 1) < out.size()))
           return;
         
        i++;
        switch ( out.at(i) ){
            case '\\':
               cout << '\\'; continue;
            case 'n':
               cout << endl; continue;
            case 'b':
               cout << '\b'; continue;
            case 't':
               cout << '\t'; continue;
            case 'f':
               cout << '\f'; continue;
            case 'r':
               cout << '\r'; continue;
            case 'v':
               cout << '\v'; continue;
            case '"':
               cout << "\""; continue;
            case '\'':
               cout << "'"; continue;
            case '#':
               cout << std::flush; continue;
            case '[': // print variable data example:  $[v483;
               {
                 i++;
                 if(!((i) < out.size()))
                   return;
                   
                 int form = out.at(i);
                 stringstream  ss;
                 ss << "";
                 i++;
                 
                 for(unsigned long i2 = i; i < out.size(); i++){
                     if(isdigit(out.at(i2)))
                       ss << out.at(i2);
                     else{
                       if(ss.str() == "")
                          continue;
                       
                       slong addr = atol(ss.str().c_str());
                       _printf_obj(addr, form, state);
                       break;
                     }
                     i2++;
                 }
                 continue;
               }
            case '{':
               {
 				   i++;
				   stringstream  ss, colorstream;
				   long color=0;
 		           for(long i2 = i; i < out.size(); i++){
 			            if(isdigit(out.at(i2)))
 			               ss << out.at(i2);
 						else{
 						  if(ss.str() == "")
 							   continue;
 						  
 						  color = atoi(ss.str().c_str());
 						  break;
 						}
 					    i2++;
 		           }
 		           
					if(color == 0)
					  cout << "\033[0m";
					else
					{
					   if(color >=30 && color <=37)  {
					     colorstream << "\033[1;" << color << "m";
					     cout << colorstream.str();	
					   }
					   else if(color >=40 && color <=47)
					   {
						     color -=10;
					      colorstream << "\033[0;" << color << "m";
					      cout << colorstream.str();	
					   }
					}
 		           continue;
               }
        }
     }
   }
 }

int ch;
struct termios t_old, t_new;

void termios_reset_()
{
    tcgetattr(STDIN_FILENO, &t_old);
    t_new = t_old;
    t_new.c_lflag &= ~(ICANON | ECHO);
}

sint _getch_()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &t_new);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
    return ch;
}

void scorpion_vexecute(scorpion_state* v_state)
{
    v_state->func_depth++;
    log.LOGV("eso application running.");
    termios_reset_();
    
    slong i;
    
   newframe:  /* unused: this is for gui processing */
   
    /* main loop for Scorpion virtual machine */
    for( ;; )
    {
       exe_:
        if(v_state->pc>=v_state->iheap_t)
          protect(v_state,v_state->pc);
        
        i = v_state->i_heap[v_state->pc++];
        if(i>sMaxOpcodeLimit||i<0) protect(v_state, i);
        
        if((v_state->state == state_frozen && i != OP_ENDNO) ||
          (v_state->state == state_passive_frozen && !(i == OP_END || i == OP_IF)))
        {
          v_state->pc+=ARG_t(v_state,i);
          continue;
        }
        
        //printf("i = (%d)\n", (unsigned int)i);
        switch( i )
        {
            case OP_NOP: { continue; }
            case OP_END: {
                 if(v_state->if_count>0)
                   v_state->if_count--;
            
                 if(v_state->state == state_passive_frozen 
                    && (v_state->if_count==v_state->if_depth))
                   v_state->state = state_normal;
                
                 continue;
            }
            case OP_NO: { 
                 v_state->state = state_frozen;
                 continue;
            }
            case OP_ENDNO: {
               v_state->state = state_normal;
               continue;
            }
            case OP_HLT: {
                 quit( v_state );
                 continue;
            }
            case OP_RETURN: {
                 func_return(v_state->i_heap[(slong)v_state->pc++],
                     v_state);
                 continue;
            }
            case OP_COUT: {
                 _stdout(data(v_state->i_heap[(slong)v_state->pc++], v_state), v_state);
                 continue;
            }
            case OP_INC: {
                 sSet(&v_state->heap[(slong)v_state->i_heap[(slong)v_state->pc]],
                   sValue(&v_state->heap[(slong)v_state->i_heap[(slong)v_state->pc]])+1);
                 v_state->pc++;
                 continue;
            }
            case OP_DEC: {
                 sSet(&v_state->heap[(slong)v_state->i_heap[(slong)v_state->pc]],
                   sValue(&v_state->heap[(slong)v_state->i_heap[(slong)v_state->pc]])-1);
                 v_state->pc++;
                 continue;
            }
            case OP_IF: {
                 do_if(v_state);
                 continue;
            }
            case OP_PUSH: {
                 v_stack_op(v_state, i);
                 continue;
            }
            case OP_POP: {
                 v_stack_op(v_state, i);
                 continue;
            }
            case OP_KILL: {
                 long ptr = v_state->i_heap[v_state->pc++];
                 //TODO: kill object
                 continue;
            }
            case OP_NEG: {
                 long ptr = v_state->i_heap[v_state->pc++];
                 sSet(&v_state->heap[ptr], !((bool)sValue(&v_state->heap[ptr])));
                 continue;
            }
            case OP_JMP: {
                 v_state->pc = sValue(&v_state->heap[(slong)v_state->i_heap[v_state->pc++]]);
                 goto exe_;
            }
            case OP_CALL: {
                 v_state->func_depth++;
                 Invoke_Method(v_state);
                 continue;
            }
            case OP_MTHD: {
                 v_state->pc++;
                 continue;
            }
            case OP_LBL: {
                 v_state->pc++;
                 continue;
            }
            case OP_DELETE: {
                 delete_object(&v_state->heap[(slong)v_state->i_heap[v_state->pc++]]);
                 continue;
            }
            case OP_CONST: {
                 slong op = v_state->i_heap[v_state->pc++];
                 slong ptr = v_state->i_heap[v_state->pc++];
                 
                 create_object(OPTYPE(op), &v_state->heap[ptr]);
                 sSet(&v_state->heap[ptr], v_state->i_heap[v_state->pc++]);
                 continue;
            }
            case OP_STRCONST: {
                 slong ptr = v_state->i_heap[v_state->pc++];
              
                 create_object(primitive_string, &v_state->heap[ptr]);
                 s_strSet(&v_state->heap[ptr], data(v_state->i_heap[v_state->pc++], v_state));
                 continue;
            }
            case OP_ACONST: {
                 slong op = v_state->i_heap[v_state->pc++], ptr = v_state->i_heap[v_state->pc++],
                       ptr2 = v_state->i_heap[v_state->pc++];
              
                 create_object(OPTYPE(op), &v_state->heap[ptr], sValue(&v_state->heap[ptr2]));
                 continue;
            }
            case OP_CAST: {
                 slong ptr = v_state->i_heap[v_state->pc++], ptr2 = v_state->i_heap[v_state->pc++],
                      c = v_state->i_heap[v_state->pc++];
              
                 sSet(&v_state->heap[ptr], cast(c, sValue(&v_state->heap[ptr2])));
              continue;
            }
            case OP_JIF: {
                 do_jump(i,v_state);
            }
            case OP_JIT: {
                 do_jump(i,v_state);
            }
            case OP_LSHFT: {
                 long ptr = v_state->i_heap[v_state->pc++], ptr2 = v_state->i_heap[v_state->pc++];
                 sSet(&v_state->heap[ptr], ((slong)sValue(&v_state->heap[ptr])<<(slong)sValue(&v_state->heap[ptr2])));
                 continue;
            }
            case OP_RSHFT: {
                 long ptr = v_state->i_heap[v_state->pc++], ptr2 = v_state->i_heap[v_state->pc++];
                 sSet(&v_state->heap[ptr], ((slong)sValue(&v_state->heap[ptr])>>(slong)sValue(&v_state->heap[ptr2])));
                 continue;
            }
            case OP_CMP: {
                 long op = v_state->i_heap[v_state->pc++], ptr = v_state->i_heap[v_state->pc++];
                 sSet(&v_state->heap[ptr], op_compare(op,v_state));
                 continue;
            }
            case OP_THROW: {
                 long ptr = v_state->i_heap[v_state->pc++], ptr2 = v_state->i_heap[v_state->pc++];
                 runtime_err(v_state, s_strValue(&v_state->heap[ptr]), s_strValue(&v_state->heap[ptr2]));
                 continue;
            }
            case OP_CIN: {
                 long ptr = v_state->i_heap[v_state->pc++];
                 sSet(&v_state->heap[ptr], _getch_());
                 continue;
            }
            case OP_STR_APND: {
                 str_append(v_state);
                 continue;
            }
            case OP_ASSN: {
                 long ptr = v_state->i_heap[v_state->pc++], ptr2 = v_state->i_heap[v_state->pc++];
                 scorpion_Vassign(ptr,ptr2,v_state);
            }
            default: {
                segfault(v_state);
            }
        }
    }
}
