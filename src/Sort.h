#pragma once
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "List.h"

typedef struct {
    char* name;
    void (*func)(LIST*);
    double time_start;
    double time;
    size_t comp;
    size_t swap;
} SortAlg;

#include "GUI.h"

size_t getComp();
size_t getSwap();
void reset();

int Verify(LIST* List);

void BubbleSort(LIST *List);

void Bad_BubbleSort(LIST *List);

void SelectionSort(LIST *List);

void StalinSort(LIST *List);

void LSD_Radix(LIST *List);

void InsertionSort(LIST *List);

void CocktailshakerSort(LIST *List);

void BogoSort(LIST *List);

void QuickSort_launcher(LIST *List);
