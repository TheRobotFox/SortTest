#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    char** data;
    int size;
} CSV;

CSV CSV_from_File(char* path);

CSV csv(char *str,int size);

CSV split(char *str,int size,char sep);

void free_CSV(CSV CSV);