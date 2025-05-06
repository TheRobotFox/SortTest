#ifndef UTIL_H_
#define UTIL_H_

#include <ostream>
#include <vector>

class List;
auto operator<< (std::ostream&  /*out*/, const List& /*l*/) -> std::ostream&;
class ElementCounter;
auto operator<< (std::ostream&  out, const ElementCounter& l) -> std::ostream&;

template <std::ranges::range R>
auto printRange(std::ostream &out, R &&l) -> std::ostream & {
    out << "[ ";
    for (const auto &e : l)
        out << e << ' ';
    out << ']';
    return out;
}

template<typename T>
auto operator<< (std::ostream&  out, const std::vector<T>& v) -> std::ostream&
{
    return printRange(out, v);
}


#endif // UTIL_H_
