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
 #include "xso_reader.h"
 
 using namespace std;
 
 
    int scorpionvm::xsoreader::flag_minor_version     = 0x0005;
    int scorpionvm::xsoreader::flag_major_version     = 0x0006;
    int scorpionvm::xsoreader::flag_file_size          = 0x0007;
    int scorpionvm::xsoreader::flag_target_dev_vers    = 0x0008;
    int scorpionvm::xsoreader::flag_minimum_dev_vers   = 0x0009;
    int scorpionvm::xsoreader::flag_version_number     = 0x0010;
    int scorpionvm::xsoreader::flag_debugging          = 0x0011;
    int scorpionvm::xsoreader::flag_logging            = 0x0012;
    int scorpionvm::xsoreader::flag_log_precedence     = 0x0013;
    int scorpionvm::xsoreader::flag_log_file           = 0x0014;
    int scorpionvm::xsoreader::flag_application_id     = 0x0015; 
    int scorpionvm::xsoreader::flag_permissions        = 0x0016;
    int scorpionvm::xsoreader::flag_nameflag           = 0x0017;
    int scorpionvm::xsoreader::flag_methodsize         = 0x0018;
    