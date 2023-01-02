#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config/globals.h"
#include "compiler/compiler.h"
#include "io/io.h"

// TODO: add EOF proof to parser

int main(int argc, char **argv)
{
    // check that target file is specified
    if (argc < 2)
    {
        printf("Please provide target file path as a command line argument.\n");
        exit(0);
    }
    // check file extension
    if (strcmp(get_file_extension(argv[1]), EXTENSION) != 0)
    {
        printf("File extension not supported. Must be *.%s files only.\n", EXTENSION);
        exit(0);
    }

    compiler_compile_file(argv[1]);
    printf("\nDone\n");

    return 0;
}
