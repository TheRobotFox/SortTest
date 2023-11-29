#pragma once
#include "List/List.h"

typedef struct{
    char** data;
    int size;
} CSV;

CSV CSV_load_file(char* path);

CSV CSV_load_string(char *str,int size);

CSV CSV_load_string_del(char *str,int size,char sep);

void CSV_print(CSV CSV);

void CSV_free(CSV CSV);
