
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
#ifndef SCORPION_SYNTAX_ANALYZER_H
#define SCORPION_SYNTAX_ANALYZER_H

#include "clib/filestream.h"
#include "Tokenizer.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
using namespace std;


#define nullptr ((void *)0)

class SyntaxAnalyzer {
      std::string file, function;
      int analyzeResponse;
      
    public:
      int analyze(string &file, Tokenizer &tokenizer);
      void syntaxerror(string std_err, Tokenizer &tokenizer);
      string getFile(){ return file; }
      void setFile(string f){
          file = f;
      }
};

#endif // SCORPION_SYNTAX_ANALYZER_H


