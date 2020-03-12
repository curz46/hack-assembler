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
