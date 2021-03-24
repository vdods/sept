// 2020.07.25 - Victor Dods

#pragma once

#include <cassert>
#include <lvd/OstreamDelegate.hpp>
#include <lvd/static_if.hpp>
#include "sept/core.hpp"
#include "sept/DataVector.hpp"
#include "sept/hash.hpp"
#include <vector>

namespace sept {

// This is just an implementation detail to help implement other classes.
// The idea is to provide basic DataVector functionality that can be used by derived classes.
template <typename Derived_ = DerivedNone>
class BaseArray_t {
public:

    using Derived = lvd::static_if_t<std::is_same_v<Derived_,DerivedNone>,BaseArray_t,Derived_>;

    BaseArray_t () : m_elements() { }
    BaseArray_t (BaseArray_t const &other) : m_elements(other.m_elements) { }
    BaseArray_t (BaseArray_t &&other) : m_elements(std::move(other.m_elements)) { }
    explicit BaseArray_t (DataVector const &elements) : m_elements(elements) { }
    explicit BaseArray_t (DataVector &&elements) : m_elements(std::move(elements)) { }
    template <typename T_>
    explicit BaseArray_t (std::vector<T_> const &elements) : m_elements(elements) { }
    template <typename T_>
    explicit BaseArray_t (std::vector<T_> &&elements)
    :   m_elements() {
        m_elements.reserve(elements.size());
        for (auto &&element : elements)
            m_elements.emplace_back(std::move(element));
    }

    BaseArray_t &operator = (BaseArray_t const &other) = default;
    BaseArray_t &operator = (BaseArray_t &&other) = default;

    bool operator == (Data const &other) const {
        return (other.type() == typeid(Derived)) && this->operator==(other.cast<Derived const &>());
    }
    // NOTE: This uses a useful, but weak notion of equality that compares only the elements.
    bool operator == (BaseArray_t const &other) const { return compare(m_elements, other.m_elements) == 0; }
    bool operator != (BaseArray_t const &other) const { return !(*this == other); }

    Derived const & as_derived () const & { return static_cast<Derived const &>(*this); }
    Derived &as_derived () & { return static_cast<Derived &>(*this); }
    operator Derived const & () const & { return as_derived(); }
    operator Derived & () & { return as_derived(); }

    size_t size () const { return m_elements.size(); }
    DataVector const &elements () const & { return m_elements; }
    DataVector &elements () & { return m_elements; }

    Data const &operator [] (size_t i) const { return m_elements.at(i); }
    Data &operator [] (size_t i) { return m_elements.at(i); }

    // TODO: Make a Data-accepting version of operator[]

    DataVector::const_iterator cbegin () const { return m_elements.cbegin(); }
    DataVector::const_iterator cend () const { return m_elements.cend(); }

    DataVector::const_iterator begin () const { return m_elements.begin(); }
    DataVector::const_iterator end () const { return m_elements.end(); }

    DataVector::iterator begin () { return m_elements.begin(); }
    DataVector::iterator end () { return m_elements.end(); }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "BaseArray_t" << elements();
        });
    }

private:

    // Ideally we would have an vector_any, where there is a single type specifier for all elements, instead
    // of having one per element as is in the case of DataVector (noting that Data derives from std::any)
    DataVector m_elements;
};

// This is used to construct BaseArray_t more efficiently (std::initializer_list lacks move semantics for some dumb
// reason), as well as to avoid a potential infinite loop in constructors between SyntacTupleT, Data, and std::any.
template <typename... Args_>
BaseArray_t<> make_SyntacTuple_t (Args_&&... args) {
    return BaseArray_t<>(make_DataVector(std::forward<Args_>(args)...));
}

// Runtime complexity: O(size)
template <typename Derived_>
bool is_member (Data const &value, BaseArray_t<Derived_> const &container) { return is_member(value, container.elements()); }

// Runtime complexity O(size); lexicographical ordering (not shortlex order)
template <typename Derived_>
int compare (BaseArray_t<Derived_> const &lhs, BaseArray_t<Derived_> const &rhs) { return compare(lhs.elements(), rhs.elements()); }

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <typename Derived_>
struct hash<sept::BaseArray_t<Derived_>> {
    size_t operator () (sept::BaseArray_t<Derived_> const &s) const {
        return sept::hash(typeid(typename sept::BaseArray_t<Derived_>::Derived), s.elements());
    }
};

} // end namespace std
