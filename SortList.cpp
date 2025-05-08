#include "SortList.hpp"
#include <iterator>
#include <mutex>
#include <random>
#include <ranges>
#include <thread>
#include <vector>
#include "UI/UI.hpp"


auto ElementCounter::operator==(const ElementCounter &other) -> bool
{
    s->comps++;
    UI::GUI::instance->wait_and_handle();
    return this->elem==other.elem;
}
auto ElementCounter::operator>(const ElementCounter &other) -> bool
{
    s->comps++;
    UI::GUI::instance->wait_and_handle();
    return this->elem>other.elem;
}
auto ElementCounter::operator<(const ElementCounter &other) -> bool
{
    s->comps++;
    UI::GUI::instance->wait_and_handle();
    return this->elem<other.elem;
}
auto ElementCounter::read() -> T
{
    s->reads++;
    UI::GUI::instance->wait_and_handle();
    return elem;
}

auto ElementCounter::operator=(const ElementCounter &other) -> ElementCounter&
{
    this->elem = other.elem;
    this->s = other.s;
    this->m = other.m;
    // TODO stats
    m->assinged(this);
    return *this;
}


auto List::swap(It a, It b) -> void
{
    s.swaps++;
    if(a>=list.end() || b>=list.end())
        throw std::out_of_range("List index OOB in swap!");

    mtx.lock();
    auto tmp = *a;
    *a = *b;
    *b = tmp;
    mtx.unlock();
    UI::GUI::instance->wait_and_handle();
}

void List::remove(It a)
{
    if(a>= list.end())
        throw std::out_of_range("List a OOB in remove!");

    bool dirty_max = max == a->elem;

    {
        std::lock_guard<std::mutex> g(mtx);
        list.erase(a);
    }

    namespace r = std::ranges;
    if(dirty_max) max = r::max(list | r::views::transform([](ElementCounter& e){return e.elem;}));
    UI::GUI::instance->wait_and_handle();
}


// List Builders

std::random_device rd;
std::mt19937 gen_rand(rd());

struct FillShuffle : ListBuilder
{
    void run(std::vector<T> &l, int n) override
    {

        using namespace std::ranges;

        auto tmp = views::iota(1)
                    | views::take(n)
                    | to<std::vector>();

        shuffle(tmp, gen_rand);
        copy(tmp, std::back_inserter(l));
    }
    FillShuffle()
        : ListBuilder("fill_shfl", "Insert elements 1..n Suffled")
    {}
};

struct Random : ListBuilder
{
    int scale = 10;
    void run(std::vector<T> &l, int n) override
    {
        using namespace std::ranges;

        std::vector<T> tmp(0, n);
        // generate_random(tmp, gen_rand);

        copy(tmp, std::back_inserter(l));
    }
    Random()
        : ListBuilder("random", "Insert n random elements")
    {}
};
struct FillReversed : ListBuilder
{
    void run(std::vector<T> &l, int n) override
    {
        using namespace std::ranges;

        copy(views::iota(1) | views::take(n) | views::reverse,
             std::back_inserter(l));
    }
    FillReversed()
        : ListBuilder("fill_rev", "Insert elements n..1")
    {}
};
struct Fill : ListBuilder
{
    void run(std::vector<T> &l, int n) override
    {
        using namespace std::ranges;

        copy(views::iota(1) | views::take(n),
             std::back_inserter(l));
    }
    Fill() : ListBuilder("fill", "Insert elements 1..n")
    {}
};

std::vector<ListBuilder*> ListBuilder::all = {
    new Fill(),
    new FillReversed(),
    new FillShuffle(),
    new Random()
    };
