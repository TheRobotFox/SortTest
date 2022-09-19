#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "csv.h"
#include "SortTest.h"
#include "List/List.h"

typedef struct{
    char* name;
    void (*func)(List, int);
    char* description;
    int n;
} FillCmd;

void List_fill_random(List l, int n);

void List_random_fill(List l, int n);

void List_fill_reversed(List l, int n);

void List_fill_to_n(List l, int n);

void List_save(List l, FILE *f);

unsigned int get_max(List l);

int List_verify(List l);

int to_num(char* str, int* num);

int List_append_csv(List l, CSV  csv);

