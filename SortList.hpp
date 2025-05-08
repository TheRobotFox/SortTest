#pragma once
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <mutex>
#include <ranges>
#include <stdexcept>
#include <vector>
#include <iostream>
#include "Util.hpp"

struct Stats
{
    size_t comps = 0, swaps = 0, reads = 0;
};

struct Markings
{
    size_t last_read;
    std::array<ElementCounter*,2> last_cmpared, last_assigned;

    void assinged(ElementCounter *e){
        last_assigned[1] = last_assigned[0];
        last_assigned[0] = e;
    }
};

class SortAlgorithm;
namespace UI {

    class GUI;
}

using T = int;


class ElementCounter {
    T elem;
    Stats *s;
    Markings *m;

    friend List;
    friend UI::GUI;
    friend auto operator<< (std::ostream&  out, const ElementCounter& l) -> std::ostream&;
    // provate copy constructor
    ElementCounter(T e, Stats *s, Markings *m)
        : elem(e), s(s), m(m)
    {}
public:
    auto operator=(const ElementCounter &other) -> ElementCounter&;
    auto operator==(const ElementCounter &other) -> bool;
    auto operator>(const ElementCounter &other) -> bool;
    auto operator<(const ElementCounter &other) -> bool;
    auto read() -> T;
};

class List
{
    Stats &s;
    Markings m;

    friend SortAlgorithm;
    friend auto operator<< (std::ostream&  /*out*/, const List& /*l*/) -> std::ostream&;
    friend UI::GUI;
    std::vector<ElementCounter> list;

    using It = decltype(list)::iterator;

    T max;


    std::mutex mtx;

    auto get_list() -> auto& {return list;}

public:
    template<std::ranges::range R>
    List(R list, Stats& s)
        : s(s)
    {
        max = std::ranges::max(list);
        std::ranges::transform(list, std::back_inserter(this->list),
                               [this,&s](T i){return ElementCounter(i,&s,&m);});
    }

    auto swap(It a, It b) -> void;
    auto size() -> size_t
    {
        return list.size();
    }
    auto at(size_t index) -> ElementCounter&
    {
        if(index>= list.size())
            throw std::out_of_range("List index OOB in read!");
        return list[index];
    }
    auto operator+(size_t offset) {
        return begin()+offset;
    }
    auto operator[](size_t index) {
        return at(index);
    }
    void remove(It a);
    auto begin() -> It {return list.begin();}
    auto end() -> It {return list.end();}
};


struct ListBuilder {
    const char* name;
    const char* description;

    static std::vector<ListBuilder*> all;

    virtual void run(std::vector<T> &list, int n) = 0;
    ListBuilder(const char *name, const char *desc)
        : name(name), description(desc)
    {}

    virtual ~ListBuilder() = default;
};
