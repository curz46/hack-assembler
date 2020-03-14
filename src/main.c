#include "assemble.h"

#include <stdlib.h>
#include <stdint.h>


int main(int argc, char **argv) {
    if (argc < 3) {
        printf("hasm <input> <output>\n");
        exit(1);
    }
    char* input_name = argv[1];
    FILE* input = fopen(input_name, "r");

    char* output_name = argv[2];
    FILE* output = fopen(output_name, "w");

    assemble(input, output);

    fclose(input);
    fclose(output);

    return 0;
}
