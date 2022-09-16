#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "csv.h"

typedef struct {
    unsigned int *data;
    int size;
    int filled;
} LIST;

typedef struct{
    char* name;
    int (*func)(int, LIST*);
    char* description;
    int n;
} FillCmd;

LIST CreateList(int size);

void printList(int line_length, LIST *List);

int fill_random(int n, LIST *List);

int random_fill(int n, LIST *List);

int fill_reversed(int n, LIST *List);

int fill_to_n(int n, LIST *List);

LIST List_from_csv(CSV CSV);

int List_to_File(FILE* path, LIST* List);

void free_List(LIST* List);

unsigned int get_max(LIST* List);

int to_num(char* str, int* num);

void List_append(LIST *List, unsigned int e);

void List_concat(LIST *a, LIST *b);
