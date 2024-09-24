#include "binCodeGen.h"

#define defMapper(name) Operand (name) (BinCodeGen, List, Map);

// объявление мапперов
defMapper(mapperPlus);
defMapper(mapperMinus);
defMapper(mapperAssigment);
defMapper(mapperPrint);
defMapper(mapperEqual);
defMapper(mapperNotEqual);
defMapper(mapperLess);
defMapper(mapperLessEqual);
defMapper(mapperMore);
defMapper(mapperMoreEqual);
defMapper(mapperOr);
defMapper(mapperAnd);
defMapper(mapperNot);
defMapper(mapperBitwiseOr);