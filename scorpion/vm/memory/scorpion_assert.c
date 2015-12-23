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
 
 #include <string>
 #include <stdint.h>
 #include "allocation_scheme.h"
 #include "gc.h"
 #include "../io/signal_handler.h"
 #include "object_container.h"
 #include "block_allocator.h"
 #include "../../clib/arraylist.h"
 #include "scorpion_assert.h"
 
 using namespace std;
 using namespace scorpionvm::io::signal;
 
  #define nullptr ((void *)0)
  
  void scorpionvm::scorpion_assert(bool case_, std::string case_source)
  {
      if(!case_)
      {
          cout << "Assertion `" << case_source << "` failed.\nAborting\n";
          sig_handler::raise_sig(SIGABRT);
      }
  }

