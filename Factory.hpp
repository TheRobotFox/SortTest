#pragma once

#include <functional>
#include <memory>
#include <vector>

template<class T>
struct InstanceBuilder {
    const std::string name;
    const std::string description;
    const std::function<std::unique_ptr<T>()> get;
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
        done.emplace_back(name, description, []{return std::make_unique<C>();});
        return *this;
    }
};

template<class T>
concept has_factory =
    std::same_as<decltype(T::all), const std::vector<InstanceBuilder<T>>>;
