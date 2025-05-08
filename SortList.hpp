#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <ranges>
#include <raylib.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include "Factory.hpp"
#include "UI/UI.hpp"
#include "Util.hpp"

struct Stats
{
    size_t comps = 0, swaps = 0, reads = 0, assigns = 0;
    auto get_total() const -> size_t {return comps+swaps+reads+assigns;}
};

class Markings
{
    std::mutex mtx;
    size_t last_read;
    std::array<const ElementCounter*,2> last_accessed;
    size_t max_temp = 1;
public:
    enum class Pointer : uint8_t{
        PRIMARY, SECONDARY, NONE
    };
    std::map<const ElementCounter *, UI::Color> marks;

    void accessed(const ElementCounter *e);
    auto get_temp(const ElementCounter *e) const -> float;
    auto get_pointer(const ElementCounter *e) const -> Pointer;
};

class SortAlgorithm;
class GUI;

using T = int;


class ElementCounter {
    T elem;
    Stats *list_stats;
    std::shared_ptr<Stats> own_stats;

    Markings *m;

    friend Markings;
    friend List;
    friend GUI;

    friend auto operator<< (std::ostream&  out, const ElementCounter& l) -> std::ostream&;
    // provate copy constructor
    ElementCounter(T e, Stats *list_stats, Markings *m)
        : elem(e), list_stats(list_stats),
          own_stats(std::make_shared<Stats>()),
          m(m)
    {}
    auto accessed(size_t (Stats::*member));
    auto assign(const ElementCounter &other)
    {
        this->elem = other.elem;
        this->list_stats = other.list_stats;
        this->own_stats = other.own_stats;
        this->m = other.m;
    }
public:
    auto operator=(const ElementCounter &other) -> ElementCounter&;
    auto operator==(ElementCounter &other) -> bool;
    auto operator>(ElementCounter &other) -> bool;
    auto operator<(ElementCounter &other) -> bool;
    auto operator>=(ElementCounter &other) -> bool;
    auto operator<=(ElementCounter &other) -> bool;
    auto read() -> T;
};

class List
{
    Stats &s;
    Markings m;

    friend SortAlgorithm;
    friend auto operator<< (std::ostream&  /*out*/, const List& /*l*/) -> std::ostream&;
    friend GUI;
    std::vector<ElementCounter> list;

    T max;

    std::mutex mtx;

    auto get_list() -> auto& {return list;}

public:

    using Iterator = decltype(list)::iterator;

    template<std::ranges::range R>
    List(R list, Stats& s)
        : s(s)
    {
        max = std::ranges::max(list);
        std::ranges::transform(list, std::back_inserter(this->list),
                               [this,&s](T i){return ElementCounter(i,&s,&m);});
    }
    List(Stats &s)
        : s(s),
            list({}),
            max(std::numeric_limits<T>::min())
    {}

    auto swap(Iterator a, Iterator b) -> void;
    auto size() -> size_t
    {
        return list.size();
    }
    auto at(size_t index) -> ElementCounter&
    {
        if(index >= list.size())
            throw std::out_of_range("List index OOB in read!");
        return list[index];
    }
    auto operator+(size_t offset) {
        return begin()+offset;
    }
    auto operator[](size_t index) -> ElementCounter& {
        return at(index);
    }
    void remove(Iterator a);
    void clear()
    {
        std::lock_guard<std::mutex> g(mtx);
        list.clear();
        max = std::numeric_limits<T>::min();
    }
    void push(ElementCounter &e);
    auto begin() -> Iterator {return list.begin();}
    auto end() -> Iterator {return list.end();}

    auto verify() -> bool;
};


struct ListBuilder {

    static const std::vector<InstanceBuilder<ListBuilder>> all;

    virtual void run(std::vector<T> &list, int n) = 0;
    virtual ~ListBuilder() = default;
};
