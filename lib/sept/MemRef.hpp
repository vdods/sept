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

// Simply a reference to memory.  Holds a pointer to the referent Data.
// NOTE: Don't use this class directly, use sept::MemRef(&d), where d has type sept::Data, to construct a term of that type.
class MemRefTermImpl : public RefTermBase_i {
public:

    MemRefTermImpl () = default;
    MemRefTermImpl (MemRefTermImpl const &other) = default;
    MemRefTermImpl (MemRefTermImpl &&other) = default;
    explicit MemRefTermImpl (lvd::nnp<Data> ptr) : m_ptr(ptr) { }
    explicit MemRefTermImpl (Data &ref) : m_ptr(&ref) { }
    virtual ~MemRefTermImpl () { }

    MemRefTermImpl &operator = (MemRefTermImpl const &other) = default;
    MemRefTermImpl &operator = (MemRefTermImpl &&other) = default;

    virtual lvd::nnup<RefTermBase_i> cloned () const override { return lvd::make_nnup<MemRefTermImpl>(m_ptr); }

    virtual Data const &referenced_data () const & override { return *m_ptr; }
    virtual Data &referenced_data () & override { return *m_ptr; }
    virtual Data referenced_data () && override;

    virtual operator lvd::OstreamDelegate () const override;

private:

    lvd::nnp<Data> m_ptr;
};

// inline RefTerm_c make_mem_ref (lvd::nnp<Data> ref) {
//     return RefTerm_c{lvd::make_nnup<MemRefTermImpl>(ref)};
// }

using MemRefType_c = NonParametricType_t<NPTerm::MEM_REF_TYPE>;
// Normally this would just be a using clause with NonParametricType_t<NPTerm::MEM_REF,RefTerm_c>,
// but because RefTerm_c implements several ref term kinds, this subclass provides a specific
// constructor only for MemRefTermImpl-carrying RefTerm_c instances.
class MemRef_c : public NonParametricType_t<NPTerm::MEM_REF,RefTerm_c,MemRef_c> {
public:

    RefTerm_c operator() (lvd::nnp<Data> ref) const {
//         return make_mem_ref(ref);
        return RefTerm_c{lvd::make_nnup<MemRefTermImpl>(ref)};
    }
};

extern MemRef_c MemRef;
extern MemRefType_c MemRefType;

inline constexpr NonParametricType_c const &abstract_type_of (MemRefType_c const &) { return NonParametricType; }
inline constexpr MemRefType_c const &abstract_type_of (MemRef_c const &) { return MemRefType; }

// NOTE: inhabits is not provided for MemRefTermImpl within MemRef_c or Ref_c, since it would violate
// referential transparency.  There needs to be some way to handle a ref without transparently
// dereferencing it.  A few options:
// - Convert to a "MemPtr" analog of MemRef which doesn't automatically dereference
// - Have a different notion of inhabitation that accounts for refs

} // end namespace sept

// TODO -- specialization for std::hash
