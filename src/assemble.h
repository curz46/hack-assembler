#ifndef ASSEMBLE__GUARD_H
#define ASSEMBLE__GUILD_H

#include <stdio.h>

/*
 * Assemble a Hack-asm program. `file` should point to a file containing Hack
 * assembly. The resultant Hack machine code will be written to `output`.
 * A return value of 0 means that the program was assembled successfully, while
 * anything else implies an error occurred.
 */
int assemble(FILE* input, FILE* output);

#endif
