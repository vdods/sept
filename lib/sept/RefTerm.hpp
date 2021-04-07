// 2021.03.31 - Victor Dods

#pragma once

#include <lvd/abort.hpp>
#include <lvd/aliases.hpp>
#include <lvd/fmt.hpp>
#include <lvd/OstreamDelegate.hpp>
#include "sept/core.hpp"

namespace sept {

class Data;

//
// Interface for defining custom transparent reference behavior.
//

class RefTermBase_i {
public:

    virtual ~RefTermBase_i () = 0;

    // Returns a clone of this.
    virtual lvd::nnup<RefTermBase_i> cloned () const = 0;

    // Value of the referenced Data.
    virtual Data const &referenced_data () const & = 0;
    // Value of the referenced Data.
    virtual Data &referenced_data () & = 0;
    // Value of the referenced Data.
    virtual Data referenced_data () && = 0;

    virtual operator lvd::OstreamDelegate () const = 0;
};

inline RefTermBase_i::~RefTermBase_i () = default;

//
// RefTerm_c
//

// This class acts as a delegate for the specific kinds of references, each of which implement
// the RefTermBase_i interface.
class RefTerm_c {
public:

    RefTerm_c () = default;
    RefTerm_c (RefTerm_c const &other) : m_ref_base(other.m_ref_base->cloned()) { }
    RefTerm_c (RefTerm_c &&other) = default;
    explicit RefTerm_c (lvd::nnup<RefTermBase_i> &&ref_base) : m_ref_base(std::move(ref_base)) { }

    RefTerm_c &operator = (RefTerm_c const &other) = default;
    RefTerm_c &operator = (RefTerm_c &&other) = default;

    // Value of the referenced Data.
    Data const &referenced_data () const & { return ref_base_get().referenced_data(); }
    // Value of the referenced Data.
    Data &referenced_data () & { return ref_base_get().referenced_data(); }
    // Value of the referenced Data.
    // TODO: Figure out if this one makes sense to have.
    Data referenced_data () &&;

    // Forward this call to the referenced_data.  Can only do so through registered construct_inhabitant_of functions.
    Data operator() (Data const &argument) const;
    // Forward this call to the referenced_data.  Can only do so through registered element_of functions.
    Data operator[] (Data const &param) const;

    operator lvd::OstreamDelegate () const;

    // This should only be used by inhabits
    RefTermBase_i const &ref_base_get () const & { return static_cast<RefTermBase_i const &>(*m_ref_base.get().get()); }

private:

    RefTermBase_i &ref_base_get () & { return static_cast<RefTermBase_i &>(*m_ref_base.get().get()); }
    RefTermBase_i &&ref_base_get () && { return static_cast<RefTermBase_i &&>(*m_ref_base.get().get()); }

    lvd::nnup<RefTermBase_i> m_ref_base;
};

bool operator== (RefTerm_c const &lhs, RefTerm_c const &rhs);
// // NOTE: These aren't yet present in DataEq registration, since it isn't yet build to take asymmetric type equality
// bool operator== (RefTerm_c const &lhs, Data const &rhs);
// bool operator== (Data const &lhs, RefTerm_c const &rhs);

// Forward to referenced_data
Data abstract_type_of (RefTerm_c const &r);

// TODO: Figure out how to use a templatized function.  For now, use specific overloads
// // Forward element_of to referenced_data.
// template <typename Index_>
// Data element_of (RefTerm_c const &r, Index_ index) {
//     return r[index];
// }
Data element_of (RefTerm_c const &r, int8_t index);
Data element_of (RefTerm_c const &r, int16_t index);
Data element_of (RefTerm_c const &r, int32_t index);
Data element_of (RefTerm_c const &r, int64_t index);
Data element_of (RefTerm_c const &r, uint8_t index);
Data element_of (RefTerm_c const &r, uint16_t index);
Data element_of (RefTerm_c const &r, uint32_t index);
Data element_of (RefTerm_c const &r, uint64_t index);

// Forward inhabit to referenced_data of each.
bool inhabits (RefTerm_c const &value, RefTerm_c const &type);

// TODO
// void serialize (RefTerm_c const &v, std::ostream &out);
//
// // This assumes that the abstract_type portion following the SerializedTopLevelCode::PARAMETRIC_TERM
// // has already been read in; that value is passed in as abstract_type.
// RefTerm_c deserialize_value_RefTerm (Data &&abstract_type, std::istream &in);

} // end namespace sept

// TODO
// namespace std {
//
// // Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// // This is generally done by combining the hashes of the typeid of the argument and its content.
//
// template <>
// struct hash<sept::RefTerm_c> {
//     size_t operator () (sept::RefTerm_c const &t) const {
//         return std::hash<sept::RefTerm_c???>()(t);
//     }
// };
//
// } // end namespace std
