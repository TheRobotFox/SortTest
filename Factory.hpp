#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <vector>


template<class T>
class InstanceBuilder {

    const std::function<T*()> construct;
public:
    InstanceBuilder(std::string name, std::string description, std::function<T*()> c)
        : name(std::move(name)), description(std::move(description)), construct(c)
    {}
    const std::string name;
    const std::string description;
    template<class P>
    constexpr auto get() const -> P {return P(construct());}
    constexpr auto describe() const -> std::string
    {
        if(description.empty()) return name;
        return name + " - " + description;
    }
};

template<class T>
struct Factory {
    std::vector<InstanceBuilder<T>> done;
    template<class C>
    constexpr auto add(std::string name, std::string description = {}) -> Factory<T>&
    {
        done.emplace_back(name, description, []{return new C();});
        return *this;
    }
};

template<class T>
concept has_factory =
    std::same_as<decltype(T::all), const std::vector<InstanceBuilder<T>>>;
