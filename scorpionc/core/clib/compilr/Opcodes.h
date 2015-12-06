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
#define sMaxOpcodeLimit 84

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
      OP_ISNEQ = 9,
      OP_ISLT = 10,
      OP_ISNLT = 11,
      OP_ISLE = 12,
      OP_ISNLE = 13,
      OP_ISGT = 14,
      OP_ISNGT = 15,
      OP_ISGE = 16,
      OP_ISNGE = 17,
      OP_ICONST = 18,
      OP_SCONST = 19,
      OP_DCONST = 20,
      OP_FCONST = 21,
      OP_CCONST = 22,
      OP_BCONST = 23,
      OP_STRCONST = 24,
      OP_IADD = 25,
      OP_SADD = 26,
      OP_DADD = 27,
      OP_FADD = 28,
      OP_CADD = 29,
      OP_ISUB = 30,
      OP_SSUB = 30,
      OP_DSUB = 32,
      OP_FSUB = 33,
      OP_CSUB = 34,
      OP_IMULT = 35,
      OP_SMULT = 36,
      OP_DMULT = 37,
      OP_FMULT = 38,
      OP_CMULT = 39,
      OP_IDIV = 40,
      OP_SDIV = 41,
      OP_DDIV = 42,
      OP_FDIV = 43,
      OP_CDIV = 44,
      OP_LSHFT = 45,
      OP_RSHFT = 46,
      OP_CIN = 47,
      OP_COUT = 48,
      OP_HLT = 49,
      OP_JIT = 50,
      OP_JIF = 51,
      OP_LBL = 52,
      OP_NO = 53,
      OP_ENDNO = 54,
      OP_IF = 55,
      OP_IMOD = 56,
      OP_SMOD = 57,
      OP_CMOD = 58,
      OP_OR = 59,
      OP_INC = 60,
      OP_DEC = 61,
      OP_AND = 62,
      // 0x46 - 47 unused
      OP_THROW = 63,
      OP_AT = 64,
      OP_STR_APND = 65,
      OP_KILL = 66,
      OP_DELETE = 67,
      OP_DELETE_ARRY = 68,
      OP_ALOAD = 69,
      OP_ASTORE = 70,
      OP_ASSN = 71,
      OP_IACONST = 72,
      OP_STR_ACONST = 73,
      OP_CAST = 74,
      OP_BYTE_CONST = 75,
      OP_LCONST = 76,
      OP_SACONST = 77,
      OP_BYTE_ACONST = 78,
      OP_FACONST = 79,
      OP_DACONST = 80,
      OP_CACONST = 81,
      OP_LACONST = 82,
      OP_BACONST = 83,
      OP_NODE = 84,
  };
  // TODO: add sizeOf & instanceOf instructions
  // TODO: add popall instruction
#endif 




