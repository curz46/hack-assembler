#include "util.h"
#include "assemble.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#define THROW(index, msg) { \
    printf("Parse error on L%i: " msg, index); \
    return -1; }

const int INITIAL_CAPACITY = 10;
const int CAPACITY_STEP    = 10;

typedef uint16_t word_t;

typedef struct symbol_t {
    char* name;
    word_t address;
} symbol_t;

typedef struct context_t {
    symbol_t* symbols;
    int num_symbols;
    int capacity;
    int num_variables;
} context_t;

char* itobin(word_t n) {
    char* str = (char*) malloc((16+1) * sizeof(char));
    for (int i = 15; i >= 0; i--) {
        str[15-i] = ((n >> i) & 0x1) ? '1' : '0';
    }
    str[16] = '\0';
    return str;
}

char* strupper(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
    return str;
}

symbol_t put_symbol(context_t* context, char* name, word_t address) {
    // check capacity
    if (context->num_symbols >= context->capacity) {
        context->capacity += CAPACITY_STEP;
        context->symbols = (char**) realloc(context->symbols, context->capacity);
    }
    // insert
    symbol_t* element = &context->symbols[context->num_symbols++];
    element->name     = name;
    element->address  = address;

    return *element;
}

void put_predefined_symbols() {
    //predefined symbols
    put_symbol(&context, "R0", 0);
    put_symbol(&context, "R1", 1);
    put_symbol(&context, "R2", 2);
    put_symbol(&context, "R3", 3);
    put_symbol(&context, "R4", 4);
    put_symbol(&context, "R5", 5);
    put_symbol(&context, "R6", 6);
    put_symbol(&context, "R7", 7);
    put_symbol(&context, "R8", 8);
    put_symbol(&context, "R9", 9);
    put_symbol(&context, "R10", 10);
    put_symbol(&context, "R11", 11);
    put_symbol(&context, "R12", 12);
    put_symbol(&context, "R13", 13);
    put_symbol(&context, "R14", 14);
    put_symbol(&context, "R15", 15);
    put_symbol(&context, "SCREEN", 16576);
    put_symbol(&context, "KBD", 24576);
    put_symbol(&context, "SP", 0);
    put_symbol(&context, "LCL", 1);
    put_symbol(&context, "ARG", 2);
    put_symbol(&context, "THIS", 3);
    put_symbol(&context, "THAT", 4);
}

symbol_t get_symbol(context_t* context, char* name) {
    for (int i = 0; i < context->num_symbols; i++) {
        symbol_t symbol = context->symbols[i];
        if (strcmp(symbol.name, name) == 0) {
            return symbol;
        }
    }

    // create variable
    word_t address = 16 + context->num_variables++;
    symbol_t symbol = put_symbol(context, name, address);
    return symbol;
}

int assemble(FILE* input, FILE* output) {
    int length;
    char** lines = readlines(input, &length);

    context_t context;
    context.symbols  = (char**) malloc(sizeof(char*) * context.capacity);
    context.num_symbols = 0;
    context.capacity = INITIAL_CAPACITY;
    context.num_variables = 0;

    put_predefined_symbols();

    for (int i = 0; i < length; i++) {
        char* line = lines[i];
        strtok(line, "\n"); // remove newline
        while (line[0] == ' ') { // ignore indentation
            line = line+1;
        }
        if (strlen(line) == 0) THROW(i, "empty line");
        if (line[0] == '@') {
            // A-type instruction
            char* location = strtok(line, "@"); //remove @
            if (strlen(line) == 0) THROW(i, "bad A-type operand");
            if (isdigit(location[0])) {
                //address
                word_t index = atoi(location);
                char* binary = itobin(index);
                fprintf(output, "0%s\n", binary);
            } else {
                //symbol
                strupper(location); // make lowercase
                symbol_t symbol = get_symbol(&context, location);
                char* binary    = itobin(symbol.address);
                fprintf(output, "0%s\n", binary);

                printf("@%s (%i=%s)\n", location, symbol.address, binary);
            }
        } else if (line[0] == '(') {
            // Label
            // strip () from (LABEL)
            char* label = strtok(line, "(");
            strtok(label, ")");
            // make lowercase
            strupper(label);

            word_t address = i + 1;
            put_symbol(&context, label, address);

            printf("New label '%s' @ %i\n", label, address);
        } else {
            // C-type instruction
            bool has_dest = strstr(line, "=") != NULL;
            bool has_jump = strstr(line, ";") != NULL;
            if (has_dest && has_jump) {
                char* dest = strtok(line, "=");
                char* comp = strtok(NULL, ";");
                char* jump = strtok(NULL, "\0");
                printf("%s\n", dest);
                printf("%s\n", comp);
                printf("%s\n", jump);
            } else if (has_dest) {
                char* dest = strtok(line, "=");
                char* comp = strtok(NULL, "\0");
                printf("%s\n", dest);
                printf("%s\n", comp);
            } else {
                char* comp = strtok(line, ";");
                char* jump = strtok(NULL, "\0");
                printf("%s\n", comp);
                printf("%s\n", jump);
            }
        }
    }

    fflush(output);
}
