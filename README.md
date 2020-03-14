## Hack Assembler

- [ ] A-type instructions

```c
@32     # load 32 into A-register
@symbol # refers to a fixed point in memory
        # guaranteed to be peristent throughout execution for a particular
        # program
```

- [ ] C-type instructions

Note: `dest` or `jump` can be omitted if empty.

```c
dest=comp;jump
# e.g.
M=D+M
MD=D+M
D;JGT
0;JMP
```

Valid `dest` mnemonics:

```c
null
M
D
MD
A
MA
AD
AMD
```

Valid `jump` mnemonics:
```c
null # nop
JGT  # jump if output > 0
JEQ  # jump if output == 0
JGE  # jump if output >= 0
JLT  # jump if output < 0
JNE  # jump if output != 0
JLE  # jump if output <= 0
JMP  # always jump
```

A `jump` directive will always jump to address `A` in instruction memory.

### Symbols

Hack defines the following constant symbols:

```c
R0     = 0  
R1     = 1
R2     = 2
R3     = 3
...
R15    = 15
SCREEN = 16576 
KBD    = 24576
SP     = 0
LCL    = 1
ARG    = 2
THIS   = 3
THAT   = 4
```

Using `@R0`, for example, is equivalent to `@0`.

Hack also supports symbols as labels:

```c
(LOOP)
  @i
  D=M
  @R0
  ...
  @LOOP // goto LOOP
  0;JMP
```

`@LOOP` is replaced by `@<address>`, where `address` is the memory location
of the next instruction. Note this refers to instruction memory rather than
RAM. A `jump` always refers to instruction memory.
