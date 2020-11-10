
#ifndef STREAM_H
#define STREAM_H

#include <stdio.h>

char *figlet(char *string);
void write_center(char *str);

char *read_single_line();
char *read_whole_file(FILE *file, size_t *size_ptr);

#endif