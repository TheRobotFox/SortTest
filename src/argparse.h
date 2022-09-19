#pragma once
#include "Sort.h"
#include "List.h"
#include "csv.h"
#include "GUI.h"
#include <stdio.h>

typedef struct{
    char id;
    short type;
    char* describtion;
    union {
    // Supported Values
        int val;
        float f_val;
        char* str;
        unsigned char boolean;
        void* data;

        CSV csv;
        SortAlg* sorting_algs;
        FILE* File;
        List l;
    };
    // unused
    int length;
    char Error[128];

} arg;

int argparse(int argc, char** argv);
void print_usage();

arg* arg_get(char id);
