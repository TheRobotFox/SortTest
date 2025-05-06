#pragma once
#include "SortList.hpp"
#include <cassert>
#include <memory>
#include <chrono>
#include <utility>
#include "UI/UI.hpp"

using TimeStamp = std::chrono::time_point<std::chrono::steady_clock>;
using UI::GUI;

class SortAlgorithm
{
    Stats s;
    TimeStamp time;

protected:

    auto create_list(std::string name, UI::WindowStyle style = {}) -> std::shared_ptr<List>
    {
        auto list = std::make_shared<List>(std::vector<T>(), s);
        auto win = GUI::instance->create_window(list).lock();

        if(win){
            win->style = style;
            win->title = std::move(name);
        }

        return list;
    }
    virtual void sort(List &l) = 0;
public:

    const std::string name;
    const std::string description;
    static std::vector<SortAlgorithm*> all;
    SortAlgorithm(std::string &&name) : name(name)
    {}
    auto run(std::vector<T> in) -> bool
    {
        auto list = std::make_shared<List>(in, s);
        UI::GUI::instance->create_window(list);
        sort(*list);
        std::cout << *list << '\n';
        // TODO: verify
        return false;
    }

        virtual ~SortAlgorithm() = default;
};

void status_reset();
