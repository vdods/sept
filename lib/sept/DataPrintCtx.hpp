// 2021.04.13 - Victor Dods

#pragma once

#include "sept/core.hpp"

#include <lvd/ScopeGuard.hpp>
#include <unordered_set>

namespace sept {

class Data;

class DataPrintCtx {
public:

    DataPrintCtx () = default;

    bool has_been_visited (Data const *ptr) const { return m_visited_data_ptrs.find(ptr) != m_visited_data_ptrs.end(); }
    void mark_visited (Data const *ptr) { m_visited_data_ptrs.insert(ptr); }
//     void unmark_visited (Data const *ptr) { m_visited_data_ptrs.erase(ptr); }
    lvd::ScopeGuard push (Data const *ptr) {
        m_visited_data_ptrs.insert(ptr);
        return lvd::ScopeGuard([=](){ m_visited_data_ptrs.erase(ptr); });
    }

    std::unordered_set<Data const *> const visited_data_ptrs () const { return m_visited_data_ptrs; }

private:

    std::unordered_set<Data const *> m_visited_data_ptrs;
};

inline std::ostream &operator<< (std::ostream &out, DataPrintCtx const &ctx) {
    out << "DataPrintCtx{";
    for (auto const *ptr : ctx.visited_data_ptrs())
        out << ptr << ", ";
    return out << '}';
}

} // end namespace sept
