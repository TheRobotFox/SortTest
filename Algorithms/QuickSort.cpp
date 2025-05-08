#include "Algos.hpp"

void quick_sort(List &l, List::Iterator first, List::Iterator last)
{
    if(last-first <= 1) return;

    auto left = first, right = last;
    ElementCounter pivot = *(first+(last-first)/2);

    while(left<right){
        while(*left<pivot) left++;
        while(*right>pivot) right--;

        if(left<right) l.swap(left, right);
        else break;
    }
    quick_sort(l, first, left);
    quick_sort(l, left+1, last);
}

void QuickSort::sort(List &l) {
    quick_sort(l, l.begin(), l.end()-1);
}
