// 2021.03.31 - Victor Dods

#pragma once

#include <lvd/abort.hpp>
#include <lvd/aliases.hpp>
#include <lvd/fmt.hpp>
#include <lvd/OstreamDelegate.hpp>
#include "sept/core.hpp"
#include "sept/NPType.hpp"
#include "sept/RefTerm.hpp"
#include "sept/SymbolTable.hpp"

namespace sept {

// Refers to a symbol in a locally defined table in memory.  This is one step more context-dependent
// than GlobalSymRefTermImpl.
// NOTE: Don't use this class directly, use sept::LocalSymRef("<symbol-id>", <shared-ptr-to-symbol-table>) to construct a term of that type.
// TODO: Make SymbolTable a first-class type (maybe it could just be an UnorderedMap of some kind)
// and then use a reference to it instead of a shared_ptr.
class LocalSymRefTermImpl : public RefTermBase_i {
public:

    LocalSymRefTermImpl () = default;
    LocalSymRefTermImpl (LocalSymRefTermImpl const &other) = default;
    LocalSymRefTermImpl (LocalSymRefTermImpl &&other) = default;
    LocalSymRefTermImpl (std::string const &symbol_id, lvd::nnsp<SymbolTable> const &symbol_table)
        :   m_symbol_id(symbol_id)
        ,   m_symbol_table(symbol_table)
    { }
    LocalSymRefTermImpl (std::string &&symbol_id, lvd::nnsp<SymbolTable> const &symbol_table)
        :   m_symbol_id(std::move(symbol_id))
        ,   m_symbol_table(symbol_table)
    { }
    virtual ~LocalSymRefTermImpl () { }

    LocalSymRefTermImpl &operator = (LocalSymRefTermImpl const &other) = default;
    LocalSymRefTermImpl &operator = (LocalSymRefTermImpl &&other) = default;

    virtual lvd::nnup<RefTermBase_i> cloned () const override {
        return lvd::make_nnup<LocalSymRefTermImpl>(m_symbol_id, m_symbol_table);
    }

    virtual Data const &referenced_data () const & override;
    virtual Data &referenced_data () & override;
    virtual Data move_referenced_data () && override;

    virtual operator lvd::OstreamDelegate () const override;

    lvd::nnsp<SymbolTable> const &symbol_table () { return m_symbol_table; }

private:

    std::string m_symbol_id;
    lvd::nnsp<SymbolTable> m_symbol_table;
};

// inline RefTerm_c make_local_sym_ref (std::string const &symbol_id) {
//     return RefTerm_c{lvd::make_nnup<LocalSymRefTermImpl>(symbol_id)};
// }
//
// inline RefTerm_c make_local_sym_ref (std::string &&symbol_id) {
//     return RefTerm_c{lvd::make_nnup<LocalSymRefTermImpl>(std::move(symbol_id))};
// }

using LocalSymRefType_c = NonParametricType_t<NPTerm::LOCAL_SYM_REF_TYPE>;
// Normally this would just be a using clause with NonParametricType_t<NPTerm::LOCAL_SYM_REF,RefTerm_c>,
// but because RefTerm_c implements several ref term kinds, this subclass provides a specific
// constructor only for LocalSymRefTermImpl-carrying RefTerm_c instances.
class LocalSymRef_c : public NonParametricType_t<NPTerm::LOCAL_SYM_REF,RefTerm_c,LocalSymRef_c> {
public:

    RefTerm_c operator() (std::string const &symbol_id, lvd::nnsp<SymbolTable> const &symbol_table) const {
//         return make_local_sym_ref(symbol_id);
        return RefTerm_c{lvd::make_nnup<LocalSymRefTermImpl>(symbol_id, symbol_table)};
    }
    RefTerm_c operator() (std::string &&symbol_id, lvd::nnsp<SymbolTable> const &symbol_table) const {
//         return make_local_sym_ref(std::move(symbol_id));
        return RefTerm_c{lvd::make_nnup<LocalSymRefTermImpl>(std::move(symbol_id), symbol_table)};
    }
};

extern LocalSymRef_c LocalSymRef;
extern LocalSymRefType_c LocalSymRefType;

inline constexpr NonParametricType_c const &abstract_type_of (LocalSymRefType_c const &) { return NonParametricType; }
inline constexpr LocalSymRefType_c const &abstract_type_of (LocalSymRef_c const &) { return LocalSymRefType; }

// NOTE: inhabits is not provided for LocalSymRefTermImpl within LocalSymRef_c or Ref_c, since it would violate
// referential transparency.  There needs to be some way to handle a ref without transparently
// dereferencing it.  A few options:
// - Convert to a "MemPtr" analog of MemRef which doesn't automatically dereference
// - Have a different notion of inhabitation that accounts for refs

} // end namespace sept

// TODO -- specialization for std::hash
