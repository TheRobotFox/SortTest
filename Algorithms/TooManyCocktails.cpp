#include "Algos.hpp"
#include <cmath>
#include <random>

static std::random_device rd;
static std::mt19937 gen_rand(rd());

auto TooManyCocktails::sort(List &l) -> void
{
    size_t counter = 0;
    for(size_t i = 0; i<l.size()/2; i++){

        bogo:

        counter++;

        auto start = l.begin()+i,
            end = l.end()-i;

        std::ranges::shuffle(start, end, gen_rand);

        auto min = start,
            max = end-1;
        for(; start<end; start++){
            if(*start>*max) goto bogo;
            if(*start<*min) goto bogo;
        }

    }
    std::cout << "Iterationen: " << counter << '\n';
}
