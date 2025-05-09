#include "Algos.hpp"

auto SelectionSort::sort(List &l) -> void
{
    for(size_t i=1; i<l.size(); i++){
        auto end = l.end()-i;

        auto it = l.begin();
        auto max = it;
        while(++it<=end)
            if(*it>*max) max = it;

        if(max!=end)
            l.swap(max, end);
    }
}
