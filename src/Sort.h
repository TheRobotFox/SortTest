#pragma once
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "List.h"

typedef struct {
    char* name;
    void (*func)(List);
    double time_start;
    double time;
    size_t comp;
    size_t swap;
} SortAlg;

#include "GUI.h"

size_t getComp();
size_t getSwap();
void status_reset();

int Verify(List l);

void BubbleSort(List l);

void Bad_BubbleSort(List l);

void SelectionSort(List l);

void StalinSort(List l);

void LSD_Radix(List l);

void InsertionSort(List l);

void CocktailshakerSort(List l);

void BogoSort(List l);

void QuickSort_launcher(List l);

void ReculinSort(List l);
