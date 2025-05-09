#include "SortList.hpp"
#include <cstddef>
#include <iterator>
#include <limits>
#include <mutex>
#include <random>
#include <ranges>
#include <thread>
#include <vector>
#include "Factory.hpp"
#include "UI/GUI.hpp"


void Markings::accessed(const ElementCounter *e)
{
    std::lock_guard<std::mutex> g(mtx);
    max_temp = std::max(e->own_stats->get_total(), max_temp);

    last_accessed[1] = last_accessed[0];
    last_accessed[0] = e;

}
auto Markings::get_temp(const ElementCounter *e) const -> float
{
    return e->own_stats->get_total()/(float)max_temp;
}
auto Markings::get_pointer(const ElementCounter *e) const -> Pointer
{
    if(last_accessed[0]==e) return Pointer::PRIMARY;
    if(last_accessed[1]==e) return Pointer::SECONDARY;
    return Pointer::NONE;
}

auto ElementCounter::accessed(size_t (Stats::*member))
{
    (list_stats->*member)++;
    (own_stats.get()->*member)++;
    m->accessed(this);
    GUI::instance->wait_and_handle();
}

auto ElementCounter::operator==(ElementCounter &other) -> bool
{
    accessed(&Stats::comps);
    other.accessed(&Stats::comps);
    return this->elem==other.elem;
}
auto ElementCounter::operator>(ElementCounter &other) -> bool
{
    accessed(&Stats::comps);
    other.accessed(&Stats::comps);
    return this->elem>other.elem;
}
auto ElementCounter::operator<(ElementCounter &other) -> bool
{
    accessed(&Stats::comps);
    return this->elem<other.elem;
}
auto ElementCounter::operator>=(ElementCounter &other) -> bool
{
    accessed(&Stats::comps);
    return this->elem>=other.elem;
}
auto ElementCounter::operator<=(ElementCounter &other) -> bool
{
    accessed(&Stats::comps);
    return this->elem<=other.elem;
}
auto ElementCounter::read() -> T
{
    accessed(&Stats::reads);
    return elem;
}

auto ElementCounter::operator=(const ElementCounter &other) -> ElementCounter&
{
    assign(other);
    accessed(&Stats::assigns);
    return *this;
}


auto List::swap(Iterator a, Iterator b) -> void
{
    s.swaps++;
    if(a>=list.end() || b>=list.end())
        throw std::out_of_range("List index OOB in swap!");

    const auto& tmp = *a;
    *a = *b;
    *b = tmp;
}

void List::remove(Iterator a)
{
    if(a>= list.end())
        throw std::out_of_range("List a OOB in remove!");

    if(a<end()-1){
        mtx.lock();

        if(a->elem==max){
            a->elem = std::numeric_limits<T>::min();
            namespace r = std::ranges;
            max = r::max(list | r::views::transform([](ElementCounter& e){return e.elem;}));
        }

        mtx.unlock();

        while (++a<end()) (a-1)->assign(*a);
    }

    std::lock_guard<std::mutex> g(mtx);
    list.pop_back();
}

auto List::verify() -> bool
{

    auto it = list.begin();
    m.marks[it.base()] = {.g = 200};

    SlowDown s(*GUI::instance, size()/2);

    while(++it<list.end()){
        if((it-1)->elem > it->elem){
            m.marks[it.base()] = {.r = 200};
            std::this_thread::sleep_for(std::chrono::seconds(2));
            return false;
        }
        m.marks[it.base()] = {.g = 200};
        GUI::instance->wait_and_handle();
    }
    return true;
}

void List::push(ElementCounter &e)
{
    std::lock_guard<std::mutex> g(mtx);
    list.push_back(ElementCounter(e.elem, &s, &m));
    max = std::max(max, e.elem);
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
};

struct Random : ListBuilder
{
    int scale = 10;
    void run(std::vector<T> &l, int n) override
    {
        using namespace std::ranges;

        auto dist = std::uniform_int_distribution<T>(1,scale*n);
        while(n--) l.push_back(dist(gen_rand));
    }
};
struct FillReversed : ListBuilder
{
    void run(std::vector<T> &l, int n) override
    {
        using namespace std::ranges;

        copy(views::iota(1) | views::take(n) | views::reverse,
             std::back_inserter(l));
    }
};
struct Fill : ListBuilder
{
    void run(std::vector<T> &l, int n) override
    {
        using namespace std::ranges;

        copy(views::iota(1) | views::take(n),
             std::back_inserter(l));
    }
};

const std::vector<InstanceBuilder<ListBuilder>> ListBuilder::all = Factory<ListBuilder>()
    .add<FillShuffle>("fill_shfl", "Insert elements 1..n Suffled")
    .add<Random>("random", "Insert n random elements")
    .add<FillReversed>("fill_rev", "Insert elements n..1")
    .add<Fill>("fill", "Insert elements 1..n")
    .done;
