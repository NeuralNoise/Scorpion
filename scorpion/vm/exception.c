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
 #include "exception.h"
 #include "scorpionvm.h"
 #include "../logservice/alog.h"
 #include "Globals.h"
 #include <sstream>
 #include <stdio.h>
 #include <stdlib.h>     
 #include <iostream>
 #include <string>
 
 using namespace std;

 extern ALog alog;

 #define EXIT_ERR -109273614

/*
 * Global traceback error handler 
*/
TraceBack Exception::trace;

Exception::Exception(string message, string clause){
     Exception::trace.addproto("vm.internal.err.Exception", "Exception", 1);
     
     gSvm.ethrow = true;
     gSvm.ForceShutdown = true;
     stringstream ss, ss1;
     ss << "FATAL EXCEPTION: main\n" << "ScorpionRuntimeException caused by " << clause << ": " << message << endl << "" << trace.getStackTrace();
     alog.ALOGE(ss.str());
     
     ss1 << "ScorpionRuntimeException caused by " << clause << ": " << message << endl;
     cout << ss1.str();
     trace.printStackTrace();
     
     cout << endl;
     gSvm.exitval = EXIT_ERR;
     
     Init_ShutdownScorpionVM();
}

void segfault(){
    Exception::trace.addproto("vm.internal.err.SegFault", "Exception", 1);
    gSvm.ForceShutdown = true;
    gSvm.ethrow = true; // no need to print program status code
    
    cout << "Segmentation Fault." << endl;
    stringstream ss;
    ss << "FATAL EXCEPTION: main\n" << "ScorpionRuntimeException caused by " << "BadException" << ": " << "A segmentation fault was thrown while running application." << endl << "" << Exception::trace.getStackTrace();
    alog.ALOGE(ss.str());
    
    Init_ShutdownScorpionVM();
}

void Exception::printStackTrace()
{
    trace.printStackTrace();
}
