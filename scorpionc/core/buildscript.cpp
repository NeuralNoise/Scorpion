#include "clib/filestream.h"
#include <string>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

int index = 0;

string dev = "";

int devparse(string file, string contents);

// Attributes
string name = "TestApplication";
int minDevVersion = 7;
int targetDevVersion = 7;
string version_num = "0.0.1.0";
bool debug = false, loog = true;
int logprecedence = 7;
string logfile = "/usr/share/scorpion/log.txt";
int compile_size = 0;
string compilefiles[2950]; // TODO: implement limit for max compiled files and max permissions
int permission_size = 0;
string io_permissions[3950];
int include_size = 0;
string include = "";
string appdir, srcdir;
string applicationid, exportFile = "@#?--!";

int response = 0;
int linenum = 0;
bool print = true;
string file_name = "build.dev";

// ------------------------ Attribute boolean
bool nme = false, mdv = false, tdv = false,
     vn = false, db = false, lp = false, lg = false, lf = false;
bool app = false, src = false, aid = false, expt = false;

bool eof = false;
string nextLine()
{
   stringstream str;
   str << "";
   for(int i = index; i < dev.length(); i++)
   {
     if((dev.at(i) == '\n') || (dev.at(i) == 10)){
       index++;
       return str.str();
     }
     else str << dev.at(i);
     if(!((i + 1) < dev.length())){
       index++;
       return str.str();
     }
     index++;
   }
  eof = true;
  return "--?";
}

int getcompile_size()
{
   return compile_size;
}

bool iswhitespace(char c)
{
   if((c == '\t') || (c == ' ') || (c == '\n'))
     return true;
  return false;
}

bool isbreaker(char c)
{
   if(c == '{' || c == '}')
     return true;
  return false;
}

bool iscomment(char c)
{
   if(c == '#')
     return true;
  return false;
}

string lexedChars[100];
int charindex = 0, charsize = 0;
int scope = 0;
bool instring = false;

void lex(string line)
{
  line.append(" \n");
  stringstream chars;
  for(int i = 0; i < line.length(); i++)
  {
    if(line.at(i) == '\''){
        if(instring)
          instring = false;
        else
          instring = true;
    }

    if((iswhitespace(line.at(i)) || isbreaker(line.at(i))) && !instring){
      if(line.at(i) == '{')
        scope++;
      else if(line.at(i) == '}')
        scope--;
      if(chars.str() != ""){
         lexedChars[ charindex++ ] = chars.str();
         charsize++;
         chars.str("");
      }
    }
    else if(iscomment(line.at(i))){
      if(chars.str() != ""){
         lexedChars[ charindex++ ] = chars.str();
         charsize++;
         chars.str("");
      }
      break;
    }
    else
     chars << "" << line.at(i);
  }

}

string getchars()
{
   stringstream ss;
   for(int i = 0; i < charsize; i++)
   {
      ss << ", " << lexedChars[i] << " ";
   }
  return ss.str();
}

bool isodd(int x)
{
  if((x % 2) != 0)
   return true;
  else
  return false;
}

#define ATTRIBS 15
string attribs[ATTRIBS] = { "name", "minDevVersion", "targetDevVersion", "versionNumber",
                   "debug", "log", "logPrecedence", "logFile", "compile", "permission",
                   "app:", "src:", "include", "applicationId", "export" };

stringstream errormsg, warningmsg;
int warnings, errors;

void deverror(string message)
{
    response = -1;
	errors++;
    errormsg << "build error: " << file_name  << ":" << linenum << "  " << message << endl;
}

void devwarning(string message)
{
	warnings++;
    warningmsg << "warning: " << file_name  << ":" << linenum << "  " << message << endl;
}

bool hasstring(string attrib)
{
   for(int i = 0; i < ATTRIBS; i++){
      if(attrib == attribs[i])
         return true;
   }
  return false;
}

bool isattribute(string attb)
{
   if(hasstring(attb))
    return true;
  return false;
}

string var1, var2;

// The different Tag types
int TYPE_STRING = 1;
int TYPE_NUMBER = 2;
int TYPE_SPECIFIC = 3;

int ETYPE = 0;

void settag_type(string attrib)
{
   if(attrib == attribs[0]) // name
    ETYPE = TYPE_STRING;
   else if(attrib == attribs[1]) // minDevVersion
    ETYPE = TYPE_NUMBER;
   else if(attrib == attribs[2]) // targetDevVersion
    ETYPE = TYPE_NUMBER;
   else if(attrib == attribs[3]) // versionNumber
    ETYPE = TYPE_STRING;
   else if(attrib == attribs[4]){ // debug
    ETYPE = TYPE_SPECIFIC;
    var1 = "true";
    var2 = "false";
   }
   else if(attrib == attribs[5]){ // log
    ETYPE = TYPE_SPECIFIC;
    var1 = "true";
    var2 = "false";
   }
   else if(attrib == attribs[6]) // logPrecedence
    ETYPE = TYPE_NUMBER;
   else if(attrib == attribs[7]) // logFile
    ETYPE = TYPE_STRING;
   else if(attrib == attribs[8]) // compile
    ETYPE = TYPE_STRING;
   else if(attrib == attribs[9]) // permission
    ETYPE = TYPE_STRING;
   else if(attrib == attribs[10]) // app:
    ETYPE = TYPE_STRING;
   else if(attrib == attribs[11]) // src:
    ETYPE = TYPE_STRING;
   else if(attrib == attribs[12]) // include
    ETYPE = TYPE_STRING;
   else if(attrib == attribs[13]) // applicationId
    ETYPE = TYPE_STRING;
   else if(attrib == attribs[14]) // export
    ETYPE = TYPE_STRING;
}

bool isstring(string data)
{
   if((data.at(0) == '\'') && (data.at(data.length() -1) == '\''))
     return true;
   else if(((data.at(0) == '(') && (data.at(data.length() -1) == ')')) &&
           ((data.at(1) == '\'') && (data.at(data.length() -2) == '\'')))
     return true;
  return false;
}

bool isdigit(char c)
{
  if(c == '0')
    return true;
  else if(c == '1')
    return true;
  else if(c == '2')
    return true;
  else if(c == '3')
    return true;
  else if(c == '4')
    return true;
  else if(c == '5')
    return true;
  else if(c == '6')
    return true;
  else if(c == '7')
    return true;
  else if(c == '8')
    return true;
  else if(c == '9')
    return true;
  else
    return false;
}

bool isnumber(string data)
{
  for(int i = 0; i < data.length(); i++)
  {
    if(!isdigit(data.at(i)))
     return false;
  }
  return true;
}


bool isspecific(string data)
{
  if(data == var1 || data == var2)
     return true;
  return false;
}

int gettype(string data)
{
   if(isstring(data))
     return TYPE_STRING;
   else if (isnumber(data))
     return TYPE_NUMBER;
   else if(isspecific(data))
     return TYPE_SPECIFIC;
   else
     return -1;
}

bool isvalid_tag(string tg)
{
  if(ETYPE == gettype(tg))
   return true;
 return false;
}

string getstring(string data)
{
    if(data.at(0) == '('){
       stringstream ss;
       for(int i = 2; i < data.length() - 2; i++)
          ss << data.at(i);
       return ss.str();
    }
    else{
        stringstream ss;
       for(int i = 1; i < data.length() - 1; i++)
          ss << data.at(i);
       return ss.str();
    }
}

int getint(string data)
{
   return atoi(data.c_str());
}

bool getbool(string data)
{
    if(data == "true")
      return true;
    else
      return false;
}

bool dup(string f)
{
    for(int i = 0; i < compile_size; i++){
        if(f == compilefiles[i])
           return true;
    }
  return false;
}

void updateattrib(string attrib, string tag)
{
    if(attrib == attribs[0]){ // name
    if(nme == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       name = getstring(tag);
       nme = true;
    }
   else if(attrib == attribs[1]){ // minDevVersion
    if(mdv == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       minDevVersion = getint(tag);
       mdv = true;
   }
   else if(attrib == attribs[2]){ // targetDevVersion
    if(tdv == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       targetDevVersion = getint(tag);
       tdv = true;
   }
   else if(attrib == attribs[3]){ // versionNumber
    if(vn == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       version_num = getstring(tag);
       vn = true;
   }
   else if(attrib == attribs[4]){ // debug
    if(db == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       debug = getbool(tag);
       db = true;
   }
   else if(attrib == attribs[5]){ // log
    if(lg == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       loog = getbool(tag);
       lg = true;
   }
   else if(attrib == attribs[6]){ // logPrecedence
    if(lp == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       logprecedence = getint(tag);
       lp = true;
   }
   else if(attrib == attribs[7]){ // logFile
    if(lf == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       stringstream ss;
       ss << getstring(tag);
       logfile = ss.str();
       lf = true;
   }
    else if(attrib == attribs[8]){ // compile
     if(compile_size == 0)
       cout<< "   build:" << attrib << "=UPDATED" << endl;
     
        string lib = getstring(tag);
       if(!dup(lib))
         compilefiles[compile_size++] = lib;
       else {
           stringstream ss;
           ss << " duplicate SAR file found.\n\t file: '" << lib << "' has already been set to compile, skipping.";
           devwarning(ss.str());
       }
   }
   else if(attrib == attribs[9]){ // permission
         if(permission_size == 0)
           cout<< "   build:" << attrib << "=UPDATED" << endl;

           io_permissions[permission_size++] = getstring(tag);
   }
   else if(attrib == attribs[10]){ // app:
    if(app == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       appdir = getstring(tag);
       app = true;
   }
   else if(attrib == attribs[11]){ // src:
    if(src == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       srcdir = getstring(tag);
       src = true;
   }
   else if(attrib == attribs[12]){ // include
           include = getstring(tag); // include dev file
           
           if(FileStream::exists(include.c_str())){
               print = false;
               string f = file_name;
               string d = dev;
               long i = index, l = linenum;
               charindex = 0;
               charsize = 0;
              // cout << "dev: \n" << dev;
               devparse(include, FileStream::getfile(include.c_str()));
               eof = false;
               print = true;
               index = i;
               linenum = l;
               dev = d;
               file_name = f;
          //     cout << "include: " << include << endl;
           }
           else{
               stringstream ss;
               ss << "include file:" << include << " does not exist.";
               deverror(ss.str());
           }
   }
   else if(attrib == attribs[11]){ // export
    if(src == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       stringstream ss;
       ss << appdir << "/build/outputs/sar/" << getstring(tag);
       exportFile = ss.str();
       src = true;
   }
   else if(attrib == attribs[13]){ // applicationId
    if(aid == true){
       stringstream ss;
       ss << "attribute:" << attrib << " has already been set.";
       devwarning(ss.str());
    }
    else
      cout<< "   build:" << attrib << "=UPDATED" << endl;
       applicationid = getstring(tag);
       aid = true;
   }
}

string attrubute = "", tag = "";
void process()
{
  if(charsize == 0)
   return;
  else {
    if(charsize > 2){
       stringstream ss;
       ss << "attribute:" << lexedChars[0] << " has too many tags!";
       deverror(ss.str());
	   return;
    }
    attrubute = lexedChars[0];
    tag = lexedChars[1];

    if(isattribute(attrubute)){
       settag_type(attrubute);
       if(isvalid_tag(tag)){
          updateattrib(attrubute, tag);
       }
       else{
        stringstream ss;
        ss << "attribute:" << attrubute << " tag is invalid";
        deverror(ss.str());
       }
    }
    else{
      stringstream ss;
      ss << "attribute:" << attrubute << " is not a know attrubute";
      deverror(ss.str());
    }
  }
}

void printwarnings()
{
	cout << "WARNINGS " << warnings << endl;
	cout << warningmsg.str() << endl;
}

void printerrors()
{
	cout << "ERRORS " << errors << endl;
	cout << errormsg.str() << endl;
}

int count = 0;
string format(string data)
{
    if(count == 0){
        count++;
        stringstream ss;
        ss << data << " ";
        return ss.str();
    }
    else {
       count++;
       stringstream ss;
       ss << " : " << data << " ";
       return ss.str();
    }
}

void checkattribs()
{
   stringstream ss;
   ss << "";
   if(!nme)
      ss << format("name");
   if(!mdv)
      ss << format("minDevVersion");
   if(!tdv)
      ss << format("targetDevVersion");
   if(!vn)
      ss << format("versionNumber");

    if(!app)
      ss << format("app");

    if(!src)
      ss << format("src");

    if(!aid)
      ss << format("applicationId");

   if(ss.str() != ""){
       stringstream sm;
       sm << "missing required attribute(s): [ " << ss.str() << "]";
       deverror(sm.str());
   }
}

bool fileexists(const char* file)
{
    return FileStream::exists(file);
}

void libchk()
{
    if(compile_size != 0){

        for(int i = 0; i < compile_size; i++){
            stringstream ss;
            ss << appdir << compilefiles[i];
            if(!fileexists(ss.str().c_str())){
                 stringstream sm;
                 sm << "lib file: '" << compilefiles[i] << "' does not exist!";
                 deverror(sm.str());
            }
            else {
                // check extension
                if(!FileStream::endswith(".sar", compilefiles[i])){
                    stringstream sm;
                    sm << "lib file: '" << compilefiles[i] << "' is not a scorpion archive file!";
                    deverror(sm.str());
                }
            }
        }
    }
}

int devparse(string file, string contents)
{
  dev = "";
  dev = contents;
  file_name = file;
  if(print)
     cout << "Dev Build running..." << endl;
  linenum = 0;
  index = 0;
  while(!eof)
  {
        linenum++;
        stringstream line;
        line << nextLine();
        if(line.str() != "--?"){
            lex(line.str());
//            printf("line: %d charsize: %d [ %s ]\n", linenum, charsize, getchars().c_str());
            process();
            charindex = 0;
            charsize = 0;
        }
        else
          break;
  }

  if( scope > 0 ){
     stringstream ss;
     ss << "missing '}' at end of file!";
     deverror(ss.str());
  }
  else if( scope < 0 ){
     stringstream ss;
     ss << "unexpected char '}' at end of file.";
     deverror(ss.str());
  }
  else { }
  
  if(print){
      libchk();
      checkattribs();
    
      cout << endl;
      printwarnings();
      printerrors();
  }


 /* cout << "name:" << name << endl;
  cout << "minDevVersion:" << minDevVersion << endl;
  cout << "targetDevVersion:" << targetDevVersion << endl;
  cout << "versionNumber:" << version_number << endl;
  cout << "debug:" << debug << endl;
  cout << "log:" << loog << endl;
  cout << "logPrecedence:" << logprecedence << endl;
  cout << "logFile:" << logfile << endl;*/
 // cout << "scope: " << scope;
  return response;
}
