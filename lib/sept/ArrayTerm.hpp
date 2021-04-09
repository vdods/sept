// 2020.03.13 - Victor Dods

#pragma once

#include <lvd/OstreamDelegate.hpp>
#include "sept/ArrayType.hpp"
#include "sept/core.hpp"
#include "sept/BaseArrayT_t.hpp"
#include "sept/Data.hpp"
#include "sept/DataVector.hpp"
#include "sept/hash.hpp"
#include <vector>

namespace sept {

class ArrayTerm_c : public BaseArrayT_t<ArrayTerm_c> {
public:

    using ParentClass = BaseArrayT_t<ArrayTerm_c>;

    ArrayTerm_c ()
        :   ParentClass(Array_c())
    {
        // ArrayConstraint is already satisfied by construction.
    }
    ArrayTerm_c (ArrayTerm_c const &other)
        :   ParentClass(other.abstract_type(), static_cast<ParentClass const &>(other))
    {
        // ArrayConstraint is already satisfied by construction.
    }
    // This is necessary to prevent the variadic template constructor from being called in this case.
    ArrayTerm_c (ArrayTerm_c &other)
        :   ArrayTerm_c(static_cast<ArrayTerm_c const &>(other))
    { }
    ArrayTerm_c (ArrayTerm_c &&other)
        :   ParentClass(std::move(other).abstract_type(), std::move(static_cast<ParentClass&&>(other)))
    {
        // ArrayConstraint is already satisfied by construction.
    }
    template <typename... Args_>
    explicit ArrayTerm_c (Args_&&... args)
        :   ParentClass(Array_c(), std::forward<Args_>(args)...)
    {
        // ArrayConstraint is already satisfied by construction.
    }

    ArrayTerm_c &operator = (ArrayTerm_c const &other) = default;
    ArrayTerm_c &operator = (ArrayTerm_c &&other) = default;

    Data element_type () const { return element_type_of(abstract_type()); }

    // This makes a copy.
    template <typename Constraint_, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Constraint_>,Data>>>
    ArrayTerm_c with_constraint (Constraint_ &&constraint) const & {
        constraint.verify_constraint_or_throw(elements());
        ArrayTerm_c retval(elements());
        retval.abstract_type() = std::forward<Constraint_>(constraint);
        return retval;
    }
    // This uses move semantics.
    template <typename Constraint_, typename = std::enable_if_t<!std::is_same_v<std::decay_t<Constraint_>,Data>>>
    ArrayTerm_c with_constraint (Constraint_ &&constraint) && {
        constraint.verify_constraint_or_throw(elements());
        abstract_type() = std::forward<Constraint_>(constraint);
        return std::move(*this);
    }
    ArrayTerm_c with_constraint (Data const &constraint) const & {
        if (constraint.type() == typeid(ArrayESTerm_c)) return with_constraint(constraint.cast<ArrayESTerm_c const &>());
        else if (constraint.type() == typeid(ArrayETerm_c)) return with_constraint(constraint.cast<ArrayETerm_c const &>());
        else if (constraint.type() == typeid(ArraySTerm_c)) return with_constraint(constraint.cast<ArraySTerm_c const &>());
        else if (constraint.type() == typeid(Array_c)) return *this;
        else throw std::runtime_error(LVD_FMT("invalid constraint for ArrayTerm_c: " << constraint));
    }
    ArrayTerm_c with_constraint (Data &&constraint) && {
        if (constraint.type() == typeid(ArrayESTerm_c)) return with_constraint(std::move(constraint.cast<ArrayESTerm_c&>()));
        else if (constraint.type() == typeid(ArrayETerm_c)) return with_constraint(std::move(constraint.cast<ArrayETerm_c&>()));
        else if (constraint.type() == typeid(ArraySTerm_c)) return with_constraint(std::move(constraint.cast<ArraySTerm_c&>()));
        else if (constraint.type() == typeid(Array_c)) return std::move(*this);
        else throw std::runtime_error(LVD_FMT("invalid constraint for ArrayTerm_c: " << constraint));
    }

    // This makes a copy.
    ArrayTerm_c without_constraint () const & {
        return ArrayTerm_c(elements());
    }
    // This uses move semantics.
    ArrayTerm_c without_constraint () && {
        abstract_type() = Array_c();
        return std::move(*this);
    }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "ArrayTerm_c<" << element_type() << ',' << size() << '>' << elements();
        });
    }

private:

    void verify_constraint_or_throw () const {
        if (abstract_type().type() == typeid(ArrayESTerm_c))
            abstract_type().cast<ArrayESTerm_c const &>().verify_constraint_or_throw(elements());
        else if (abstract_type().type() == typeid(ArrayETerm_c))
            abstract_type().cast<ArrayETerm_c const &>().verify_constraint_or_throw(elements());
        else if (abstract_type().type() == typeid(ArraySTerm_c))
            abstract_type().cast<ArraySTerm_c const &>().verify_constraint_or_throw(elements());
        else if (abstract_type().type() == typeid(Array_c))
            abstract_type().cast<Array_c const &>().verify_constraint_or_throw(elements());
        else
            throw std::runtime_error(LVD_FMT("unhandled type: " << abstract_type().type()));
    }
};

// This is used to construct ArrayTerm_c more efficiently (std::initializer_list lacks move semantics for some dumb
// reason), as well as to avoid a potential infinite loop in constructors between ArrayTerm_c, Data, and std::any.
template <typename... Args_>
ArrayTerm_c make_array (Args_&&... args) {
    return ArrayTerm_c(make_DataVector(std::forward<Args_>(args)...));
}

void serialize (ArrayTerm_c const &v, std::ostream &out);

// This assumes that the abstract_type portion following the SerializedTopLevelCode::PARAMETRIC_TERM
// has already been read in; that value is passed in as abstract_type.
ArrayTerm_c deserialize_value_ArrayTerm (Data &&abstract_type, std::istream &in);

// inline constexpr Data const &abstract_type_of (ArrayTerm_c const &a) { return a.constraint().array_type(); }

// is_member is provided by the one for BaseArrayT_t; see BaseArrayT_t.hpp
// compare is provided by the one for BaseArrayT_t; see BaseArrayT_t.hpp

template <typename... Args_>
ArrayTerm_c ArrayESTerm_c::operator () (Args_&&... args) const {
    return make_array(std::forward<Args_>(args)...).with_constraint(*this);
}

template <typename... Args_>
ArrayTerm_c ArrayETerm_c::operator () (Args_&&... args) const {
    return make_array(std::forward<Args_>(args)...).with_constraint(*this);
}

template <typename... Args_>
ArrayTerm_c ArraySTerm_c::operator () (Args_&&... args) const {
    return make_array(std::forward<Args_>(args)...).with_constraint(*this);
}

template <typename... Args_>
ArrayTerm_c Array_c::operator () (Args_&&... args) const {
    return make_array(std::forward<Args_>(args)...);
}

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::ArrayTerm_c> {
    size_t operator () (sept::ArrayTerm_c const &a) const {
        return std::hash<sept::ArrayTerm_c::ParentClass>()(a);
    }
};

} // end namespace std
