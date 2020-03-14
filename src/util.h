#ifndef UTIL_GUARD__H
#define UTIL_GUARD__H

#include <stdio.h>

// e.g. int num_lines; char** lines = read_lines(handle, &num_lines);
char** readlines(FILE* handle, int* length);

#endif
