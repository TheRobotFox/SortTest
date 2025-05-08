#include "Factory.hpp"
#include "argparse.hpp"
#include "Sort.hpp"
#include <algorithm>
#include <ranges>
#include <chrono>
#include <iostream>
#include <iterator>
#include <memory>
#include <raylib.h>
#include <thread>
#include "SortList.hpp"

struct Builder {
    std::unique_ptr<ListBuilder> builder;
    int n;
};
template<class P>
requires has_factory<typename P::element_type>
struct ArgParse<P>
{
    using T = P::element_type;
    constexpr auto operator()(const char *text, P& val) -> bool
    {
        std::string name(text);

        for(const InstanceBuilder<T> &e : T::all){
            if(e.name==name){

                val = std::move(e.get());
                return true;
            }
        }
        std::cout << "Unknown Name: " << text << "\n";
        std::cout << "Available:" << '\n';
        for(const InstanceBuilder<T> &e : T::all)
            std::cout << e.describe() << '\n';

        return false;
    }
};
template<>
struct ArgParse<Builder>
{
    auto operator()(const char *text, Builder& val) -> bool
    {
        std::string str(text);
        if(!ArgParse<std::unique_ptr<ListBuilder>>()
           (str.substr(0,str.find(':')).c_str(), val.builder)) return false;
        return ArgParse<int>()(text+str.find(':')+1, val.n);
    }
};


auto main(int argc, const char **argv) -> int
{
    std::vector<Builder> builders;
    std::vector<std::unique_ptr<SortAlgorithm>> sorts;
    std::vector<T> list;

    ArgParser ap(argc, argv,
                 Arg(builders, 'c', "create List using Builders", true),
                 Arg(sorts, 'a', "List of Algorithms to run"));

    if(sorts.empty())
        std::ranges::transform(SortAlgorithm::all, std::back_inserter(sorts),
                               [](const auto& i){return i.get();});


    if(!ap.parse_args()){
        std::cout << "Failed paring Args!" << '\n';
        return -1;
    }

    for(auto &b : builders) b.builder->run(list, b.n);

    GUI::instance->start({.ops_per_second = 60});

    std::cout << list << '\n';


    for(auto &s : sorts){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        GUI::instance->set_algorithm(*s);
        GUI::instance->set_speed(60);
        s->run(list);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    GUI::instance->stop();
}
