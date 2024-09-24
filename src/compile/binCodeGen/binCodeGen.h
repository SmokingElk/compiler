#pragma once
#include "../../data_structures/queue/queue.h"
#include "../../data_structures/stack/stack.h"
#include "../../data_structures/linkedList/linkedList.h"
#include "../../data_structures/map/map.h"
#include "../../data_structures/strset/strset.h"
#include "../instruction/instruction.h"
#include "../type/type.h"
#include "../nameTable/nameTable.h"
#include "../../config.h"
#include "../lexeme/lexeme.h"
#include "../syntaxAnalyzer/syntaxAnalyzer.h"
#include "./utilTypesBinCodeGen/utilTypesBinCodeGen.h"
#include "./linkSequence/linkSequence.h"
#include "./memoryStack/memoryStack.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Stack freeRegs;
    CompileError error;
    FILE* out;
    FILE* prepatchedFile;
    Map opMappers;
    word instrWritten;
    LinkSequence linkSequence;
    Map funcsAddrs;
} _BinCodeGen, *BinCodeGen;

BinCodeGen createBinCodeGen ();
void deleteBinCodeGen (BinCodeGen gen);
BinCodeGen setOut (BinCodeGen gen, FILE *file);
void resetBinCodeGen (BinCodeGen gen);
void generatePrefixCode (BinCodeGen gen);
CompileError genErrorBinCodeGen (BinCodeGen gen);

// генерирует промежуточный машинный код по выражению в постфиксной форме
REGISTER postfixToBinCode (BinCodeGen gen, Queue in, PriorityNameTable nameTable, Map typeTable, SyntaxAnalyzer analyzer, bool *res);

// создать код для ветки (ссылка не заполнена)
void addBranchBinCodeGen (BinCodeGen gen, REGISTER conditionValueReg);

// указать текущий адрес как конец ветки
void closeBranch (BinCodeGen gen);

void functionHereBinCodeGen (BinCodeGen gen, char name[]);
void generateReturnBinCodeGen (BinCodeGen gen, Map typeTable, string funcName, REGISTER resReg);

void addLoopLinkBinCodeGen (BinCodeGen gen);
void addLoopConditionBinCodeGen (BinCodeGen gen, REGISTER conditionValueReg);
void closeLoop (BinCodeGen gen);

// заполняет пропущенные ссылки 
void backpatchLinks (BinCodeGen gen);

LOCATION allocReg (BinCodeGen gen);
void freeReg (BinCodeGen gen, LOCATION loc);
void freeOperandReg (BinCodeGen gen, Operand operand);
void writeInstruction (BinCodeGen gen, instruction instr);