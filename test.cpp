
#include "argparse.hpp"
#include "Sort.hpp"
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <raylib.h>
#include <thread>
#include <type_traits>
#include "SortList.hpp"

struct Builder {
    ListBuilder *builder;
    int n;
};
template<typename T> requires
    requires(T& e){
        e->name;
        e->description;
        std::remove_pointer_t<T>::all;
    }
struct ArgParse<T>
{
    using C = std::remove_pointer_t<T>;
    auto operator()(const char *text, C*& val) -> bool
    {
        std::string name(text);

        for(C *e : C::all){
            if(e->name==name){

                val = e;
                return true;
            }
        }
        std::cout << "Unknown Name: " << text << "\n";
        std::cout << "Available:" << '\n';
        for(C *e : C::all)
            std::cout << e->name << " - " << e->description << '\n';

        return false;
    }
};
template<>
struct ArgParse<Builder>
{
    auto operator()(const char *text, Builder& val) -> bool
    {
        std::string str(text);
        if(!ArgParse<ListBuilder*>()
           (str.substr(0,str.find(':')).c_str(), val.builder)) return false;
        if(!ArgParse<int>()(text+str.find(':')+1, val.n)) return false;
        return true;
    }
};


auto main(int argc, const char **argv) -> int
{
    std::vector<Builder> builders;
    std::vector<SortAlgorithm*> sorts = SortAlgorithm::all;
    std::vector<T> list;

    ArgParser ap(argc, argv,
                 Arg(builders, 'c', "create List using Builders", true),
                 Arg(sorts, 'a', "List of Algorithms to run"));


    if(!ap.parse_args()){
        std::cout << "Failed paring Args!" << '\n';
        return -1;
    }

    for(auto b : builders) b.builder->run(list, b.n);

    UI::GUI::instance->start({.ops_per_second = 40});

    std::cout << list << '\n';


    for(auto *s : sorts){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        UI::GUI::instance->set_algorithm(s);
        s->run(list);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }


    UI::GUI::instance->stop();
}
