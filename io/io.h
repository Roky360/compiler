#ifndef INFINITY_COMPILER_IO_H
#define INFINITY_COMPILER_IO_H

#include <stdio.h>

unsigned long file_length(FILE *fp);

char *read_file(const char *filename);

char *get_file_extension(char *filename);

#endif //INFINITY_COMPILER_IO_H
