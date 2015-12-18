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
 #ifndef SCORPION_SYSTEM_TIME
 #define SCORPION_SYSTEM_TIME
 
 #include <string>
 #include <sstream>
 #include <sys/time.h>

 using namespace std;
 
 namespace scorpionvm
 {
     namespace system_time
     {
         // Begin Dalvik code
        /*
         * Get the wall-clock date/time, in usec.
         */
         long getWallTimeInUsec()
         {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return  tv.tv_usec / 1000LL;
         }
         // END Delvik code

         const string currtime(){
            time_t     now = time(0);
            struct tm  tstruct;
            char       buf[90];
            tstruct = *localtime(&now);
            // Time used for logging
            strftime(buf, sizeof(buf), "%Y-%m-%d--%X", &tstruct);
            stringstream ss;
            ss << buf << "." << getWallTimeInUsec();
            return ss.str();
         }
     } // end time
 } // end ScorpionVM
 
 #endif // SCORPION_SYSTEM_TIME