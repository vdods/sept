// 2020.07.25 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/hash.hpp"
#include "sept/BaseArray_t.hpp"

namespace sept {

// The T stands for "Typed", meaning that the abstract type is specified as a runtime value.
// This is just an implementation detail to help implement other classes.
// The idea is to provide basic DataVector functionality that can be used by derived classes,
// as well as the abstract type of the thing, stored as a runtime value.
template <typename Derived_ = DerivedNone>
class BaseArrayT_t : public BaseArray_t<lvd::static_if_t<std::is_same_v<Derived_,DerivedNone>,BaseArrayT_t<>,Derived_>> {
public:

    using Derived = lvd::static_if_t<std::is_same_v<Derived_,DerivedNone>,BaseArrayT_t,Derived_>;
    using ParentClass = BaseArray_t<Derived>;

    template <typename AbstractType_t, typename... Args_>
    BaseArrayT_t (AbstractType_t &&abstract_type, Args_&&... args)
    :   ParentClass(std::forward<Args_>(args)...)
    ,   m_abstract_type(std::forward<AbstractType_t>(abstract_type)) {
    }
    BaseArrayT_t (BaseArrayT_t const &other)
    :   ParentClass(other)
    ,   m_abstract_type(other.m_abstract_type) {
    }
    BaseArrayT_t (BaseArrayT_t &&other)
    :   ParentClass(std::move(static_cast<ParentClass&&>(other))) // Not sure if the static_cast is necessary.
    ,   m_abstract_type(std::move(other.m_abstract_type)) {
    }

    BaseArrayT_t &operator = (BaseArrayT_t const &other) = default;
    BaseArrayT_t &operator = (BaseArrayT_t &&other) = default;

    // Constructs a Term of this type, setting this as its Type.  Note the use DerivedNone, which causes
    // the Term to be "generic".
    template <typename... Args_>
    BaseArrayT_t<DerivedNone> operator () (Args_&&... args) const {
        return BaseArrayT_t<DerivedNone>(*this, make_DataVector(std::forward<Args_>(args)...));
    }

    bool operator == (Data const &other) const {
        return (other.type() == typeid(Derived)) && this->operator==(other.cast<Derived const &>());
    }
    bool operator == (BaseArrayT_t const &other) const {
        return m_abstract_type == other.m_abstract_type && static_cast<ParentClass const &>(*this) == static_cast<ParentClass const &>(other);
    }
    bool operator != (BaseArrayT_t const &other) const { return !(*this == other); }

    Data const &abstract_type () const & { return m_abstract_type; }
    Data &abstract_type () & { return m_abstract_type; }
    Data abstract_type () && { return std::move(m_abstract_type); }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "BaseArrayT_t<" << abstract_type() << '>' << this->elements();
        });
    }

private:

    Data m_abstract_type;
};

// // This is used to construct BaseArrayT_t more efficiently (std::initializer_list lacks move semantics for some dumb
// // reason), as well as to avoid a potential infinite loop in constructors between BaseArrayT_t, Data, and std::any.
// template <typename Derived_, typename AbstractType_t, typename... Args_>
// BaseArrayT_t<Derived_> make_SyntacTupleT (AbstractType_t &&abstract_type, Args_&&... args) {
//     return BaseArrayT_t(std::move(abstract_type), make_SyntacTuple(std::forward<Args_>(args)...));
// }

template <typename Derived_>
Data const &abstract_type_of (BaseArrayT_t<Derived_> const &a) { return a.abstract_type(); }

// Runtime complexity O(size); Comparison compares abstract_type first, then uses lexicographical ordering (not shortlex order)
template <typename Derived_>
int compare (BaseArrayT_t<Derived_> const &lhs, BaseArrayT_t<Derived_> const &rhs) {
    auto c = compare_data(lhs.abstract_type(), rhs.abstract_type());
    if (c != 0)
        return c;
    return compare(static_cast<BaseArray_t<Derived_> const &>(lhs), static_cast<BaseArray_t<Derived_> const &>(rhs));
}

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <typename Derived_>
struct hash<sept::BaseArrayT_t<Derived_>> {
    size_t operator () (sept::BaseArrayT_t<Derived_> const &s) const {
        return sept::hash(typeid(typename sept::BaseArrayT_t<Derived_>::Derived), s.abstract_type(), s.elements());
    }
};

} // end namespace std
