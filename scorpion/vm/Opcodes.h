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
#ifndef SCORPION_VM_OPCODES
#define SCORPION_VM_OPCODES

// The max numeric value of a VM opcode
#define sMaxOpcodeLimit 65

/*
* vm instruction flags
*/
#define OP_CONST 63
#define OP_ACONST 64
#define OP_CMP 65

  enum Opcodes {
      OP_NOP = 0, 
      OP_PUSH = 1,
      OP_POP = 2,
      OP_JMP = 3,
      OP_MTHD = 4,
      OP_RETURN = 5,
      OP_END = 6,
      OP_CALL = 7,
      OP_ISEQ = 8,
      OP_ISLT = 9,
      OP_ISLE = 10,
      OP_ISGT = 11,
      OP_ISGE = 12,
      OP_ICONST = 13,
      OP_SCONST = 14,
      OP_DCONST = 15,
      OP_FCONST = 16,
      OP_CCONST = 17,
      OP_BCONST = 18,
      OP_STRCONST = 19,
      OP_ADD = 20,
      OP_SUB = 21,
      OP_MULT = 22,
      OP_DIV = 23,
      OP_MOD = 24,
      OP_LSHFT = 25,
      OP_RSHFT = 26,
      OP_CIN = 27,
      OP_COUT = 28,
      OP_HLT = 29,
      OP_JIT = 30,
      OP_JIF = 31,
      OP_LBL = 32,
      OP_NO = 33,
      OP_ENDNO = 34,
      OP_IF = 35,
      OP_OR = 36,
      OP_INC = 37,
      OP_DEC = 38,
      OP_AND = 39,
      OP_THROW = 40,
      OP_AT = 41,
      OP_STR_APND = 42,
      OP_KILL = 43,
      OP_DELETE = 44,
      OP_ALOAD = 46,
      OP_ASTORE = 47,
      OP_ASSN = 48,
      OP_IACONST = 49,
      OP_STR_ACONST = 50,
      OP_CAST = 51,
      OP_BYTE_CONST = 52,
      OP_LCONST = 53,
      OP_SACONST = 54,
      OP_BYTE_ACONST = 55,
      OP_FACONST = 56,
      OP_DACONST = 57,
      OP_CACONST = 58,
      OP_LACONST = 59,
      OP_BACONST = 60,
      OP_NODE = 61,
      OP_NEG = 62
  };
  // TODO: add sizeOf & instanceOf instructions
  // TODO: add popall instruction
#endif 




