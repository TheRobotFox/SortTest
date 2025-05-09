#pragma once
#include "../Sort.hpp"
#include <utility>
#include <vector>


class BadBubbleSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
    public:
    BadBubbleSort() : SortAlgorithm("BadBubbleSort")
    {}
};

class BubbleSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
    public:
    BubbleSort() : SortAlgorithm("BubbleSort")
    {}
};

class SelectionSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
    public:
    SelectionSort() : SortAlgorithm("SelectionSort")
    {}
};

class InsertionSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
    public:
    InsertionSort() : SortAlgorithm("InsertionSort")
    {}
};

class StalinSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
    public:
    StalinSort() : SortAlgorithm("StalinSort")
    {}
};

class ReculinSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
    public:
    ReculinSort() : SortAlgorithm("ReculinSort")
    {}
};

class GulagSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
    public:
    GulagSort() : SortAlgorithm("GulagSort")
    {}
};

class QuickSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
    public:
    QuickSort() : SortAlgorithm("QuickSort")
    {}
};

class TooManyCocktails : public SortAlgorithm
{
    auto sort(List &l) -> void override;
    public:
    TooManyCocktails() : SortAlgorithm("TooManyCocktails")
    {}
};

struct SFactory : public Factory<SortAlgorithm>
{
    template<class C>
    constexpr auto add(std::string description = {}){
        Factory<SortAlgorithm>::add<C>(C().name, std::move(description));
        return *this;
    }
};
inline const std::vector<InstanceBuilder<SortAlgorithm>> SortAlgorithm::all = SFactory()
    .add<BadBubbleSort>("Unflagged BubbleSort")
    .add<BubbleSort>("Standard BubbleSort")
    .add<StalinSort>("Everyone out of order is sent on Holiday")
    .add<ReculinSort>("Recursive StalinSort")
    .add<QuickSort>("The Classic")
    .add<SelectionSort>("The Chosen One")
    .add<GulagSort>("Recursive StalinSort Extended")
    .add<TooManyCocktails>("Bogosort on too many Cocktails ")
    .done;
