// 2021.05.16 - Victor Dods

#include "sept/SymbolTable.hpp"

namespace sept {

Data const &SymbolTable::resolve_symbol_const (std::string const &symbol_id) const noexcept(false) {
    auto it = m_symbol_map.find(symbol_id);
    if (it != m_symbol_map.end())
        return it->second;
    else if (m_parent_symbol_table != nullptr)
        return m_parent_symbol_table->resolve_symbol_const(symbol_id); // Fall through to parent, if it exists
    else
        throw std::runtime_error(LVD_FMT("Symbol " << lvd::literal_of(symbol_id) << " is not defined"));
}

Data &SymbolTable::resolve_symbol_nonconst (std::string const &symbol_id) noexcept(false) {
    auto it = m_symbol_map.find(symbol_id);
    if (it != m_symbol_map.end())
        return it->second;
    else if (m_parent_symbol_table != nullptr)
        return m_parent_symbol_table->resolve_symbol_nonconst(symbol_id); // Fall through to parent, if it exists
    else
        throw std::runtime_error(LVD_FMT("Symbol " << lvd::literal_of(symbol_id) << " is not defined"));
}

bool SymbolTable::symbol_is_defined (std::string const &symbol_id) const noexcept {
    auto it = m_symbol_map.find(symbol_id);
    if (it != m_symbol_map.end())
        return true;
    else if (m_parent_symbol_table != nullptr)
        return m_parent_symbol_table->symbol_is_defined(symbol_id); // Fall through to parent, if it exists
    else
        return false;
}

void SymbolTable::define_symbol (std::string const &symbol_id, Data const &value) noexcept(false) {
    auto it = m_symbol_map.find(symbol_id);
    if (it != m_symbol_map.end())
        throw std::runtime_error(LVD_FMT("Symbol " << lvd::literal_of(symbol_id) << " already defined; can't re-define.  Existing value is " << it->second << ", new value is " << value));
    m_symbol_map.emplace(symbol_id, value);
}

void SymbolTable::define_symbol (std::string const &symbol_id, Data &&value) noexcept(false) {
    auto it = m_symbol_map.find(symbol_id);
    if (it != m_symbol_map.end())
        throw std::runtime_error(LVD_FMT("Symbol " << lvd::literal_of(symbol_id) << " already defined; can't re-define.  Existing value is " << it->second << ", new value is " << value));
    m_symbol_map.emplace(symbol_id, std::move(value));
}

lvd::nnsp<SymbolTable> SymbolTable::parent_symbol_table () const noexcept(false) {
    if (m_parent_symbol_table == nullptr)
        throw std::runtime_error("this SymbolTable has no parent SymbolTable");
    return m_parent_symbol_table;
}

lvd::nnsp<SymbolTable> SymbolTable::push_symbol_table () {
    return lvd::make_nnsp<SymbolTable>(SymbolMap{}, shared_from_this());
}

void SymbolTable::clear () {
    m_symbol_map.clear();
    m_parent_symbol_table.reset();
}

std::ostream &operator<< (std::ostream &out, SymbolTable const &symbol_table) {
    lvd::Log log(out);
    log << "SymbolTable(\n";
    {
        auto ig1 = lvd::IndentGuard(log);
        log << "symbol_map = {\n";
        {
            auto ig2 = lvd::IndentGuard(log);
            for (auto const &it : symbol_table.symbol_map())
                log << lvd::literal_of(it.first) << " -> " << it.second << '\n';
        }
        log << "}\n";
        log << "has_parent_symbol_table = " << std::boolalpha << symbol_table.has_parent_symbol_table() << '\n';
    }
    log << ')';
    return out;
}

} // end namespace sept
