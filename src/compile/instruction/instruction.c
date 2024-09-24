#include "instruction.h"

instruction buildInstr (OP_CODE opCode, ALU_MODE aluMode, MEM_MODE memMode, REGISTER reg1, REGISTER reg2, word immValue) {
    instruction instr = 0;
    instr += opCode;
    instr = (instr << 3) + aluMode;
    instr = (instr << 1) + memMode;
    instr = (instr << 4) + reg1;
    instr = (instr << 4) + reg2;
    instr = (instr << 16) + immValue;
    return instr;
}

OP_CODE getOpCode (instruction instr) {
    return instr >> 28;
}

ALU_MODE getAluMode (instruction instr) {
    return (instr >> 25) % 8;
}

CMP_MODE getCmpMode (instruction instr) {
    return (instr >> 25) % 8;
}

MEM_MODE getMemMode (instruction instr) {
    return (instr >> 24) % 2;
}

REGISTER getReg1 (instruction instr) {
    return (instr >> 20) % 16;
}

REGISTER getReg2 (instruction instr) {
    return (instr >> 16) % 16;
}

word getImmediate (instruction instr) {
    return instr;
}

void printInstruction (instruction instr) {
    char opNames[16][8] = {
        "MOVE",
        "WRITE",
        "SAVER",
        "SAVEC",
        "SVPR",
        "LOAD",
        "LOADP",
        "ALUR",
        "ALUC",
        "CMPR",
        "CMPC",
        "JUMP",
        "JUMPP",
        "BCON",
        "CALL",
        "RET"
    };

    char regNames[16][3] = {
        "RS",
        "AX",
        "BX",
        "CX",
        "DX",
        "EX",
        "FX",
        "GX",
        "HX",
        "IX",
        "JX",
        "BP",
        "RA",
        "AO",
        "BO",
        "DI"
    };

    char aluNames[8][4] = {
        "AND",
        "OR",
        "ADD",
        "SUB",
        "SRA",
        "SRL",
        "XOR",
        "NOT"
    };

    printf("%s\t", opNames[(int)getOpCode(instr)]);
    printf("%s\t", aluNames[(int)getAluMode(instr)]);
    printf("%s\t", getMemMode(instr) == MEM_WORD_16 ? "M16" : "M8");
    printf("%s\t", regNames[(int)getReg1(instr) % 16]);
    printf("%s\t", regNames[(int)getReg2(instr) % 16]);
    printf("%d", getImmediate(instr));

    if (getImmediate(instr) > 32767) {
        printf(" (-%d)", 65536 - getImmediate(instr));
    }
}

word shiftInstrAddr (word addr, int shiftSize) {
    return addr + sizeof(instruction) * shiftSize;
}

instruction reverseBytesInstruction (instruction instr) {
    uint8_t b1 = instr & 255;
    uint8_t b2 = (instr >> 8) & 255;
    uint8_t b3 = (instr >> 16) & 255;
    uint8_t b4 = (instr >> 24) & 255;
    return (b1 << 24) + (b2 << 16) + (b3 << 8) + b4;
}

instruction i_MOVE (REGISTER dst, REGISTER src) {
    return buildInstr(OP_MOVE, ALU_AND, MEM_WORD_16, dst, src, 0);
}

instruction i_WRITE (REGISTER dst, word immediate) {
    return buildInstr(OP_WRITE, ALU_AND, MEM_WORD_16, dst, REG_RS, immediate);
}

instruction i_SAVER (word addr, MEM_MODE memMode, REGISTER src) {
    return buildInstr(OP_SAVER, ALU_AND, memMode, src, REG_RS, addr);
}

instruction i_SAVEC (word addr, MEM_MODE memMode, word immediate) {
    // не поддерживается
}

instruction i_SVPR (REGISTER addrReg, MEM_MODE memMode, REGISTER src) {
    return buildInstr(OP_SVPR, ALU_AND, memMode, addrReg, src, 0);
}

instruction i_LOAD (REGISTER dst, word addr, MEM_MODE memMode) {
    return buildInstr(OP_LOAD, ALU_AND, memMode, dst, REG_RS, addr);
}

instruction i_LOADP (REGISTER dst, REGISTER addrReg, MEM_MODE memMode) {
    return buildInstr(OP_LOADP, ALU_AND, memMode, dst, addrReg, 0);
}

instruction i_ALUR (ALU_MODE mode, REGISTER operandA, REGISTER operandB) {
    return buildInstr(OP_ALUR, mode, MEM_WORD_16, operandA, operandB, 0);
}

instruction i_ALUC (ALU_MODE mode, REGISTER operandA, word operandB) {
    return buildInstr(OP_ALUC, mode, MEM_WORD_16, operandA, REG_RS, operandB);
}

instruction i_CMPR (REGISTER operandA, REGISTER operandB) {
    return buildInstr(OP_CMPR, ALU_AND, MEM_WORD_16, operandA, operandB, 0);
}

instruction i_CMPC (REGISTER operandA, word operandB) {
    return buildInstr(OP_CMPC, ALU_AND, MEM_WORD_16, operandA, REG_RS, operandB);
}

instruction i_JUMP (word addr) {
    return buildInstr(OP_JUMP, ALU_AND, MEM_WORD_16, REG_RS, REG_RS, addr);
}

instruction i_JUMPP (REGISTER addrReg) {
    return buildInstr(OP_JUMPP, ALU_AND, MEM_WORD_16, addrReg, REG_RS, 0);
}

instruction i_BCON (CMP_MODE mode, word addr) {
    return buildInstr(OP_BCON, (ALU_MODE)mode, MEM_WORD_16, REG_RS, REG_RS, addr);
}

instruction i_CALL (word addr) {
    // не поддерживается
}

instruction i_RET () {
    // не поддерживается
}
