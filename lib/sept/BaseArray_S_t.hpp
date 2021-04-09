// 2020.07.25 - Victor Dods

#pragma once

#include <lvd/static_if.hpp>
#include "sept/core.hpp"
#include "sept/hash.hpp"
#include "sept/DataArray_t.hpp"

namespace sept {

// The S stands for "Sized", and the underscore preceding it denotes that the size parameter is a template parameter,
// meaning that the size is specified at compile time.  BaseArray_S_t is just an implementation detail to help
// implement other classes.
// TODO: Technically, this should use std::tuple instead of array, because the types could potentially be known.
template <size_t ELEMENT_COUNT_, typename Derived_ = DerivedNone>
class BaseArray_S_t {
public:

    using Derived = lvd::static_if_t<std::is_same_v<Derived_,DerivedNone>,BaseArray_S_t,Derived_>;
    using DataArray = DataArray_t<ELEMENT_COUNT_>;

    BaseArray_S_t () = delete;
    template <typename Arg_, typename = std::enable_if_t<ELEMENT_COUNT_ == 1>>
    BaseArray_S_t (Arg_ &&arg)
        :   m_elements(make_DataArray_t<ELEMENT_COUNT_>(std::move(arg)))
    {
        for (auto const &element : m_elements)
            if (!element.has_value())
                throw std::runtime_error("all elements in BaseArray_S_t must have values");
    }
    template <typename... Args_, typename = std::enable_if_t<sizeof...(Args_) == ELEMENT_COUNT_ && (ELEMENT_COUNT_ > 1)>>
    BaseArray_S_t (Args_&&... args)
        :   m_elements(make_DataArray_t<ELEMENT_COUNT_>(std::forward<Args_>(args)...))
    {
        for (auto const &element : m_elements)
            if (!element.has_value())
                throw std::runtime_error("all elements in BaseArray_S_t must have values");
    }
    BaseArray_S_t (DataArray const &elements)
        :   m_elements(elements)
    {
        for (auto const &element : m_elements)
            if (!element.has_value())
                throw std::runtime_error("all elements in BaseArray_S_t must have values");
    }
    BaseArray_S_t (DataArray &&elements)
        :   m_elements(std::move(elements))
    {
        for (auto const &element : m_elements)
            if (!element.has_value())
                throw std::runtime_error("all elements in BaseArray_S_t must have values");
    }
    BaseArray_S_t (BaseArray_S_t const &other) = default;
    // This is necessary to prevent the variadic template constructor from being called in this case.
    BaseArray_S_t (BaseArray_S_t &other) : BaseArray_S_t(static_cast<BaseArray_S_t const &>(other)) { }
    BaseArray_S_t (BaseArray_S_t &&other) = default;

    BaseArray_S_t &operator = (BaseArray_S_t const &other) = default;
    BaseArray_S_t &operator = (BaseArray_S_t &&other) = default;

    bool operator == (Data const &other) const {
        return (other.type() == typeid(Derived)) && this->operator==(other.cast<Derived const &>());
    }
    bool operator == (BaseArray_S_t const &other) const { return m_elements == other.m_elements; }
    bool operator != (BaseArray_S_t const &other) const { return m_elements != other.m_elements; }

    DataArray const &elements () const & { return m_elements; }
    DataArray &elements () & { return m_elements; }
    DataArray elements () && { return std::move(m_elements); }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "BaseArray_S_t<" << ELEMENT_COUNT_ << ',' << typeid(Derived).name() << '>' << elements();
        });
    }

private:

    DataArray m_elements;
};

// This is used to construct BaseArray_S_t more efficiently (std::initializer_list lacks move semantics for some dumb
// reason), as well as to avoid a potential infinite loop in constructors between BaseArray_S_t, Data, and std::any.
template <size_t ELEMENT_COUNT_, typename Derived_, typename... Args_>
BaseArray_S_t<ELEMENT_COUNT_,Derived_> make_SyntacTuple_S_t (Args_&&... args) {
    return BaseArray_S_t<ELEMENT_COUNT_,Derived_>(std::forward<Args_>(args)...);
}

// Runtime complexity O(size); Comparison compares abstract_type first, then uses lexicographical ordering (not shortlex order)
template <size_t ELEMENT_COUNT_, typename Derived_>
int compare (BaseArray_S_t<ELEMENT_COUNT_,Derived_> const &lhs, BaseArray_S_t<ELEMENT_COUNT_,Derived_> const &rhs) { return compare(lhs.elements(), rhs.elements()); }

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <size_t ELEMENT_COUNT_, typename Derived_>
struct hash<sept::BaseArray_S_t<ELEMENT_COUNT_,Derived_>> {
    size_t operator () (sept::BaseArray_S_t<ELEMENT_COUNT_,Derived_> const &s) const {
        return sept::hash(typeid(typename sept::BaseArray_S_t<ELEMENT_COUNT_,Derived_>::Derived), s.elements());
    }
};

} // end namespace std
