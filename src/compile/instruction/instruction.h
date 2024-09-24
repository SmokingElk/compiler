#pragma once
#include <inttypes.h>
#include <stdio.h>

typedef uint32_t instruction;
typedef uint16_t word;

typedef enum {
    OP_MOVE = 0,
    OP_WRITE = 1,
    OP_SAVER = 2,
    OP_SAVEC = 3,
    OP_SVPR = 4,
    OP_LOAD = 5,
    OP_LOADP = 6,
    OP_ALUR = 7,
    OP_ALUC = 8,
    OP_CMPR = 9,
    OP_CMPC = 10,
    OP_JUMP = 11,
    OP_JUMPP = 12,
    OP_BCON = 13,
    OP_CALL = 14,
    OP_RET = 15
} OP_CODE;

typedef enum {
    ALU_AND = 0,
    ALU_OR = 1,
    ALU_ADD = 2,
    ALU_SUB = 3,
    ALU_SRA = 4,
    ALU_SRL = 5,
    ALU_XOR = 6,
    ALU_NOT = 7
} ALU_MODE;

typedef enum {
    CMP_NE = 0,
    CMP_LT = 1,
    CMP_LE = 2,
    CMP_MT = 3,
    CMP_ME = 4,
    CMP_OV = 5,
    CMP_NOV = 6,
    CMP_EQ = 7
} CMP_MODE;

typedef enum {
    REG_RS = 0,

    REG_AX = 1,
    REG_BX = 2,
    REG_CX = 3,
    REG_DX = 4,
    REG_EX = 5,
    REG_FX = 6,
    REG_GX = 7,
    REG_HX = 8,
    REG_IX = 9,
    REG_JX = 10,

    REG_BP = 11,
    REG_RA = 12,
    
    REG_AO = 13,
    REG_BO = 14,
    REG_DI = 15
} REGISTER;

#define REG_COMMON_START REG_AX
#define REG_COMMON_END REG_JX

typedef enum {
    MEM_WORD_16 = 0,
    MEM_WORD_8 = 1
} MEM_MODE;

word shiftInstrAddr (word addr, int shiftSize);

OP_CODE getOpCode (instruction instr);
ALU_MODE getAluMode (instruction instr);
CMP_MODE getCmpMode (instruction instr);
MEM_MODE getMemMode (instruction instr);
REGISTER getReg1 (instruction instr);
REGISTER getReg2 (instruction instr);
word getImmediate (instruction instr);
instruction reverseBytesInstruction (instruction instr);
void printInstruction (instruction instr);

instruction buildInstr (OP_CODE opCode, ALU_MODE aluMode, MEM_MODE memMode, REGISTER reg1, REGISTER reg2, word immValue);

instruction i_MOVE (REGISTER dst, REGISTER src);
instruction i_WRITE (REGISTER dst, word immediate);
instruction i_SAVER (word addr, MEM_MODE memMode, REGISTER src);
instruction i_SAVEC (word addr, MEM_MODE memMode, word immediate);
instruction i_SVPR (REGISTER addrReg, MEM_MODE memMode, REGISTER src);
instruction i_LOAD (REGISTER dst, word addr, MEM_MODE memMode);
instruction i_LOADP (REGISTER dst, REGISTER addrReg, MEM_MODE memMode);
instruction i_ALUR (ALU_MODE mode, REGISTER operandA, REGISTER operandB);
instruction i_ALUC (ALU_MODE mode, REGISTER operandA, word operandB);
instruction i_CMPR (REGISTER operandA, REGISTER operandB);
instruction i_CMPC (REGISTER operandA, word operandB);
instruction i_JUMP (word addr);
instruction i_JUMPP (REGISTER addrReg);
instruction i_BCON (CMP_MODE mode, word addr);
instruction i_CALL (word addr);
instruction i_RET ();