#include "io.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Chunk size for reading files
#define CHUNK_SIZE 128

/*
Gets the size of the file in bytes.
Sets the file pointer to the beginning of the file.
Returns the length of the file.
*/
unsigned long file_size(const char *file_name) {
    // opening the file in read mode
    unsigned long len;
    FILE *fp = fopen(file_name, "r");

    // checking if the file exist or not
    if (fp == NULL)
        return -1;

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file
    len = ftell(fp);

    fclose(fp);
    return len;
}

/*
Reads a file in text mode and return its contents as (char *).
Reads in chunks. CHUNK_SIZE is the size of each chunk.
*/
char *read_file(const char *filename) {
    FILE *fp;
    char *content;
    unsigned long flen;
    // unsigned long bytes_read = 0;
    // unsigned short reading_amount;
    char chunk[CHUNK_SIZE];
    chunk[CHUNK_SIZE - 1] = '\0';

    // open file for reading
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file \"%s\". It may does not exist.\n", filename);
        exit(1);
    }

    // get file length and allocate memory for content buffer
    flen = file_size(filename);
    content = malloc(flen + 1);
    if (!content) {
        printf("Error allocating memory\n");
        fclose(fp);
        return NULL;
    }
    content[0] = '\0';

    // read file into the buffer
    while (fgets(chunk, CHUNK_SIZE - 1, fp) != NULL) {
        strcat(content, chunk);
    }

    content[flen] = '\0';

    fclose(fp);
    return content;
}

char *get_file_extension(char *filename) {
    char *p = filename + strlen(filename) - 1;
    while (*--p != '.')
        if (p == filename) // if file name does not contain extension
            return "";
    return strdup(p + 1);
}

int alsprintf(char **buf, const char *format, ...) {
    va_list args;
    int printed_chars;
    va_start(args, format);

    // Get the size of the buffer needed to hold the formatted string
    printed_chars = vsnprintf(NULL, 0, format, args);
    if (printed_chars < 0) {
        // An error occurred
        va_end(args);
        return printed_chars;
    }
    // Allocate the buffer
    *buf = malloc(printed_chars + 1);
    // Format the string
    printed_chars = vsnprintf(*buf, printed_chars + 1, format, args);
    va_end(args);
    return printed_chars;
}
