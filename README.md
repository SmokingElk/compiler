# Compiler

## Usage
build compiler:
```
make build
```

compile test program:
```
make compile
```

run test program:
```
make emulate
```

## Example
program.txt:
```
function int16 main () {
    int16 a = 1;
    int16 b = 2;

    int16 res = a + b;

    @res;
    return;
}
```

Compilation:
```
compiler.exe ./test/test_program.txt ./test/test_program.bin
./test/emulator.exe ./test/test_program.bin
```

Execution trace:
```
IP: 0
RS = 0  AX = 0  BX = 0  CX = 0
DX = 0  EX = 0  FX = 0  GX = 0
HX = 0  IX = 0  JX = 0  BP = 0
RA = 0  AO = 0  BO = 0  DI = 0

IP: 4
RS = 0  AX = 0  BX = 0  CX = 0
DX = 0  EX = 0  FX = 0  GX = 0
HX = 0  IX = 0  JX = 0  BP = 32767
RA = 0  AO = 0  BO = 0  DI = 0
...
...
...

IP: 20
RS = 32768      AX = 3  BX = 3  CX = 2
DX = 0  EX = 0  FX = 0  GX = 0
HX = 0  IX = 0  JX = 0  BP = 32767
RA = 16 AO = 3  BO = 0  DI = 0
```

AO is temporary used as program output.