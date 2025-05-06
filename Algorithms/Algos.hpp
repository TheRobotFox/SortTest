#pragma once
#include "../Sort.hpp"


class BadBubbleSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
public:
    BadBubbleSort() : SortAlgorithm("BadBubbleSort"){}
};

class BubbleSort : SortAlgorithm
{
    auto sort(List &l) -> void override;
    BubbleSort() : SortAlgorithm("BubbleSort"){}
};

class SelectionSort : SortAlgorithm
{
    auto sort(List &l) -> void override;
    SelectionSort() : SortAlgorithm("SelectionSort"){}
};

class InsertionSort : SortAlgorithm
{
    auto sort(List &l) -> void override;
    InsertionSort() : SortAlgorithm("InsertionSort"){}
};

class StalinSort : SortAlgorithm
{
    auto sort(List &l) -> void override;
    StalinSort() : SortAlgorithm("StalinSort"){}
};


inline std::vector<SortAlgorithm*> SortAlgorithm::all = {
    new BadBubbleSort(),
};
