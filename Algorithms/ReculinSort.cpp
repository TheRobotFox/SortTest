#include "Algos.hpp"
#include <memory>

void ReculinSort::sort(List &l)
{
    if(l.size()==0) return;

    while(true){
        std::shared_ptr<List> urlaub = create_list("Urlaub", {.background = {.r = 128, .g = 128, .b = 128}});
        auto it = l.begin();
        while(it<l.end()-1) {
            if(*it>*(it+1)){
                urlaub->push(*it);
                l.remove(it);
                if(it>l.begin()) it--;
            } else it++;
        }

        sort(*urlaub);
        for(auto &e : *urlaub) l.push(e);
        if(urlaub->size()==0) break;
    }
}
