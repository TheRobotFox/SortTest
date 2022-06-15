#pragma once
#include <stdio.h>
#include <string.h>

#undef malloc
typedef struct{
    char** data;
    int size;
} CSV;

CSV CSV_from_File(char* path);

CSV csv(char *str,int size);

CSV split(char *str,int size,char sep);

void CSV_print(CSV CSV);

void free_CSV(CSV CSV);
