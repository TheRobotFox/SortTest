#include "Factory.hpp"
#include "argparse.hpp"
#include "Sort.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <raylib.h>
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

                val = std::move(e.template get<P>());
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
    std::vector<std::shared_ptr<SortAlgorithm>> sorts;
    std::vector<T> list;
    std::optional<float> operations_per_second {};

    std::ranges::transform(SortAlgorithm::all, std::back_inserter(sorts),
                           [](const auto& i){return i.template get<std::shared_ptr<SortAlgorithm>>();});
    ArgParser ap(argc, argv,
                 Arg(list, 'l', "read List from argument"),
                 Arg(builders, 'c', "create List using Builders"),
                 Arg(operations_per_second, 'v', "Visualisation operations per second"),
                 Arg(sorts, 'a', "List of Algorithms to run"));



    if(!ap.parse_args()){
        std::cout << "Failed paring Args!" << '\n';
        return -1;
    }
    for(auto &b : builders) b.builder->run(list, b.n);

    if(list.empty()){
        std::cout << "WARNING: List is empty! Use -h to see how to create one\n";
    }

    float ops = operations_per_second.value_or(0);
    if(operations_per_second) GUI::instance->start({.ops_per_second = ops});

    std::cout << list << '\n';

    try {
        for(auto &s : sorts){
            GUI::instance->set_algorithm(s);
            GUI::instance->set_speed(ops);
            s->run(list);
        }
    } catch(GUI::ExitRequest &e){
        std::cout << "Exit!\n";
    }

    GUI::instance->stop();
}
