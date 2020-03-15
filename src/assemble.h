#ifndef ASSEMBLE__GUARD_H
#define ASSEMBLE__GUILD_H

#include <stdio.h>

/*
 * Assemble a Hack-asm program. `file` should point to a file containing Hack
 * assembly. The resultant Hack machine code will be written to `output`.
 * The number of lines written to `output` is returned. In the event of
 * a syntax error, the function will write an error message to stdout
 * and call exit.
 */
int assemble(FILE* input, FILE* output);

#endif
