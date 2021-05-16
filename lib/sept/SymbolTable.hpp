// 2021.04.07 - Victor Dods

#pragma once

#include <lvd/aliases.hpp>
#include <lvd/fmt.hpp>
#include <lvd/literal.hpp>
#include <memory>
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include <unordered_map>

namespace sept {

// TODO: Maybe add an identifier to the table
// TODO: Maybe add unresolved symbol handler that could either return Data or throw.
// TODO: Allow the symbol ID to be Data, so that more sophisticated symbols can be used, e.g.
// canonically induced symbols such as ConcreteTypeOf(x), where x is a symbol; this would theoretically
// allow the symbol table entry for x to only know its storage layout, because then ConcreteTypeOf(x)
// would dictate the rest.
class SymbolTable : public std::enable_shared_from_this<SymbolTable> {
public:

    using SymbolMap = std::unordered_map<std::string,Data>;

    SymbolTable () = default;
    SymbolTable (SymbolMap &&symbol_map, lvd::sp<SymbolTable> const &parent_symbol_table)
        :   m_symbol_map(std::move(symbol_map))
        ,   m_parent_symbol_table(parent_symbol_table)
    { }

    Data const &resolve_symbol_const (std::string const &symbol_id) const noexcept(false);
    // Please don't modify ms_unresolved_symbol_nonconst!
    Data &resolve_symbol_nonconst (std::string const &symbol_id) noexcept(false);

    // TODO: Maybe add a "is locally defined" option
    bool symbol_is_defined (std::string const &symbol_id) const noexcept;
    // This will throw if the symbol is already defined in this SymbolTable (though it's fine
    // if it's defined in m_parent_symbol_table or higher).
    void define_symbol (std::string const &symbol_id, Data const &value) noexcept(false);
    // This will throw if the symbol is already defined in this SymbolTable (though it's fine
    // if it's defined in m_parent_symbol_table or higher).
    void define_symbol (std::string const &symbol_id, Data &&value) noexcept(false);

    SymbolMap const &symbol_map () const { return m_symbol_map; }
    SymbolMap &symbol_map () { return m_symbol_map; }
    bool has_parent_symbol_table () const { return m_parent_symbol_table != nullptr; }
    // This will throw if there is no parent SymbolTable.
    lvd::nnsp<SymbolTable> parent_symbol_table () const noexcept(false);

    // This constructs a symbol table whose parent is this one.
    lvd::nnsp<SymbolTable> push_symbol_table ();
    // Clears the SymbolMap and nullifies parent_symbol_table
    void clear ();

private:

    SymbolMap m_symbol_map;
    lvd::sp<SymbolTable> m_parent_symbol_table;
};

std::ostream &operator<< (std::ostream &out, SymbolTable const &symbol_table);

} // end namespace sept
