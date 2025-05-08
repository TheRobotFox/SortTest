#include "Algos.hpp"

void StalinSort::sort(List &l) {

    size_t i = 1;
    while(i<l.size()){
        if(l[i-1]>l[i]) l.remove(l+i);
        else i++;
    }
}
