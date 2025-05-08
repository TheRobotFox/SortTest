#pragma once
#include "../Sort.hpp"


class BadBubbleSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
public:
    BadBubbleSort() : SortAlgorithm("BadBubbleSort"){}
};

class BubbleSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
public:
    BubbleSort() : SortAlgorithm("BubbleSort"){}
};

class SelectionSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
public:
    SelectionSort() : SortAlgorithm("SelectionSort"){}
};

class InsertionSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
public:
    InsertionSort() : SortAlgorithm("InsertionSort"){}
};

class StalinSort : public SortAlgorithm
{
    auto sort(List &l) -> void override;
public:
    StalinSort() : SortAlgorithm("StalinSort"){}
};


inline std::vector<SortAlgorithm*> SortAlgorithm::all = {
    new BadBubbleSort(),
    new BubbleSort(),
};
