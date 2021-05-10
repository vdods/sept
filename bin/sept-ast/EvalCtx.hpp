// 2021.04.19 - Victor Dods

// Includes from this program's source
#include "core.hpp"

#include <lvd/aliases.hpp>
#include "sept/GlobalSymRef.hpp"
#include "sept/LocalSymRef.hpp"
#include "sept/SymbolTable.hpp"

namespace sem {

class EvalCtx {
public:

    // Start off using global_symbol_table.
    EvalCtx ()
        :   m_current_scope(sept::global_symbol_table())
    { }

    lvd::nnsp<sept::SymbolTable> const &current_scope () const { return m_current_scope; }

    // This will return a ScopeGuard object which facilitates exception-safe popping of scope.
    // That ScopeGuard object should be kept alive as long as the scope is meant to live.
    [[nodiscard]] lvd::ScopeGuard push_scope () {
        m_current_scope = m_current_scope->push_symbol_table();
        return lvd::ScopeGuard{[this](){ this->pop_scope(); }};
    }
    void pop_scope () {
        // This will throw if there is no parent.
        m_current_scope = m_current_scope->parent_symbol_table();
    }

private:

    lvd::nnsp<sept::SymbolTable> m_current_scope;
};

} // end namespace sem
