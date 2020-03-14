#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// e.g. char** lines; int num_lines = read_lines(handle, &lines);
char** readlines(FILE* handle, int* length) {
    const int INITIAL_CAPACITY = 10;
    const int CAPACITY_STEP    = 10;
    int capacity = INITIAL_CAPACITY;
    *length      = 0;
    char** array = (char**) malloc(sizeof(char*) * capacity);

    //https://linux.die.net/man/3/getline
    char* line = NULL;
    size_t len = 0;
    size_t read;
    while ((read = getline(&line, &len, handle)) != -1) {
        (*length)++;
        if (*length > capacity) {
            capacity += CAPACITY_STEP;
            array = realloc(array, sizeof(char*) * capacity);
        }
        char* copy = (char*) malloc(sizeof(char) * strlen(line) + 1);
        strcpy(copy, line);
        array[*length - 1] = copy;
    }
    free(line);
    return array;
}
