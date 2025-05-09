#include "Algos.hpp"
#include <memory>

void merge(List &out, List &a, List &b)
{
    auto ai = a.begin();
    auto bi = b.begin();

    while(ai<a.end() && bi<b.end()){
        if(*ai<*bi) out.push(*ai++);
        else if(*ai>*bi) out.push(*bi++);
    }
    while(ai<a.end()) out.push(*ai++);
    while(bi<b.end()) out.push(*bi++);
}

void GulagSort::sort(List &l)
{
    if(l.size()<=1) return;

    static size_t counter = 0;
    std::shared_ptr<List> urlaub = create_list("Urlaub",
            {.background = UI::grey,
                // .foreground = WindowStyle::const_color(UI::red)
        }),
        output = create_list("Output",
            {.background = UI::grey,
                // .foreground = WindowStyle::const_color(UI::red)
        });

    auto& max = *l.begin();
    for(auto &e : l){
        if(e>=max){
            output->push(e);
            max = e;
        } else urlaub->push(e);
    }
    counter++;

    l.clear();

    if(urlaub->size()>0) sort(*urlaub);
    merge(l, *output, *urlaub);
    std::cout << "Iterationen: " << counter << '\n';
}
