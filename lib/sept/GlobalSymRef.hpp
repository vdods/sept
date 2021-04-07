// 2021.03.31 - Victor Dods

#pragma once

#include <lvd/abort.hpp>
#include <lvd/aliases.hpp>
#include <lvd/fmt.hpp>
#include <lvd/OstreamDelegate.hpp>
#include "sept/core.hpp"
#include "sept/NPType.hpp"
#include "sept/RefTerm.hpp"

namespace sept {

// Refers to a symbol in a globally defined table in memory.  This is as context-independent
// as a symbolic reference can get while still operating against a symbol table in memory.
// NOTE: Don't use this class directly, use sept::GlobalSymRef("<symbol-id>") to construct a term of that type.
class GlobalSymRefTermImpl : public RefTermBase_i {
public:

    GlobalSymRefTermImpl () = default;
    GlobalSymRefTermImpl (GlobalSymRefTermImpl const &other) = default;
    GlobalSymRefTermImpl (GlobalSymRefTermImpl &&other) = default;
    explicit GlobalSymRefTermImpl (std::string const &symbol_id) : m_symbol_id(symbol_id) { }
    explicit GlobalSymRefTermImpl (std::string &&symbol_id) : m_symbol_id(std::move(symbol_id)) { }
    virtual ~GlobalSymRefTermImpl () { }

    GlobalSymRefTermImpl &operator = (GlobalSymRefTermImpl const &other) = default;
    GlobalSymRefTermImpl &operator = (GlobalSymRefTermImpl &&other) = default;

    virtual lvd::nnup<RefTermBase_i> cloned () const override { return lvd::make_nnup<GlobalSymRefTermImpl>(m_symbol_id); }

    virtual Data const &referenced_data () const & override;
    virtual Data &referenced_data () & override;
    virtual Data referenced_data () && override;

    virtual operator lvd::OstreamDelegate () const override;

    static std::unordered_map<std::string,Data> &symbol_table () { return ms_symbol_table; }

private:

    std::string m_symbol_id;

    static std::unordered_map<std::string,Data> ms_symbol_table;
};

// TODO: Make a context-dependent symbolic ref, which has the symbol_id and also a pointer
// (or maybe RefTermBase_i) to the symbol table that it depends on.  GlobalSymRefTermImpl is a special
// case where the pointer to the table is known at compile time, so it doesn't have to be
// stored along with the reference.

// inline RefTerm_c make_global_sym_ref (std::string const &symbol_id) {
//     return RefTerm_c{lvd::make_nnup<GlobalSymRefTermImpl>(symbol_id)};
// }
//
// inline RefTerm_c make_global_sym_ref (std::string &&symbol_id) {
//     return RefTerm_c{lvd::make_nnup<GlobalSymRefTermImpl>(std::move(symbol_id))};
// }

using GlobalSymRefType_c = NonParametricType_t<NPTerm::GLOBAL_SYM_REF_TYPE>;
// Normally this would just be a using clause with NonParametricType_t<NPTerm::GLOBAL_SYM_REF,RefTerm_c>,
// but because RefTerm_c implements several ref term kinds, this subclass provides a specific
// constructor only for GlobalSymRefTermImpl-carrying RefTerm_c instances.
class GlobalSymRef_c : public NonParametricType_t<NPTerm::GLOBAL_SYM_REF,RefTerm_c,GlobalSymRef_c> {
public:

    RefTerm_c operator() (std::string const &symbol_id) const {
//         return make_global_sym_ref(symbol_id);
        return RefTerm_c{lvd::make_nnup<GlobalSymRefTermImpl>(symbol_id)};
    }
    RefTerm_c operator() (std::string &&symbol_id) const {
//         return make_global_sym_ref(std::move(symbol_id));
        return RefTerm_c{lvd::make_nnup<GlobalSymRefTermImpl>(std::move(symbol_id))};
    }
};

extern GlobalSymRef_c GlobalSymRef;
extern GlobalSymRefType_c GlobalSymRefType;

inline constexpr NonParametricType_c const &abstract_type_of (GlobalSymRefType_c const &) { return NonParametricType; }
inline constexpr GlobalSymRefType_c const &abstract_type_of (GlobalSymRef_c const &) { return GlobalSymRefType; }

// NOTE: inhabits is not provided for GlobalSymRefTermImpl within GlobalSymRef_c or Ref_c, since it would violate
// referential transparency.  There needs to be some way to handle a ref without transparently
// dereferencing it.  A few options:
// - Convert to a "MemPtr" analog of MemRef which doesn't automatically dereference
// - Have a different notion of inhabitation that accounts for refs

} // end namespace sept

// TODO -- specialization for std::hash
