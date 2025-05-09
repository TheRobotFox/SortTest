#include "Algos.hpp"

void quick_sort(List &l, List::Iterator first, List::Iterator last)
{
    if(last-first <= 1) return;

    auto pivot = *first;
    auto left = first, right = last;

    while(true){
        while(*left<pivot) left++;
        while(*right>pivot) right--;
        if(left>=right) break;
        l.swap(left, right);
    }
    quick_sort(l, first, right);
    quick_sort(l, right+1, last);
}

void QuickSort::sort(List &l) {
    quick_sort(l, l.begin(), l.end()-1);
}
