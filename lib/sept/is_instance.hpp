// 2020.04.20 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/Data.hpp"

namespace sept {

inline bool is_instance (Data const &value, Data const &type) {
    return inhabits_data(value, type);
}

} // end namespace sept
