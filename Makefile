src_path = ./src
object_path = ./object_modules
compiler = gcc

test_input = ./test/test_program.txt
test_output = ./test/test_program.bin

all: build compile

build:
	$(compiler) -c $(src_path)/data_structures/map/map.c -o $(object_path)/map.o
	$(compiler) -c $(src_path)/data_structures/strset/strset.c -o $(object_path)/strset.o
	$(compiler) -c $(src_path)/data_structures/dstring/dstring.c -o $(object_path)/dstring.o
	$(compiler) -c $(src_path)/data_structures/queue/queue.c -o $(object_path)/queue.o
	$(compiler) -c $(src_path)/data_structures/stack/stack.c -o $(object_path)/stack.o
	$(compiler) -c $(src_path)/data_structures/linkedList/linkedList.c -o $(object_path)/linkedList.o

	$(compiler) -c $(src_path)/compile/textPos/textPos.c -o $(object_path)/textPos.o
	$(compiler) -c $(src_path)/compile/compileError/compileError.c -o $(object_path)/compileError.o

	$(compiler) -c $(src_path)/compile/lexeme/lexeme.c -o $(object_path)/lexeme.o
	$(compiler) -c $(src_path)/compile/lexer/lexer.c -o $(object_path)/lexer.o

	$(compiler) -c $(src_path)/compile/opProps/opProps.c -o $(object_path)/opProps.o
	$(compiler) -c $(src_path)/compile/syntaxAnalyzer/syntaxAnalyzer.c -o $(object_path)/syntaxAnalyzer.o

	$(compiler) -c $(src_path)/compile/instruction/instruction.c -o $(object_path)/instruction.o
	$(compiler) -c $(src_path)/compile/type/type.c -o $(object_path)/type.o
	$(compiler) -c $(src_path)/compile/nameTable/nameTable.c -o $(object_path)/nameTable.o

	$(compiler) -c $(src_path)/compile/binCodeGen/utilTypesBinCodeGen/utilTypesBinCodeGen.c -o $(object_path)/utilTypesBinCodeGen.o
	$(compiler) -c $(src_path)/compile/binCodeGen/linkSequence/linkSequence.c -o $(object_path)/linkSequence.o
	$(compiler) -c $(src_path)/compile/binCodeGen/memoryStack/memoryStack.c -o $(object_path)/memoryStack.o
	$(compiler) -c $(src_path)/compile/binCodeGen/binCodeGen.c -o $(object_path)/binCodeGen.o

	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperPlus.c -o $(object_path)/mapperPlus.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperMinus.c -o $(object_path)/mapperMinus.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperAssigment.c -o $(object_path)/mapperAssigment.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperEqual.c -o $(object_path)/mapperEqual.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperNotEqual.c -o $(object_path)/mapperNotEqual.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperLess.c -o $(object_path)/mapperLess.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperLessEqual.c -o $(object_path)/mapperLessEqual.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperMore.c -o $(object_path)/mapperMore.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperMoreEqual.c -o $(object_path)/mapperMoreEqual.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperPrint.c -o $(object_path)/mapperPrint.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperOr.c -o $(object_path)/mapperOr.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperAnd.c -o $(object_path)/mapperAnd.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperNot.c -o $(object_path)/mapperNot.o
	$(compiler) -c $(src_path)/compile/binCodeGen/mappers/mapperBitwiseOr.c -o $(object_path)/mapperBitwiseOr.o

	$(compiler) -c $(src_path)/compile/compile.c -o $(object_path)/compile.o

	$(compiler) -c $(src_path)/main.c -o ${object_path}/main.o
	
	$(compiler) $(object_path)/dstring.o $(object_path)/queue.o $(object_path)/stack.o $(object_path)/linkedList.o $(object_path)/map.o $(object_path)/strset.o $(object_path)/textPos.o $(object_path)/compileError.o $(object_path)/lexeme.o $(object_path)/lexer.o $(object_path)/opProps.o $(object_path)/syntaxAnalyzer.o $(object_path)/instruction.o $(object_path)/type.o $(object_path)/nameTable.o $(object_path)/utilTypesBinCodeGen.o $(object_path)/linkSequence.o $(object_path)/memoryStack.o $(object_path)/binCodeGen.o $(object_path)/mapperPlus.o $(object_path)/mapperMinus.o $(object_path)/mapperAssigment.o $(object_path)/mapperEqual.o $(object_path)/mapperNotEqual.o $(object_path)/mapperLess.o $(object_path)/mapperLessEqual.o $(object_path)/mapperMore.o $(object_path)/mapperMoreEqual.o $(object_path)/mapperPrint.o $(object_path)/mapperOr.o $(object_path)/mapperAnd.o $(object_path)/mapperNot.o $(object_path)/mapperBitwiseOr.o $(object_path)/compile.o $(object_path)/main.o -o ./compiler.exe

clear:
	del "$(CURDIR)/object_modules/*.o"

compile:
	./compiler.exe $(test_input) $(test_output)

emulate:
	./test/emulator.exe $(test_output)

compile_emulate: compile emulate