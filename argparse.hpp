#pragma once

#include <concepts>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <optional>
#include <queue>
#include <string>
#include <tuple>
#include <utility>
#include <sstream>
#include <vector>

template<typename... Args>
class ArgParser;

template<typename T>
class Arg
{
    T& val;
    std::string desc;
    char name;
    bool required;

    template<typename... Args>
    friend class ArgParser;

public:

    Arg(T&val, char name, std::string desc, bool required = false)
        : name(name),
            desc(std::move(desc)),
            val(val),
            required(required)
    {}
};


template<typename T>
struct ArgParse
{};

template<typename T>
struct ArgParse<std::optional<T>> {
    auto operator()(const char* text, std::optional<T>& val) -> bool
    {
        T tmp;
        bool res = ArgParse<T>()(text, tmp);
        val = tmp;
        return res;
    }
};

template<typename T> requires requires (std::istringstream ss, T& e){ss>>e;}
struct ArgParse<T> {
    auto operator()(const char* text, T& val) -> bool
    {
        std::stringstream ss(text);
        if(! (ss>>val)) {
            std::cout << "Could not parse \"" << text << "\" to " << typeid(T).name() << '\n';
            return false;
        }
        return true;
    }
};

template<template<typename T> class C, typename T>
requires std::same_as<std::vector<T>, C<T>>
        || std::same_as<std::queue<T>, C<T>>
        || std::same_as<std::deque<T>, C<T>>
        || std::same_as<std::list<T>, C<T>>
struct ArgParse<C<T>>
{
    auto operator()(const char* text, std::vector<T>& arg) -> bool
    {
        arg.clear();
        auto ins = std::back_inserter(arg);

        std::stringstream ss(text);
        std::string tmp;
        while(std::getline(ss,tmp, ',')){

            // strip string
            tmp.erase(tmp.find_last_not_of(' ')+1);
            tmp.erase(0, tmp.find_first_not_of(' '));

            if(tmp.empty()) continue;

            T e;
            if(!ArgParse<T>()(tmp.c_str(), e)) return false;
            *ins = std::move(e);
        }
        return true;
    }
};

template<>
struct ArgParse<bool>
{
    auto operator()(const char* _, bool& arg) -> bool
    {
        return (arg = true);
    }
};

template<typename... Args>
class ArgParser
{
    std::map<char, const char *> params;
    std::tuple<Args...> args;

    template<typename F>
    void run(F fn)
    {
        std::apply([&fn](auto&... args)
        {(fn(args), ...);}, args);
    }

public:
    auto parse_args() -> bool
    {
        // Print help
        if(params.contains('h')){
            print_help();
            return false; // indicate Error
        }

        bool error = false, missing = false;

        run([this, &error, &missing]<typename  T>(Arg<T>& arg){
                if(!params.contains(arg.name)){
                    if(arg.required){
                        missing = true;
                        std::cout << "Missing Required Argument '" << arg.name << "' for " << arg.desc << '\n';
                    }
                    return;
                };
                error |= !ArgParse<T>()(params[arg.name], arg.val);
                params.erase(arg.name);
            });

        if(!params.empty()){
            std::cout << "Warning: Unknown Arguments!" << '\n';
            for(auto& [k,v] : params){
                std::cout << "  -" << k << " : " << v << '\n';
            }
            std::cout << '\n';
        }
        bool res = true;
        if(error){
            std::cout << "Failed to Parse Argument" << '\n';
            res = false;
        }
        if(missing) {
            std::cout << "Incomplete Parameter Set! use -h to print help" << '\n';
            res = false;
        }
        return res;
    }
    void print_help(){
        std::cout << "Arguments:" << '\n';
        run([]<typename T>(Arg<T>& arg){
                std::cout << (arg.required ? "* " : "  ");
                std::cout << arg.name << " - " << arg.desc << '\n';
            });
    }

    ArgParser(int argc, const char **argv, Args... args)
        : args(std::forward<Args>(args)...)
    {
        for(int i=1; i<argc; i++){
            if(argv[i][0]=='-') params[argv[i][1]] = ((argv[i+1] != nullptr) ? argv[i+1] : "");
        }
    }

};
