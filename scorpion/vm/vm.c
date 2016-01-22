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

#define str_append(i, v_state) \
        { \
            stringstream ss; \
            ss << s_strValue(&v_state->heap[i.A]); \
            s_strSet(&v_state->heap[i.A], ""); \
             \
            if(isnumber(&v_state->heap[i.B])) \
            {  \
                ss << (char)sValue(&v_state->heap[i.B]); \
                s_strSet(&v_state->heap[i.A], ss.str()); \
            } \
            else if(isstring(&v_state->heap[i.B])) \
            { \
                ss << s_strValue(&v_state->heap[i.B]);  \
                s_strSet(&v_state->heap[i.A], ss.str()); \
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

void stack_pop(Instruction i, scorpion_state* v_state)
{
   if(v_state->sp < 0) runtime_err( v_state, "StackUnderflowException", "Scorpion stack space" );
   scorpion_Vassign(i.A,v_state->stack[v_state->sp--],v_state);
}

void stack_push(Instruction i, scorpion_state* v_state)
{
   if((v_state->sp+1) >= v_state->stack_t) 
      runtime_err( v_state, "StackOverflowException", "The stack has overflowed with too much data" );
   v_state->stack[++v_state->sp] = i.A;
}

void setvalue(int op, slong ptr, scorpion_state* v_state, double v, double v2)
{
    if(op==OP_ADD) sSet(&v_state->heap[ptr], (v+v2));
    else if(op==OP_SUB) sSet(&v_state->heap[ptr], (v-v2));
    else if(op==OP_MULT) sSet(&v_state->heap[ptr], (v*v2));
    else if(op==OP_DIV) sSet(&v_state->heap[ptr], (v/v2));
    else if(op==OP_MOD) sSet(&v_state->heap[ptr], ((slong)v%(slong)v2));
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
    else if(op == OP_BIT_CONST || op == OP_BIT_ACONST)
       return primitive_bit;
    else return 1111;
}

#define func_return(i, v_state) { \
            --v_state->func_depth; \
            if((i.A == 0) && (v_state->func_depth <= 0)) { \
              log.LOGA("Exiting main() function: __init__"); \
              quit( v_state ); \
            } \
            else _return(i.A, v_state); \
        }

#define v_stack_op(v_state, i) \
        { \
            if(i.O == OP_PUSH) \
              stack_push(i, v_state); \
            else \
              stack_pop(i, v_state); \
        } 

#define do_jump(i,v_state) \
        { \
          if((bool)sValue(&v_state->heap[i.B]) == ((i.O==OP_JIT) ? true : false)) \
             v_state->pc = sValue(&v_state->heap[i.A]); \
           goto exe_; \
        }

#define arith_op(i, v_state) \
          setvalue(i.O, i.A, v_state, sValue(&v_state->heap[i.B]), sValue(&v_state->heap[(slong)i.C]));
        
bool op_compare(Instruction i, scorpion_state* v_state)
{
    slong op = i.A;
    if(isnumber(&v_state->heap[i.B]) && isnumber(&v_state->heap[(slong)i.C]))
      return s_numcompare(op,sValue(&v_state->heap[i.B]), sValue(&v_state->heap[(slong)i.C]));
    else if(isstring(&v_state->heap[i.B]) && isstring(&v_state->heap[(slong)i.C]))
      return s_strcompare(op,s_strValue(&v_state->heap[i.B]), s_strValue(&v_state->heap[(slong)i.C]));
    else return false;
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

void Invoke_Method(Instruction i, scorpion_state* state)
{
    unsigned long ptr = i.A, line = i.B;
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

void do_if(Instruction i, scorpion_state* v_state)
{
    if(v_state->state == state_normal &&
        ((bool)sValue(&v_state->heap[i.A])) == 0)
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
     else if(form == 'I' || state->heap[ptr].o->type == primitive_bit)
       cout << (sdouble) ((bool)v);
     else
       cout << (sdouble)v;
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
    
    Instruction i;
    
   newframe:  /* unused: this is for gui processing */
   
    /* main loop for Scorpion virtual machine */
    for( ;; )
    {
       exe_:
        if(v_state->pc>=v_state->iheap_t)
          protect(v_state,v_state->pc);
        
        i = v_state->i_heap[v_state->pc++];
        if((v_state->state == state_frozen && i.O != OP_ENDNO) ||
          (v_state->state == state_passive_frozen && !(i.O == OP_END || i.O == OP_IF)))
          continue;
        
        //printf("i = (%d)\n", (unsigned int)i.O);
        switch( i.O )
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
                 func_return(i, v_state);
                 continue;
            }
            case OP_COUT: {
                 _stdout(i.data.str(), v_state);
                 continue;
            }
            case OP_INC: {
                 sSet(&v_state->heap[i.A], sValue(&v_state->heap[i.B])+1);
                 continue;
            }
            case OP_DEC: {
                 sSet(&v_state->heap[i.A], sValue(&v_state->heap[i.B])-1);
                 continue;
            }
            case OP_IF: {
                 do_if(i, v_state);
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
                 //TODO: kill object
                 continue;
            }
            case OP_NEG: {
                 sSet(&v_state->heap[i.A], !((bool)sValue(&v_state->heap[i.A])));
                 continue;
            }
            case OP_JMP: {
                 v_state->pc = sValue(&v_state->heap[i.A]);
                 goto exe_;
            }
            case OP_CALL: {
                 v_state->func_depth++;
                 Invoke_Method(i, v_state);
                 continue;
            }
            case OP_MTHD: {
                 continue;
            }
            case OP_LBL: {
                 continue;
            }
            case OP_DELETE: {
                 delete_object(&v_state->heap[i.A]);
                 continue;
            }
            case OP_STR_X2I: {
                 sSet(&v_state->heap[i.A], atol(s_strValue(&v_state->heap[i.B]).c_str()));
                 continue;
            }
            case OP_STR_X2F: {
                 sSet(&v_state->heap[i.A], atof(s_strValue(&v_state->heap[i.B]).c_str()));
                 continue;
            }
            case OP_ADD: {
                 arith_op(i, v_state);
                 continue;
            }
            case OP_SUB: {
                 arith_op(i, v_state);
                 continue;
            }
            case OP_MULT: {
                 arith_op(i, v_state);
                 continue;
            }
            case OP_DIV: {
                 arith_op(i, v_state);
                 continue;
            }
            case OP_MOD: {
                 arith_op(i, v_state);
                 continue;
            }
            case OP_CONST: {
                 create_object(OPTYPE(i.A), &v_state->heap[i.B], 0, false);
                 sSet(&v_state->heap[i.B], i.C);
                 continue;
            }
            case OP_STRCONST: {
                 create_object(primitive_string, &v_state->heap[i.A]);
                 s_strSet(&v_state->heap[i.A], i.data.str());
                 continue;
            }
            case OP_ACONST: {
                 create_object(OPTYPE(i.A), &v_state->heap[i.B], sValue(&v_state->heap[(slong)i.C]), true);
                 continue;
            }
            case OP_CAST: {
                 sSet(&v_state->heap[i.A], cast(i.C, sValue(&v_state->heap[i.B])));
              continue;
            }
            case OP_JIF: {
                 do_jump(i,v_state);
            }
            case OP_JIT: {
                 do_jump(i,v_state);
            }
            case OP_LSHFT: {
                 sSet(&v_state->heap[i.A], ((slong)sValue(&v_state->heap[i.A])<<(slong)sValue(&v_state->heap[i.B])));
                 continue;
            }
            case OP_RSHFT: {
                 sSet(&v_state->heap[i.A], ((slong)sValue(&v_state->heap[i.A])>>(slong)sValue(&v_state->heap[i.B])));
                 continue;
            }
            case OP_CMP: {
                 sSet(&v_state->heap[i.Ax], op_compare(i,v_state));
                 continue;
            }
            case OP_THROW: {
                 runtime_err(v_state, s_strValue(&v_state->heap[i.A]), s_strValue(&v_state->heap[i.B]));
                 continue;
            }
            case OP_CIN: {
                 sSet(&v_state->heap[i.A], _getch_());
                 continue;
            }
            case OP_STR_APND: {
                 str_append(i, v_state);
                 continue;
            }
            case OP_ASSN: {
                 scorpion_Vassign(i.A,i.B,v_state);
                 continue;
            }
            case OP_AT: {
                 sSet(&v_state->heap[i.A], sAt(&v_state->heap[i.B], sValue(&v_state->heap[(slong)i.C])));
                 continue;
            }
            case OP_ALOAD: {
                 if(isnumber(&v_state->heap[i.A]))
                   sSet(&v_state->heap[i.A], sValue(&v_state->heap[i.B], 
                      sValue(&v_state->heap[(slong)i.C])));
                 else if(isstring(&v_state->heap[i.A]))
                   s_strSet(&v_state->heap[i.A], s_strValue(&v_state->heap[i.B], 
                      sValue(&v_state->heap[(slong)i.C])));
                 continue;
            }
            case OP_ASTORE: {
                 if(isnumber(&v_state->heap[i.A]))
                   sSet(&v_state->heap[i.A], sValue(&v_state->heap[i.B]), 
                      sValue(&v_state->heap[(slong)i.C]));
                 else if(isstring(&v_state->heap[i.A]))
                   s_strSet(&v_state->heap[i.A], s_strValue(&v_state->heap[i.B]),
                      sValue(&v_state->heap[(slong)i.C]));
                 continue;
            }
            default: {
                segfault(v_state);
            }
        }
    }
}
