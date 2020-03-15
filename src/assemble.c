#include "util.h"
#include "debug.h"
#include "assemble.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

int current_line;

#define THROW(msg, ...) { \
    printf("error: Parse error on L%i: " msg "\n", current_line, ## __VA_ARGS__); \
    exit(-1); }

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

char* itobin(word_t n, int len) {
    char* str = (char*) malloc((len+1) * sizeof(char));
    for (int i = len-1; i >= 0; i--) {
        str[(len-1)-i] = ((n >> i) & 0x1) ? '1' : '0';
    }
    str[len] = '\0';
    return str;
}

char* strdup(char* str) {
    char* newstr = (char*) malloc((strlen(str)+1)+1);
    strcpy(newstr, str);
    return newstr;
}

char* strupper(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
    return str;
}

char* strstrip(char* str, char* chars) {
    int len_str   = strlen(str);
    int len_chars = strlen(chars);

    char* newstr = (char*) malloc((len_str+1) * sizeof(char));
    int k = 0;
    for (int i = 0; i < len_str; i++) {
        int skip = 0;
        for (int j = 0; j < len_chars; j++) {
            if (str[i] == chars[j]) skip = 1; // don't add this char
        }
        if (skip) continue;
        newstr[k++] = str[i];
    }
    newstr[k] = '\0';
    // realloc to used size
    return realloc(newstr, (k+1) * sizeof(char)); 
}

int strcontains(char* str, char* chars) {
    chars = strdup(chars);
    int len_chars = strlen(chars);

    for (int i = 0; i < strlen(str); i++) {
        for (int j = 0; j < len_chars; j++) {
            if (str[i] != chars[j]) continue;
            //mark char as contained
            chars[j] = 0;
            break;
        }
    }

    for (int j = 0; j < len_chars; j++) {
        if (chars[j] != 0) return 0;
    }
    return 1;
}

char* encode_dest(char* dest) {
    // this could be impl. much better, but it's unlikely that
    // assembler time is long enough for it to matter
    strupper(dest);
    if (strcmp(dest, "NULL") == 0) return "000";
    if (strcontains(dest, "AMD"))  return "111";
    if (strcontains(dest, "AD"))   return "110";
    if (strcontains(dest, "MA"))   return "101";
    if (strcontains(dest, "MD"))   return "011";
    if (strcontains(dest, "A"))    return "100";
    if (strcontains(dest, "D"))    return "010";
    if (strcontains(dest, "M"))    return "001";
    THROW("Unrecognised destination '%s'", dest);
}

char* encode_comp(char* comp) {
    int length = strlen(comp);
    if (length == 0) THROW("Empty computation");
    if (length == 1) {
        char val = comp[0];
        switch (val) {
            case '0': return "0101010";
            case '1': return "0111111";
            case 'D': return "0001100";
            case 'A': return "0110000";
            case 'M': return "1110000";
            default:  THROW("Bad computation value '%c'", val);
        }
    }
    if (length == 2) {
        char op  = comp[0];
        char val = comp[1];
        switch (op) {
            case '-':
                switch (val) {
                    case '1': return "0111010";
                    case 'D': return "0001111";
                    case 'A': return "0110011";
                    case 'M': return "1110011";
                    default:  THROW("Bad computation '%s'", comp);
                }
            case '~':
                switch (val) {
                    case 'D': return "0001101";
                    case 'A': return "0110001";
                    case 'M': return "1110001";
                    default:  THROW("Bad computation '%s'", comp);
                }
            default: THROW("Unknown operation '%c'", op);
        }
    }
    if (length == 3) {
        char a  = comp[0];
        char op = comp[1];
        char b  = comp[2];
        switch (op) {
            case '+':
                if (a == 'D' && b == '1') return "0011111";
                if (a == 'A' && b == '1') return "0110111";
                if (a == 'D' && b == 'A') return "0000010";
                if (a == 'M' && b == '1') return "1110111";
                if (a == 'D' && b == 'M') return "1000010";
                THROW("Bad computation '%s'", comp);
            case '-':
                if (a == 'D' && b == '1') return "0001110";
                if (a == 'A' && b == '1') return "0110010";
                if (a == 'D' && b == 'A') return "0100011";
                if (a == 'A' && b == 'D') return "0000111";
                if (a == 'M' && b == '1') return "1110010";
                if (a == 'D' && b == 'M') return "1010011";
                if (a == 'M' && b == 'D') return "1000111";
                THROW("Bad computation '%s'", comp);
            case 'V':
                if (a == 'D' && b == 'A') return "0010101";
                if (a == 'A' && b == 'D') return "0010101";
                if (a == 'D' && b == 'M') return "1010101";
                if (a == 'M' && b == 'D') return "1010101";
                THROW("Bad computation '%s'", comp);
            case '^':
                if (a == 'D' && b == 'A') return "0000000";
                if (a == 'A' && b == 'D') return "0000000";
                if (a == 'D' && b == 'M') return "1000000";
                if (a == 'M' && b == 'D') return "1000000";
                THROW("Bad computation '%s'", comp);
            default: THROW("Unknown operation '%c'", op);
        }
    }
    THROW("Bad computation '%s'", comp);
}

char* encode_jump(char* jump) {
    strupper(jump);
    if (strcmp(jump, "NULL") == 0) return "000";
    if (strcmp(jump, "JGT") == 0)  return "001";
    if (strcmp(jump, "JEQ") == 0)  return "010";
    if (strcmp(jump, "JGE") == 0)  return "011";
    if (strcmp(jump, "JLT") == 0)  return "100";
    if (strcmp(jump, "JNE") == 0)  return "101";
    if (strcmp(jump, "JLE") == 0)  return "110";
    if (strcmp(jump, "JMP") == 0)  return "111";
    THROW("Unrecognised jump '%s'", jump);
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

void put_predefined_symbols(context_t* context) {
    //predefined symbols
    put_symbol(context, "R0", 0);
    put_symbol(context, "R1", 1);
    put_symbol(context, "R2", 2);
    put_symbol(context, "R3", 3);
    put_symbol(context, "R4", 4);
    put_symbol(context, "R5", 5);
    put_symbol(context, "R6", 6);
    put_symbol(context, "R7", 7);
    put_symbol(context, "R8", 8);
    put_symbol(context, "R9", 9);
    put_symbol(context, "R10", 10);
    put_symbol(context, "R11", 11);
    put_symbol(context, "R12", 12);
    put_symbol(context, "R13", 13);
    put_symbol(context, "R14", 14);
    put_symbol(context, "R15", 15);
    put_symbol(context, "SCREEN", 16576);
    put_symbol(context, "KBD", 24576);
    put_symbol(context, "SP", 0);
    put_symbol(context, "LCL", 1);
    put_symbol(context, "ARG", 2);
    put_symbol(context, "THIS", 3);
    put_symbol(context, "THAT", 4);
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

    put_predefined_symbols(&context);

    int written_lines = 0;

    for (int i = 0; i < length; i++) {
        current_line = i + 1;
        
        char* line = lines[i];
        // remove whitespace
        char* inst = strstrip(line, "\n\t\r ");
        if (strlen(inst) == 0) THROW("Empty instruction");
        if (inst[0] == '@') {
            // A-type instruction
            char* location = strtok(inst, "@"); //remove @
            if (strlen(inst) == 0) THROW("Empty A-type instruction");
            if (isdigit(location[0])) {
                //address
                word_t index = atoi(location);
                char* binary = itobin(index, 15);
                fprintf(output, "0%s\n", binary);
                written_lines++;

                DEBUG(2, "@%i (%s)\n", index, binary);
            } else {
                //symbol
                strupper(location); // make lowercase
                symbol_t symbol = get_symbol(&context, location);
                char* binary    = itobin(symbol.address, 15);
                fprintf(output, "0%s\n", binary);
                written_lines++;

                DEBUG(2, "@%s (%i=%s)\n", location, symbol.address, binary);
            }
        } else if (inst[0] == '(') {
            // Label
            // strip () from (LABEL)
            char* label = strtok(inst, "(");
            strtok(label, ")");
            // make lowercase
            strupper(label);

            word_t address = i + 1;
            put_symbol(&context, label, address);

            DEBUG(2, "New label '%s' @ %i\n", label, address);
        } else {
            // C-type instruction
            bool has_dest = strstr(inst, "=") != NULL;
            bool has_jump = strstr(inst, ";") != NULL;

            char* dest_bits;
            char* comp_bits;
            char* jump_bits;
            if (has_dest && has_jump) {
                char* dest = strtok(inst, "=");
                char* comp = strtok(NULL, ";");
                char* jump = strtok(NULL, "\0");

                dest_bits = encode_dest(dest);
                comp_bits = encode_comp(comp);
                jump_bits = encode_jump(jump);
            } else if (has_dest) {
                char* dest = strtok(inst, "=");
                char* comp = strtok(NULL, "\0");

                dest_bits = encode_dest(dest);
                comp_bits = encode_comp(comp);
                jump_bits = "000"; // default
            } else {
                char* comp = strtok(inst, ";");
                char* jump = strtok(NULL, "\0");

                dest_bits = "000";
                comp_bits = encode_comp(comp);
                jump_bits = encode_jump(jump);
            }

            fprintf(output, "111%s%s%s\n", comp_bits, dest_bits, jump_bits);
            written_lines++;

            DEBUG(2, "wrote C-type: 111%s%s%s\n", comp_bits, dest_bits, jump_bits);
        }
    }

    fflush(output);

    return written_lines;
}
