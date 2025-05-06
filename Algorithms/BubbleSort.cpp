#include "Algos.hpp"
#include <iostream>

void BadBubbleSort::sort(List &l) {

    std::cout << "BubbleSort" << std::endl;
    for(size_t i=0; i<l.size(); i++){
        auto a = l.begin();
        while(++a<l.end())
            if(*(a-1)>*a) l.swap(a-1,a);
    }
}
