#pragma once
#include "SortList.hpp"
#include "Factory.hpp"
#include <cassert>
#include <memory>
#include <chrono>
#include <utility>
#include "UI/GUI.hpp"

using TimeStamp = std::chrono::time_point<std::chrono::steady_clock>;

class SortAlgorithm
{
    Stats s;
    TimeStamp time;

protected:

    auto create_list(std::string name, WindowStyle style = {}) -> std::shared_ptr<List>
    {
        auto list = std::make_shared<List>(s);
        auto win = GUI::instance->create_window(list).lock();

        if(win){
            win->style = style;
            win->title = std::move(name);
        }

        return list;
    }
    virtual void sort(List &l) = 0;
public:

    std::string name;
    SortAlgorithm(std::string name)
        : name(std::move(name))
    {}
    static const std::vector<InstanceBuilder<SortAlgorithm>> all;
    auto run(std::vector<T> in) -> bool
    {
        auto list = std::make_shared<List>(in, s);
        GUI::instance->create_window(list);
        sort(*list);
        bool res = list->verify();
        std::cout << *list << '\n';
        return res;
    }

    virtual ~SortAlgorithm() = default;
};

void status_reset();
