// 2021.05.07 - Victor Dods

#pragma once

#include <lvd/fmt.hpp>
#include <stdexcept>
#include <utility>

#define ABSTRACT_TYPE_CONSTRUCTOR \
    template <typename Argument_> \
    Argument_ operator() (Argument_ &&argument) const { \
        if (!inhabits(argument, *this)) \
            throw std::runtime_error(LVD_FMT("argument " << argument << " does not inhabit abstract type " << *this)); \
        return std::forward<Argument_>(argument); \
    }
