#include "assemble.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <getopt.h> 
#include <unistd.h>

#define HELP                                                  \
    "Usage: hasm <file> [options]\n"                          \
    "Options:\n"                                              \
    "  -o <output file>\n"                                    \
    "    File to write the resultant Hack machine code to.\n" \
    "    default: 'out.hack'\n"                               \
    "  -w\n"                                                  \
    "    If the output file already exists, overwrite it.\n"  \
    "    default: off\n"


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Use `hasm -h` for help.\n");
        exit(1);
    }
    char* input_name = argv[1];
    FILE* input = fopen(input_name, "r");

    char option;
    int overwrite = 0;
    char* output_name = "out.hack";

    while ((option = getopt(argc, argv, ":ho:w")) != -1) {
        switch (option) {
            case 'h':
                printf(HELP);
                exit(0);
            case 'o':
                output_name = optarg;
                break;
            case 'w':
                overwrite = 1;
                break;
            case ':':
                printf("error: '-%c' flag requires an option.\n", optopt);
                exit(1);
            case '?':
                printf("warn: Unrecognised option '-%c'.\n", optopt);
                continue;
            default:
                break;
        }
    }

    if (! overwrite && access(output_name, F_OK) != -1) {
        printf("'%s' already exists. Overwrite? [y/N] ", output_name);
        char answer;
        if (! scanf("%c", &answer)) return;
        if (answer != 'y' && answer != 'Y') return;
    }

    FILE* output = fopen(output_name, "w");

    int lines;
    if ((lines = assemble(input, output)) != -1) {
        printf("Wrote %i lines to '%s'\n", lines, output_name);
    } else {
        printf("Assembly failed.\n");
    }

    fclose(input);
    fclose(output);

    return 0;
}
