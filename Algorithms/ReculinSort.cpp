#include "Algos.hpp"
#include <memory>


void ReculinSort::sort(List &l)
{
    if(l.size()==0) return;

    for(int i=0; i<l.size();i++){
        std::shared_ptr<List> urlaub = create_list("Urlaub",
                {.background = UI::grey,
                .foreground = WindowStyle::const_color(UI::red)}),
            output = create_list("Ouput",
                 {.background = UI::grey,
                 .foreground = WindowStyle::const_color(UI::green)});

        auto it = l.begin();
        auto max = it;
        while(it<l.end()-i) {
            if(*it>=*max){
                output->push(*it);
                max = it;
                it++;
            } else {
                urlaub->push(*it);
                l.remove(it);
            }
        }
        while(it<l.end()) output->push(*it);

        l.clear();
        for(auto &e : *urlaub) l.push(e);
        for(auto &e : *output) l.push(e);
        if(urlaub->size()==0) break;
    }
}
