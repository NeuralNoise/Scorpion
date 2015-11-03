#include <iostream>
#include <string>
#include <stdlib.h> 
#include <stdio.h>
#include <sstream>
using namespace std;

#define MAX_PROTO ((128* 64) * 8) - 24

// Here we define our global vairable that will be used to hold our
// Language source code
static string scorpion = "@ Define the main function\n\timport macros.System; class Main base Activity \nend \nextern main(int i, string **args, bool h, char f, short i); \ndef main(int *x, int num,string **count, string name) \ndef main(int *x, int num,string **count, string name) throws Exception \nend \nendclass\n";
static string filename = "test.scorpion";

// get the next char from the source code being processed
// if the current indexof the source code is at the end, return EOF 
static long index = 0, line = 0, linedx = 0, prevdx, flg = 1;
static std::string line_str = "";
std::string prevlne;

long DATA_SZ = (124 * 64) * 8;
bool reset = false, array_var = false;
int lastlne = 0;

void lupdate()
{
       prevlne = line_str;
       prevdx = linedx;
       linedx = 0;
       line_str = "";
       for(int i = index; i < scorpion.size(); i++){
           if(scorpion.at(i) != '\n' || scorpion.at(i) != 10)
              line_str += scorpion.at(i);
           else{
            line++;
            break;
           }
       }
}

int getch()
{
    if(!(index < scorpion.size()))
        return EOF;
    
    linedx++;    
    if(reset){
       lastlne = 1;
       reset = false;
       lupdate();
      // cout << line_str << endl;
    }
    else
      lastlne = 0;
    
    reset = (scorpion.at(index) == '\n') ? true : false;
    return (int) scorpion.at(index++);
}

// Define the Types of tokens we are expecting to recieve back from the lexer
// Any unknown tokens will just retern their numeric value of [0-255]
// The lexer will provide the returned token, its numeric value and possibly some
// metadata such as the value of a number
enum Token{
    tok_eof = 0,
    
    tok_def = -1,
    tok_extern = -2,
    tok_class = -3,
    tok_end = -4,
    tok_endclass = -5,
    tok_int = -6,
    tok_string = -7,
    tok_void = -8,
    tok_throws = -9,
    tok_base = -10,
    tok_import = -11,
    tok_enum = -12,
    tok_double = -13,
    tok_float = -14,
    tok_short = -15,
    tok_bool = -16,
    tok_char = -17,
    tok_for = -18,
    tok_while = -19,
    tok_true = -20,
    tok_false = -21,
    tok_mod = -22,
    tok_try = -23,
    tok_catch = -24,
    tok_string_literal = -25,
    tok_new = -26,
    
    tok_identifier = -1134,
    tok_number = -2821,
};

std::string tokenIdentifier;
std::string tokenString;
double tokenNumVal;

static int LastChar = ' ';
static int getTok(){ 
    static std::string IdentifierStr;
    
    // Skip any whitespace.
    while (isspace(LastChar))
      LastChar = getch();
    
    if(LastChar == '"'){
        IdentifierStr = LastChar;
        do{
            LastChar = getch();
            IdentifierStr += LastChar;
        }
        while (LastChar != EOF && LastChar != '\n' && LastChar != '"');
        
        if(LastChar == EOF)
           return tok_eof;
           
    
        tokenString = IdentifierStr;
        LastChar = ' '; // weird trick to not confuse the compiler
        return tok_string_literal;
    }
    
    if(isalpha(LastChar)){  // identifier: [a-zA-Z][a-zA-Z0-9]*
         IdentifierStr = LastChar;
          while (isalnum((LastChar = getch()))){
            IdentifierStr += LastChar;
          }
         
         // Scorpion Language v.1.0 revision 7 reserved words
         if (IdentifierStr == "def")
            return tok_def;
         if (IdentifierStr == "extern")
            return tok_extern;
         if (IdentifierStr == "class")
            return tok_class;
         if (IdentifierStr == "end")
            return tok_end;
         if (IdentifierStr == "endclass")
            return tok_endclass;
         if (IdentifierStr == "int")
            return tok_int;
         if (IdentifierStr == "string")
            return tok_string;
         if (IdentifierStr == "void")
            return tok_void;
         if (IdentifierStr == "throws")
            return tok_throws;
         if (IdentifierStr == "base")
            return tok_base;
         if (IdentifierStr == "import")
            return tok_import;
         if (IdentifierStr == "enum")
            return tok_enum;
         if (IdentifierStr == "bool")
            return tok_bool;
         if (IdentifierStr == "float")
            return tok_float;
         if (IdentifierStr == "double")
            return tok_double;
         if (IdentifierStr == "short")
            return tok_short;
         if (IdentifierStr == "char")
            return tok_char;
         if (IdentifierStr == "for")
            return tok_for;
         if (IdentifierStr == "while")
            return tok_while;
         if (IdentifierStr == "true")
            return tok_true;
         if (IdentifierStr == "false")
            return tok_false;
         if (IdentifierStr == "module")
            return tok_mod;
         if (IdentifierStr == "try")
            return tok_try;
         if (IdentifierStr == "catch")
            return tok_catch;
            
        tokenIdentifier = IdentifierStr;
        return tok_identifier;
    }
    
    if (isdigit(LastChar)) {   // Number: [0-9.]+
          std::string NumStr;
          do {
            NumStr += LastChar;
            LastChar = getch();
          } while (isdigit(LastChar) || LastChar == '.');
          
          if(LastChar == '.'){
              NumStr += LastChar;
              do {
                NumStr += LastChar;
                LastChar = getch();
              } while (isdigit(LastChar));
          }
          
          tokenNumVal = strtod(NumStr.c_str(), 0);
          return tok_number;
    }
    
    if (LastChar == '@') {
      // Comment until end of line.
      do
        LastChar = getch();
      while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
    
      if (LastChar != EOF)
        return getTok();
    }
    
    
     // Check for end of file.  Don't eat the EOF.
      if (LastChar == EOF)
        return tok_eof;
  
      // Otherwise, just return the character as its ascii value.
      int ThisChar = LastChar;
      LastChar = getch();
      return ThisChar;
}



/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST {
  double val;

public:
  NumberExprAST(double Val){ val = Val; }
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST {
  std::string name;

public:
  VariableExprAST(const std::string &Name){ name = Name; }
};

struct unique_ptr {
    int exprType_l;
    const char *LHS;
    
    int exprType_r;
    const char *RHS;
}; 

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST {
  char Op;
  unique_ptr binary;

public:
  BinaryExprAST(char op, int lexpr, const char *expr1, int rexpr, const char *expr2)
  { Op = op;
    binary.exprType_l = lexpr;
    binary.LHS = expr1;
    binary.exprType_r = rexpr;
    binary.RHS = expr2;
  }
};



/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
 public:
  std::string Name;
  std::string Class;
  std::string *Args;
  std::string *ATypes;
  int *Ptr;
  std::string BME; // the base Exception class
  int argsize_t;
  string construct;

public:
  void addPrototype(std::string c, const std::string &name, std::string *args, std::string *aTypes, 
                    std::string _class, int *ptr, string bme, int asz)
  {
      Name = name;
      Args = args;
      ATypes = aTypes;
      Class = _class;
      Ptr = ptr;
      BME = bme;
      argsize_t = asz;
      construct = c;
  }
};

string *externs;
stringstream obj;
string *objectf;
int objcount = 0;

/// CurTok/getNextToken - Provide a simple token buffer.  CurTok is the current
/// token the parser is looking at.  getNextToken reads another token from the
/// lexer and updates CurTok with its results.
static int CurTok;
static int getNextToken() {
  return CurTok = getTok();
}

#define nullptr ((void *)0)

#define SPACE_SIZE 4

PrototypeAST prototypes[MAX_PROTO];
long prototypesize_t = 0;

void earrow()
{
   for(int i = 0; i < SPACE_SIZE; i++)
       cout << " ";

   if(lastlne == 0){       
       for(int i = 0; i < linedx;  i++)
          cout << " ";
   }
   else{
       for(int i = 0; i < prevdx;  i++)
          cout << " ";
   }
    cout << "\b^\n";
}

/// Error* - These are little helper functions for error handling.
bool Error(const char *Str) {
  if(lastlne == 0)
      printf("%s:%lu:%lu error:  %s\n    %s\n", filename.c_str(), line - 10, linedx, Str, line_str.c_str());
  else
      printf("%s:%lu:%lu error:  %s\n    %s\n", filename.c_str(), line - 10, prevdx, Str, prevlne.c_str());

  earrow();
  lastlne=0;
  return false;
}
bool ErrorP(const char *Str) {
  return Error(Str);
}

/// numberexpr ::= number.number
static double ParseNumberExpr() {
  double Result = tokenNumVal;
  getNextToken(); // consume the number
  return Result;
}

bool isescape(char c) // t n \ r b & '
{
    if(c == 'n')
      return true;
    if(c == 't')
      return true;
    if(c == '\\')
      return true;
    if(c == 'r')
      return true;
    if(c == 'b')
      return true;
    if(c == '&')
      return true;
    if(c == '\'')
      return true;
    if(c == '"')
      return true;
   return false;
}


string ttostr(int v)
{
    if(v == tok_int)
      return "integer";
    else if(v == tok_bool)
      return "boolean";
    else if(v == tok_char)
      return "character";
    else if(v == tok_double)
      return "double integer";
    else if(v == tok_float)
      return "float integer";
    else if(v == tok_short)
      return "short integer";
    else if(v == tok_string)
      return "string";
    else
       return "invalid";
}

bool isexpression(int expr)
{
   if(expr == tok_int || expr == tok_bool ||expr == tok_char || expr == tok_double 
      || expr == tok_float || expr == tok_short || expr == tok_string)
      return true;
  return false;
}

struct exprTable {
   string name;
   int ptr;
   string type;
};

exprTable table[3000]; // temporary expression defining tables
int exprsize_t = 0;
int externsize_t = 0;

// expression ::= expr identifier
bool ParseExpression()
{
  int ptr = 0;
  if(isexpression(CurTok)){
       if(CurTok == tok_int || CurTok == tok_bool ||CurTok == tok_char || CurTok == tok_double 
          || CurTok == tok_float || CurTok == tok_short || CurTok == tok_string){
           
           int type = CurTok;
           if(scorpion.at(index++) == '*'){ // optional pointer addon
                // expr ::= expr identifier* <identifier_ptr>
               ptr++;
               if(scorpion.at(index++) == '*') // optional pointer array addon
                    ptr++;  // expr ::= expr identifier** <identifier-array_ptr>
               else
                index--;
           }
           else
            index--;
            getNextToken();
                  
           if(CurTok == tok_identifier){ 
               // add to expression table
               table[exprsize_t].name = tokenIdentifier;
                table[exprsize_t].ptr = ptr;
                 table[exprsize_t].type = ttostr(type);
                 exprsize_t++;
           }
           else
             return Error("expected unique-identifier.");
       } 
       else
         return Error("expected expression.");
  }
  
   getNextToken();
   return true;
}

std::string CurPrototype, CurBaseMethodException, CurClass, CurMod, CurBase, CurImport;
/// prototype ::= '(' expression , expression1..')'
bool ParsePrototype() {
  
  if(CurTok == tok_identifier){
     // printf("def %s\n", tokenIdentifier.c_str());
      CurPrototype = tokenIdentifier;
  }
  else
      return Error("expected unique-identifier.");
      
  getNextToken(); // eat (.

  if (CurTok != '(')
    return Error("expected '('.");
  
  while(1){
    getNextToken();
    
    if(isexpression(CurTok)){
      bool ok = ParseExpression(); // parse the expr
      if(!ok)
         return false;
      
      if(CurTok == ',')
        continue;
      if(CurTok == '.'){
          int i = index;
          getNextToken();
          if(CurTok == '.'){
              getNextToken();
              array_var = true;
              if(CurTok == ',')
                continue;
              else if(CurTok == ')'){ 
                 int s = index;
                  getNextToken(); // eat throws
                  
                  if(CurTok == tok_throws){ // optional
                      getNextToken();
                      if(CurTok == tok_identifier){
                         CurBaseMethodException = tokenIdentifier; // verify if the ase method exception is a valid class
                        printf("method %s throws base class %s\n", CurPrototype.c_str(), CurBaseMethodException.c_str());
                      }
                      else
                         return Error("expected unique-identifier after throws.");
                  }
                  else
                    index = s;
                break; 
              }
              else{
                 stringstream ss;
                 if(LastChar == EOF)
                   ss << "expected ',' or ')' in function definition before end of file.";
                 else
                   ss << "unexpected char, expected ',' or ')' in function definition.";
                 return Error(ss.str().c_str());
              }
          }
          else {
              index = i;
              return Error("expected '.'");
          }
      }
      else if(CurTok == ')'){ 
         int s = index;
          getNextToken(); // eat throws
          
          if(CurTok == tok_throws){ // optional
              getNextToken();
              if(CurTok == tok_identifier){
                 CurBaseMethodException = tokenIdentifier; // verify if the ase method exception is a valid class
                printf("method %s throws base class %s\n", CurPrototype.c_str(), CurBaseMethodException.c_str());
              }
              else
                 return Error("expected unique-identifier after throws.");
          }
          else
            index = s;
        break; 
      }
      else{
         stringstream ss;
         if(LastChar == EOF)
           ss << "expected ',' or ')' in function definition before end of file.";
         else
           ss << "unexpected char, expected ',' or ')' in function definition.";
         return Error(ss.str().c_str());
      }
    }
    else if(CurTok == ')'){ 
      int s = index;
      getNextToken(); // eat throws
      
      if(CurTok == tok_throws){ // optional
          getNextToken();
          if(CurTok == tok_identifier){
             CurBaseMethodException = tokenIdentifier; // verify if the ase method exception is a valid class
            printf("method %s throws base class %s\n", CurPrototype.c_str(), CurBaseMethodException.c_str());
          }
          else
             return Error("expected unique-identifier after throws.");
      }
      else
        index = s;
      break; 
    }
    else {
         Error("expected expression or ')'.");
         char c;
         for(int i = 0; i < scorpion.size(); i++){
                c = getch();
                if(c == '\n' || c == ')')
                    break;
                
                else { } // skip to end of method decliration or new line
         }
         return false;
    }
  }

  getNextToken();
  return true;
}


void skiptoparen()
{
    char c;
      do
        c = getch();
      while (c != EOF && c != '\n' && c != ')');
}

bool ParseWhile()
{
    getNextToken();
    if(CurTok != '('){
       skiptoparen();
      getNextToken();
      return Error("expected '('"); 
    }
    
    
    
    // format ::= ( identifier ) | ( number )
    //           | ( boolean )
    getNextToken();
     if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok == ')'){ // ( identifier ) | ( boolean )
            // for loop ready
            getNextToken();
            return true;
        }
        else {
            skiptoparen();
            getNextToken();
            return Error("expected ')'");
        }
     }
     else if(CurTok == tok_number){
        getNextToken();
        if(CurTok == ')'){ // ( number )
            // for loop ready
            getNextToken();
            return true;
        }
        else {
            skiptoparen();
            getNextToken();
            return Error("expected ')'");
        }
     }
     else if(CurTok == tok_true){
        getNextToken();
        if(CurTok == ')'){ // ( true )
            // for loop ready
            getNextToken();
            return true;
        }
        else {
            skiptoparen();
            getNextToken();
            return Error("expected ')'");
        }
     }
     else if(CurTok == tok_false){
        getNextToken();
        if(CurTok == ')'){ // ( false )
            // for loop ready
            getNextToken();
            return true;
        }
        else {
            skiptoparen();
            getNextToken();
            return Error("expected ')'");
        }
     }
     else {
        skiptoparen();
        getNextToken();
        return Error("expected for loop format");
    }
}

// for loop ::= 'for' ( format )
bool ParseFor()
{
    getNextToken();
    if(CurTok != '('){
       skiptoparen();
      getNextToken();
      return Error("expected '('");
    }
    
    // format ::= ( identifier ) | ( identifier ':' number )
    //           | ( identifier ';;' identifier[incrementer|decrementer] )
    //           | ( identifier ';;')
    getNextToken();
    if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok == ')'){ // ( identifier )
            // for loop ready
            getNextToken();
            return true;
        }
        else if(CurTok == ':'){
             getNextToken();
             if(CurTok == tok_number){
                 getNextToken();
                 if(CurTok == ')'){ // ( identifier : number )
                    // for loop ready
                    getNextToken();
                    return true;
                 }
                 else {
                    getNextToken();
                    skiptoparen();
                    return Error("expected ')'");  
                 }
                 return true;
             }  
             else if(CurTok == tok_identifier){
                 getNextToken();
                 if(CurTok == ')'){ // ( identifier : identifier )
                    // for loop ready
                    getNextToken();
                    return true;
                 }
                 else {
                    getNextToken();
                    skiptoparen();
                    return Error("expected ')'");  
                 }
                 return true;
             }
             else{
               skiptoparen();
               getNextToken();
               return Error("expected numeric-value");
             }
        }
        else if(CurTok == ';'){ // ( identifier ';;' identifier[incrementer|decrementer] ) | ( identifier ';;' )
            getNextToken();
            if(CurTok == ';'){
                getNextToken();
                
                if(CurTok == ')'){
                    // for loop ready
                    getNextToken();
                    return true;
                }
                else if(CurTok == tok_identifier){
                    getNextToken();
                    char cc = CurTok;
                    long i = index;
                    getNextToken();
                    char c = CurTok;
                    if(CurTok == '+' && c == '+'){
                        getNextToken();
                        if(CurTok == ')'){
                            // for loop ready
                            getNextToken();
                            return true;
                        }  
                        else{
                          skiptoparen();
                          getNextToken();
                          return Error("expected ')'");
                        }
                    }
                    else if(CurTok == '-' && c == '-'){
                        getNextToken();
                        if(CurTok == ')'){
                            // for loop ready
                            getNextToken();
                            return true;
                        }  
                        else{
                          skiptoparen();
                          getNextToken();
                          return Error("expected ')'");
                        }
                    }
                    else{
                       index = i;
                       skiptoparen();
                       getNextToken();
                       return Error("expected incrementer or decrementer");
                     }
                    
                }  
                else{
                  skiptoparen();
                  getNextToken();
                  return Error("expected identifier or ')'--");
                }
            }
            else{
              skiptoparen();
              getNextToken();
              return Error("expected ';'");
            }
        }
    }
    // format ::= ( number )
    else if (CurTok == tok_number){
        getNextToken();
        if(CurTok == ')'){
            // for loop ready
            getNextToken();
            return true;
        }
        else{
           skiptoparen();
           getNextToken();
           return Error("expected ')'");
        }
    }
    // format ::=  ( ';;' )
    else if(CurTok == ';'){
         getNextToken();
        if(CurTok == ';'){
             getNextToken();
            if(CurTok == ')'){
                // for loop ready
                getNextToken();
                return true;
            }
            else{
               skiptoparen();
               getNextToken();
               return Error("expected ')'");
            }
        }
        else{
           skiptoparen();
           getNextToken();
           return Error("expected ';'");
        }
    }
    else {
        skiptoparen();
        getNextToken();
        return Error("expected for loop format");
    }
    
    getNextToken();
}

// Import ::= 'import' identifier ;
bool ParseImport()
{
  CurImport = "";
  bool dot = false;
  bool asterisk = false;
    while(1){
        getNextToken();
        
        if(CurTok == tok_identifier){
            if(asterisk == true)
               return Error("faulty import statement, cannot place a class after '*'.");
            CurImport += tokenIdentifier;
            int i = index;
            getNextToken();
            
            if(CurTok == '.'){
                CurImport += ".";
                dot = true;
                continue;
            }
            else if(CurTok == ';')
               return true;
            else{
              index = i;
              return Error("expected '.' or ';'.");
            }
          dot = false;
        }
        else if(CurTok == ';'){
           if(CurImport == "")
             return Error("expected identifier before ';'.");
           if(dot == true)
             return Error("expected identifier after '.'");
           return true;
        }
        else if(CurTok == '*' && (dot == true)){
            if(asterisk == true)
               return Error("faulty import statement, cannot place a asterisk after '*'.");
           dot = false;
           asterisk = true;
           CurImport += "*";
           int i = index;
            getNextToken();
            
            if(CurTok == '.'){
                CurImport += ".";
                dot = true;
                continue;
            }
            else if(CurTok == ';'){
               if(dot == true)
                 return Error("expected identifier after '.'");
               return true;
            }
            else{
              index = i;
              return Error("expected '.' or ';'.");
            }
        }
        else
          return Error("expected identifier.");
    }
}

/// definition ::= 'def' prototype ( expression )
bool ParseDefinition() {
  bool OK = true;
  getNextToken();  // eat identifier
  OK = ParsePrototype();
  
  return OK;
}

string ttpt(string s)
{
    if(s == "integer")
      return "int";
    else if(s == "boolean")
      return "bool";
    else if(s == "double integer")
      return "double";
    else if(s == "float integer")
      return "float";
    else if(s == "short integer")
      return "short";
    else if(s == "boolean")
      return "bool";
    else if(s == "string")
      return "string";
   return "invalid";
}

string getags()
{
   stringstream ss;
   ss << "";
   for(int i = 0; i < exprsize_t; i++){
       ss << ttpt(table[i].type) << " ";
       for(int c = 0; c < table[i].ptr; c++)
         ss << "*";
       ss << table[i].name;
       if(array_var){
        ss << "..";
        array_var = false;
       }
       ss << ", ";
   }
   if(exprsize_t != 0)
     ss << "\b\b";
   return ss.str();
}

string constr()
{
    stringstream ss;
    ss << CurPrototype << "(" << getags() << ") : " << CurClass;
    return ss.str();
}

void HandleDefinition()
{
   if (ParseDefinition()) {
     printf("def %s\n", CurPrototype.c_str());
    // check argument count and if the function throws a base exception class
        string protocon = constr(); // make a constructor string for method comparasin
        cout << protocon << endl;
        for(int i = 0; i < prototypesize_t; i++){
            if(prototypes[i].construct == protocon){
               stringstream ss;
               ss << " multiple definition of method '" << protocon << "'";
               lastlne = 1;
               Error(ss.str().c_str());
               return;
            }
        }
        
    /*    string *args = new string[exprsize_t];
        string *atypes = new string[exprsize_t];
        int *aptrs = new int[exprsize_t];
        for(int i = 0; i < exprsize_t; i++){
           args[i] = table[i].name;
           atypes[i] = table[i].type;
           aptrs[i] = table[i].ptr;
        }
        
        prototypes[prototypesize_t].addPrototype(protocon, CurPrototype, args, atypes, CurClass, aptrs, CurBaseMethodException, exprsize_t);
     //  create unique argument variables and have method require try()->catch() wrapping
   //  obj << "mthd <" << CurClass << ">@" << CurPrototype << endl;
    // obj << "mthd_1\n"; // start method body
     endscope++;
    CurBaseMethodException = "n/a";*/
    prototypesize_t++;
    exprsize_t = 0; // reset temp expresstion tabel size
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

/// external ::= 'extern' prototype
static bool ParseExtern() {
  getNextToken(); // eat extern.
  return ParsePrototype();
}

/// Class Def ::= 'class' identifier
static bool ParseClass(){
   getNextToken(); // eat class
   if(CurTok == tok_identifier){
       CurClass = tokenIdentifier;
       getNextToken();
       if(CurTok == tok_base){
          getNextToken();
          if(CurTok == tok_identifier){
             CurBase = tokenIdentifier;
             printf("class %s base %s\n", CurClass.c_str(), CurBase.c_str());
          }
          else{
               getNextToken();
               return Error("expected identifier.");
          }
        //  endclass_scope++;
          getNextToken();
       }
       return true;
   }
   else{
      getNextToken();
      return Error("expected identifier.");
   }
}

/// Module Def ::= 'module' identifier
static bool ParseMod(){
   getNextToken(); // eat module
   if(CurTok == tok_identifier){
       CurMod = tokenIdentifier;
       getNextToken();
       return true;
   }
   else{
      getNextToken();
      return Error("expected identifier.");
   }
}

// Boolean Def ::= 'bool' identifier '=' true ; |
//                       identifier '(' number ')' ;
static bool ParseBooleanDef()
{
    getNextToken();
    if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok == '='){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               return true;
           }
           else if(CurTok == tok_true){
               // var init ready
               getNextToken();
               return true;
           }
           else if(CurTok == tok_false){
               // var init ready
               getNextToken();
               return true;
           }
           else{
              getNextToken();
              return Error("expected number or true/false.");
           }
        }
        else if(CurTok == '('){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               if(CurTok != ')')
                 return Error("expected ')'");
               
               getNextToken();  
               return true;
           }
           else if(CurTok == tok_true){
               // var init ready
               getNextToken();
               if(CurTok != ')')
                 return Error("expected ')'");
               
               getNextToken(); 
               return true;
           }
           else if(CurTok == tok_false){
               // var init ready
               getNextToken();
               if(CurTok != ')')
                 return Error("expected ')'");
               
               getNextToken(); 
               return true;
           }
           else{
              getNextToken();
              return Error("expected number or true/false.");
           }
        }
        else{
          getNextToken();
          return Error("expected '=' or '('.");
        }
    }
    else{
      getNextToken();
      return Error("expected identifier.");
    }
}

// Character Def ::= 'char' identifier '=' character ; |
//                       identifier '(' character ')' ;
static bool ParseCharDef()
{
    getNextToken();
    if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok == '='){
           getNextToken();
           if(CurTok == '\''){
               getNextToken();
               if(CurTok == '\\'){
                   getNextToken();
                   if(!isescape((char) CurTok))
                      return Error("expected escape character.");
                   getNextToken();
                  if(CurTok == '\''){
                      // var init ready
                       getNextToken();
                       return true;
                  }
                  else{
                    getNextToken();
                    return Error("expected '''.");
                  }
               }
               else{
                  getNextToken();
                  if(CurTok == '\''){
                      // var init ready
                     getNextToken();
                     return true;
                  }
                  else{
                    getNextToken();
                    return Error("expected '''.");
                  }
               }
           }
           else{
              getNextToken();
              return Error("expected '''.");
           }
        }
        else if(CurTok == '('){
           getNextToken();
           if(CurTok == '\''){
               // var init ready
               getNextToken();
               if(CurTok == '\\'){
                   getNextToken();
                   if(!isescape((char) CurTok))
                      return Error("expected escape character.");
                   getNextToken();
                  if(CurTok == '\''){
                      // var init ready
                      getNextToken();
                     if(CurTok != ')')
                       return Error("expected ')'");
                   
                     getNextToken(); 
                     return true;
                  }
                  else{
                    getNextToken();
                    return Error("expected '''.");
                  }
               }
               else{
                  getNextToken();
                  if(CurTok == '\''){
                      
                      // var init ready
                      getNextToken();
                     if(CurTok != ')')
                       return Error("expected ')'");
                   
                     getNextToken(); 
                     return true;
                  }
                  else{
                    getNextToken();
                    return Error("expected '''.");
                  }
               }
           }
           else{
              getNextToken();
              return Error("expected '''.");
           }
        }
        else{
          getNextToken();
          return Error("expected '=' or '('.");
        }
    }
    else{
      getNextToken();
      return Error("expected identifier.");
    }
}

// Double Def ::= 'double' identifier '=' number ; |
//                       identifier '(' number ')' ;
static bool ParseDoubleDef()
{
    getNextToken();
    if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok == '='){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               return true;
           }
           else{
              getNextToken();
              return Error("expected number.");
           }
        }
        else if(CurTok == '('){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               if(CurTok != ')')
                 return Error("expected ')'");
               
               getNextToken();  
               return true;
           }
           else{
              getNextToken();
              return Error("expected number.");
           }
        }
        else{
          getNextToken();
          return Error("expected '=' or '('.");
        }
    }
    else{
      getNextToken();
      return Error("expected identifier.");
    }
}


// Float Def ::= 'float' identifier '=' number ; |
//                       identifier '(' number ')' ;
static bool ParseFloatDef()
{
    getNextToken();
    if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok == '='){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               return true;
           }
           else{
              getNextToken();
              return Error("expected number.");
           }
        }
        else if(CurTok == '('){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               if(CurTok != ')')
                 return Error("expected ')'");
               
               getNextToken();  
               return true;
           }
           else{
              getNextToken();
              return Error("expected number.");
           }
        }
        else{
          getNextToken();
          return Error("expected '=' or '('.");
        }
    }
    else{
      getNextToken();
      return Error("expected identifier.");
    }
}

// Integer Def ::= 'short' identifier '=' number ; |
//                       identifier '(' number ')' ;
static bool ParseShortDef()
{
    getNextToken();
    if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok == '='){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               return true;
           }
           else{
              getNextToken();
              return Error("expected number.");
           }
        }
        else if(CurTok == '('){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               if(CurTok != ')')
                 return Error("expected ')'");
               
               getNextToken();  
               return true;
           }
           else{
              getNextToken();
              return Error("expected number.");
           }
        }
        else{
          getNextToken();
          return Error("expected '=' or '('.");
        }
    }
    else{
      getNextToken();
      return Error("expected identifier.");
    }
}

// String Def ::= 'string' identifier '=' string-literal ; |
//                       identifier '(' string-literal ')' ;
static bool ParseStringDef()
{
    getNextToken();
    if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok == '='){
            getNextToken();
            if(CurTok == tok_string_literal){
                // var init ready
              cout << tokenString << endl;
               getNextToken();
               return true;
            }
            else{
              getNextToken();
              return Error("expected string literal.");
            }
        }
        else if(CurTok == '('){
            getNextToken();
            if(CurTok == tok_string_literal){
                // var init ready
              cout << tokenString << endl;
               getNextToken();
               
               if(CurTok != ')')
                 return Error("expected ')'.");
                 
               getNextToken();
               return true;
            }
            else{
              getNextToken();
              return Error("expected string literal.");
            }
        }
        else{
          getNextToken();
          return Error("expected '='.");
        }
    }
    else{
      getNextToken();
      return Error("expected identifier.");
    }
}

// High level expression parsing
// this will parse a high level expression based on numerious ammounts of possibilities
// example:  i++; str.size() i = 9; Class j = new Class();
bool ParseHighLevelExpression()
{
    // check if identifier is a valid class of variable
    getNextToken();
    if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok = ';'){
            // var init ready
            // Class klass;
            getNextToken();
            return true;
        }
        else if(CurTok == '='){
            getNextToken();
            if(CurTok = tok_new){
                getNextToken();
               // bool a = ParseConstruct();
                return true;
            }
            else {
                return Error("expected new.");
            }
        }
        else {
            return Error("expected ';'.");
        }
    }
    else if(CurTok == '+'){
        
    }
    else if(CurTok == '-'){
        
    }
    else if(CurTok == '='){
        
    }
    else{
        return Error("expected identifier.");
    }
}

// Integer Def ::= 'int' identifier '=' number ; |
//                       identifier '(' number ')' ;
static bool ParseIntDef()
{
    getNextToken();
    if(CurTok == tok_identifier){
        getNextToken();
        if(CurTok == '='){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               return true;
           }
           else{
              getNextToken();
              return Error("expected number.");
           }
        }
        else if(CurTok == '('){
           getNextToken();
           if(CurTok == tok_number){
               // var init ready
               getNextToken();
               if(CurTok != ')')
                 return Error("expected ')'");
               
               getNextToken();  
               return true;
           }
           else{
              getNextToken();
              return Error("expected number.");
           }
        }
        else{
          getNextToken();
          return Error("expected '=' or '('.");
        }
    }
    else{
      getNextToken();
      return Error("expected identifier.");
    }
}

/// Catch Def ::= 'catch' ( identifier (*)identifier )
static bool ParseCatch(){
   getNextToken(); // eat module
   if(CurTok != '(')
      return Error("expected '('");
   
   getNextToken();
   if(CurTok == tok_identifier){
       // this is our exception class
       getNextToken();
       
       if(CurTok == '*'){
           getNextToken();
           if(CurTok == tok_identifier){
               // exception id name
               getNextToken();
               if(CurTok != ')')
                  return Error("expected ')'");
                // catch ready
                getNextToken();
                return true;
           }
           else
             return Error("expected identifier");
       }
       else if(CurTok == tok_identifier){
           // exception id name
           getNextToken();
           if(CurTok != ')')
              return Error("expected ')'");
            // catch ready
            getNextToken();
            return true;
       }
       else 
           return Error("expected '*' or identifier");
   }
   else{
      getNextToken();
      return Error("expected identifier.");
   }
  return false;
}

void HandleExtern() {
  if (ParseExtern()) {
    printf("Parsed an extern %s\n", CurPrototype.c_str()); 
    // save function in external function map and check at end of compilation if the function is refrenced
  // string protocon = constr(); // make a constructor string
   // externs[externsize_t++] = protocon;
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleClass() {
  if (ParseClass()) {
    printf("Parsed a class %s\n", CurClass.c_str()); 
    // save function class and check for availability
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleImport() {
  if (ParseImport()) {
    printf("Parsed an import %s\n", CurImport.c_str()); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleFor() {
  if (ParseFor()) {
    printf("Parsed a for loop\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleWhile() {
  if (ParseWhile()) {
    printf("Parsed a while loop\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleMod() {
  if (ParseMod()) {
    printf("Parsed a module\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleCatch() {
  if (ParseCatch()) {
    printf("Parsed a catch\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleInt() {
  if (ParseIntDef()) {
    printf("Parsed a int def\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleBool() {
  if (ParseBooleanDef()) {
    printf("Parsed a boolean def\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleShort() {
  if (ParseShortDef()) {
    printf("Parsed a short def\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleString() {
  if (ParseStringDef()) {
    printf("Parsed a string def\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleChar() {
  if (ParseCharDef()) {
    printf("Parsed a char def\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleFloat() {
  if (ParseFloatDef()) {
    printf("Parsed a float def\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleDouble() {
  if (ParseDoubleDef()) {
    printf("Parsed a double def\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void HandleHighLevelExpression() {
  if (ParseHighLevelExpression()) {
    printf("Parsed a ligh level expression\n"); 
    // save function in external function map and check at end of compilation if the function is refrenced
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

void parse(string file, string source)
{
    filename = file;
    scorpion = source;
    lupdate();
    getNextToken();
    
    while(1){
      switch (CurTok) {
        case tok_eof:
          return;
        case ';': // ignore top-level semicolons.
          getNextToken();
          break;
        case tok_def:
          HandleDefinition();
          break;
        case tok_extern:
          HandleExtern();
          break;
        case tok_class:
          HandleClass();
          break;
        case tok_end:
          // no need to do much
          printf("Parsed a end\n");
         getNextToken();
          break;
        case tok_endclass:
          CurClass = "";
          printf("Parsed a  endclass\n");
          getNextToken();
          break;
        case tok_import:
          HandleImport();
          break;
        case tok_for:
          HandleFor();
          break;
        case tok_while:
          HandleWhile();
          break;
        case tok_mod:
          HandleMod();
          break;
        case tok_try:
          // handle try
          cout << "parsed try\n";
          getNextToken();
          break;
        case tok_catch:
          HandleCatch();
          break;
        case tok_int:
          HandleInt();
          break;
        case tok_bool:
          HandleBool();
          break;
        case tok_short:
          HandleShort();
          break;
        case tok_string:
          HandleString();
          break;
        case tok_char:
          HandleChar();
          break;
        case tok_float:
          HandleFloat();
          break;
        case tok_double:
          HandleDouble();
          break;
        case tok_identifier:
          HandleHighLevelExpression();
          break;
        default:
        //  if(linedx == 0)
        //    lastlne = 1;
           Error("unexpected token, expected expression or statement.");
          getNextToken();
          break;
      }
  }
    return;
}
