#include "Algos.hpp"
#include <iostream>

void BadBubbleSort::sort(List &l) {

    for(size_t i=0; i<l.size(); i++){
        auto a = l.begin();
        while(++a<l.end())
            if(*(a-1)>*a) l.swap(a-1,a);
    }
}

void BubbleSort::sort(List &l) {

    for(size_t j=0; j<l.size()-1; j++){
        for(size_t i=j; i<l.size()-1; i++){
            if(l[i]>l[i+1]) l.swap(l+i, l+i+1);
        }
    }
}

// TODO Fix Thread; Raylib Rendering; Wait on compares
