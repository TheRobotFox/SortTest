#include "SortList.hpp"

auto operator<< (std::ostream& out, const List& l) -> std::ostream&
{
    return printRange(out, l.list);
}

auto operator<< (std::ostream&  out, const ElementCounter& l) -> std::ostream&
{
    out << l.elem;
    return out;
}

