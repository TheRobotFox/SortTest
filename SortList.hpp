#pragma once
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <mutex>
#include <ranges>
#include <stdexcept>
#include <vector>
#include "UI/OS.hpp"
#include <iostream>
#include "Util.hpp"

struct Stats
{
    size_t comps = 0, swaps = 0, reads = 0;
};

class SortAlgorithm;
namespace UI {

    class GUI;
}

using T = int;


class ElementCounter {
    T elem;
    Stats s;
    friend List;
    friend UI::GUI;
    friend auto operator<< (std::ostream&  out, const ElementCounter& l) -> std::ostream&;
    // provate copy constructor
    ElementCounter(T e)
        : elem(e)
    {}
public:
    auto operator==(ElementCounter &other) -> bool
    {
        s.comps++;
        return this->elem==other.elem;
    }
    auto operator>(ElementCounter &other) -> bool
    {
        s.comps++;
        return this->elem>other.elem;
    }
    auto operator<(ElementCounter &other) -> bool
    {
        s.comps++;
        return this->elem<other.elem;
    }
    auto read() -> T
    {
        s.reads++;
        return elem;
    }
};

class List
{
    Stats &s;
    friend SortAlgorithm;
    friend auto operator<< (std::ostream&  /*out*/, const List& /*l*/) -> std::ostream&;
    friend UI::GUI;
    std::vector<ElementCounter> list;

    using It = decltype(list)::iterator;

    T max;

    std::map<size_t, UI::Color> marks;

    std::mutex mtx;

    auto get_list() -> auto& {return list;}

public:
    template<std::ranges::range R>
    List(R list, Stats& s)
        : s(s)
    {
        max = std::ranges::max(list);
        std::ranges::transform(list, std::back_inserter(this->list),
                               [](T i){return ElementCounter(i);});
    }

    auto swap(It a, It b) -> void;
    auto size() -> size_t
    {
        return list.size();
    }
    auto at(size_t index) -> ElementCounter
    {
        s.reads++;
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
