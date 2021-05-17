// 2021.05.16 - Victor Dods

#include "common.hpp"
#include "sept/Data.hpp"
#include "sept/SymbolTable.hpp"

class Match {
public:

    Match (sept::Data &&matched_content, sept::SymbolTable &&symbol_assignment)
        :   m_matched_content(std::move(matched_content))
        ,   m_symbol_assignment(std::move(symbol_assignment))
    { }

    sept::Data const &matched_content () const { return m_matched_content; }
    sept::SymbolTable const &symbol_assignment () const { return m_symbol_assignment; }

private:

    sept::Data m_matched_content;
    sept::SymbolTable m_symbol_assignment;
};

std::ostream &operator<< (std::ostream &out, Match const &match);

// This will return std::nullopt if there is no match, otherwise a populated Match struct.
std::optional<Match> matched_pattern__data (sept::Data const &pattern, sept::Data &&term);

// Substitute FreeVars with their SymbolTable-defined values in a term.
sept::Data free_var_substitution__data (sept::Data const &term, sept::SymbolTable const &symbol_assignment) noexcept;
