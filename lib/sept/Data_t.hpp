// 2020.04.05 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/Data.hpp"

namespace sept {

// This is a version of Data which already "knows" the type it's populated with.  The trick is
// that the run-time type check is done before this type is even constructed.
template <typename T_>
class Data_t : public Data {
public:

    // TODO: Add all the enable_if stuff
    // TODO: Also add runtime checks that the underlying type is what was expected.

    // These are the same constructors Data has.

    // It should be impossible to create a Data_t without a value.
    Data_t () = delete;
    Data_t (Data_t const &other) : Data(static_cast<Data const &>(other)) { }
    Data_t (Data_t &&other) noexcept : Data(std::move(static_cast<Data &&>(other))) { }

    template <
        typename ValueType_,
        typename U_ = std::decay_t<ValueType_>,
        typename = std::enable_if_t<
            !std::is_base_of_v<Data_t,U_> && // TODO: Change to "is subclass of Data_t"
            !is_in_place_type_v<ValueType_> &&
            std::is_copy_constructible_v<U_>
        >
    >
    Data_t (ValueType_ &&value)
    :   Data(std::forward<ValueType_>(value))
    {
        // This will throw std::bad_any_cast if the constructed type is wrong.
        std::any_cast<T_ const &>(static_cast<std::any const &>(*this));
    }

    template <
        typename ValueType_,
        typename... Args_,
        typename U_ = std::decay_t<ValueType_>,
        typename = std::enable_if_t<
            std::is_constructible_v<U_,Args_...> &&
            std::is_copy_constructible_v<U_> // This one doesn't make sense, it should allow move-construction too.
        >
    >
    explicit Data_t (std::in_place_type_t<ValueType_>, Args_&&... args)
    :   Data(std::in_place_type_t<ValueType_>(), std::forward<Args_>(args)...)
    {
        // This will throw std::bad_any_cast if the constructed type is wrong.
        std::any_cast<T_ const &>(static_cast<std::any const &>(*this));
    }

    template <
        typename ValueType_,
        typename I_,
        typename... Args_,
        typename U_ = std::decay_t<ValueType_>,
        typename = std::enable_if_t<
            std::is_constructible_v<U_,std::initializer_list<I_>&, Args_...> &&
            std::is_copy_constructible_v<U_> // Also not 100% sure about this one.
        >
    >
    explicit Data_t (std::in_place_type_t<ValueType_>, std::initializer_list<I_> il, Args_&&... args)
    :   Data(std::in_place_type_t<ValueType_>(), il, std::forward<Args_>(args)...)
    {
        // This will throw std::bad_any_cast if the constructed type is wrong.
        std::any_cast<T_ const &>(static_cast<std::any const &>(*this));
    }

    // These are the same operator= overloads as Data has.
    Data_t &operator = (Data_t const &other) {
        Data::operator=(static_cast<Data const &>(other));
        return *this;
    }
    Data_t &operator = (Data_t &&other) noexcept {
        Data::operator=(std::move(static_cast<Data &&>(other)));
        return *this;
    }
    template <
        typename ValueType_,
        typename U_ = std::decay_t<ValueType_>,
        typename = std::enable_if_t<
            !std::is_base_of_v<Data_t,U_> && // TODO: Change to "is subclass of Data_t"
            std::is_copy_constructible_v<U_>
        >
    >
    Data_t &operator = (ValueType_ &&value) {
        Data::operator=(std::move(value));
        // This will throw std::bad_any_cast if the constructed type is wrong.
        std::any_cast<T_ const &>(static_cast<std::any const &>(*this));
        return *this;
    }

    // TODO: Figure out how to avoid the std::any_cast, because by construction, Data_t satisfies that cast.
    T_ const &value () const & { return std::any_cast<T_ const &>(*this); }
    // TODO: Figure out how to avoid the std::any_cast, because by construction, Data_t satisfies that cast.
    T_ &value () & { return std::any_cast<T_ &>(*this); }
    // TODO: Figure out how to avoid the std::any_cast, because by construction, Data_t satisfies that cast.
    // Not 100% sure about this one.
    T_ &&value () && { return std::any_cast<T_ &&>(*this); }

    // Convenient way to get an overload for operator<< on std::ostream.
    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            // TODO: Use name demangling, or implement own type printing.
//             out << "Data_t<" << typeid(T_).name() << ">(" << value() << ')';
            out << "Data_t(" << value() << ')';
        });
    }
};

// This is the same as make_data, except that it produces the "known type" version of Data, i.e. Data_t<T_>.
template <typename T_, typename... Args_>
Data_t<T_> make_data_t (Args_&&... args) {
    return Data_t<T_>(std::in_place_type_t<T_>(), std::forward<Args_>(args)...);
}

template <typename T_>
inline bool operator == (Data_t<T_> const &lhs, Data_t<T_> const &rhs) {
    return lhs.value() == rhs.value();
}

template <typename T_>
inline bool operator != (Data_t<T_> const &lhs, Data_t<T_> const &rhs) {
    return lhs.value() != rhs.value();
}

} // end namespace sept

namespace std {

// Overload to define std::swap for sept::Data_t.
template <typename T_>
void swap (sept::Data_t<T_> &lhs, sept::Data_t<T_> &rhs) noexcept {
    swap(static_cast<any&>(lhs), static_cast<any&>(rhs));
}

} // end namespace std
