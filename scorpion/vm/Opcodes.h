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
#define sMaxOpcodeLimit 0x50

  enum Opcodes {
      OP_NOP = 0x0, 
      OP_PUSH = 0x2,
      OP_POP = 0x3,
      OP_JMP = 0x4,
      OP_MTHD = 0x5,
      OP_RETURN = 0x6,
      OP_END = 0x7,
      OP_CALL = 0X8,
      OP_ISEQ = 0x9,
      OP_ISNEQ = 0xa,
      OP_ISLT = 0xb,
      OP_ISNLT = 0xc,
      OP_ISLE = 0xd,
      OP_ISNLE = 0xe,
      OP_ISGT = 0xf,
      OP_ISNGT = 0x16,
      OP_ISGE = 0x17,
      OP_ISNGE = 0x18,
      OP_ICONST = 0x19,
      OP_SCONST = 0x1a,
      OP_DCONST = 0x1b,
      OP_FCONST = 0x1c,
      OP_CCONST = 0x1d,
      OP_BCONST = 0x1e,
      OP_STRCONST = 0x1f,
      OP_IADD = 0x20,
      OP_SADD = 0x21,
      OP_DADD = 0x22,
      OP_FADD = 0x23,
      OP_CADD = 0x24,
      OP_ISUB = 0x25,
      OP_SSUB = 0x26,
      OP_DSUB = 0x27,
      OP_FSUB = 0x28,
      OP_CSUB = 0x29,
      OP_IMULT = 0x2a,
      OP_SMULT = 0x2b,
      OP_DMULT = 0x2c,
      OP_FMULT = 0x2d,
      OP_CMULT = 0x2e,
      OP_IDIV = 0x2f,
      OP_SDIV = 0x30,
      OP_DDIV = 0x31,
      OP_FDIV = 0x32,
      OP_CDIV = 0x33,
      OP_LSHFT = 0x34,
      OP_RSHFT = 0x35,
      OP_CIN = 0x36,
      OP_COUT = 0x37,
      OP_HLT = 0x38,
      OP_JIT = 0x39,
      OP_JIF = 0x3a,
      OP_LBL = 0x3b,
      OP_NO = 0x3c,
      OP_ENDNO = 0x3d,
      OP_IF = 0x3e,
      OP_IMOD = 0x3f,
      OP_SMOD = 0x40,
      OP_CMOD = 0x41,
      OP_OR = 0x42,
      OP_INC = 0x43,
      OP_DEC = 0x44,
      OP_AND = 0x45,
      OP_SLP = 0x46,
      OP_USLP = 0x47,
      OP_THROW = 0x48,
      OP_AT = 0x49,
      OP_STR_APND = 0x4a,
      OP_KILL = 0x4b,
      OP_DELETE = 0x4c,
      OP_DELETE_ARRY = 0x4d,
      OP_ALOAD = 0x4e,
      OP_ASTORE = 0x4f,
      OP_ASSN = 0x50,
  };

#endif 




