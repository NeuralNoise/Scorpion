
/*
 * Copyright (C) 2015 The Scorpion Programming Language
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Portions of the code surrounded by "// Begin Dalvik code" and
 * "// END Delvik code" are copyrighted and licensed separately, as
 * follows:
 *
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements. The ASF licenses this file to You
 * under the Apache License, Version 2.0 (the "License"); you may not use 
 * this file except in compliance with the License.  You may obtain a copy
 * of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "clib/filestream.h"
#include "SyntaxAnalyzer.h"
#include "Tokenizer.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
using namespace std;

int SyntaxAnalyzer::analyze(string &file, Tokenizer &tokenizer)
{
    analyzeResponse=0;
    // while(tokenizer.eof == false){ tokenizer.nextTok(); }
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    tokenizer.nextTok();
    syntaxerror("test err", tokenizer);
    
    return analyzeResponse;
}
    
void SyntaxAnalyzer::syntaxerror(string std_err, Tokenizer &tokenizer)
{
    analyzeResponse = -1;
    if(function != "")
       cout << " In fuction '" << function << "':\n";
       
    cout << file << ":" << tokenizer.getLine() << ": error: " << std_err << endl;
    cout << "   " << tokenizer.lineTxt.str() << endl;
    
    for(int i = 0; i < tokenizer.charptr - 1; i++)
      cout << " ";
    cout << "^\n";
}