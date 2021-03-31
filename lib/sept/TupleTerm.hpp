// 2021.03.27 - Victor Dods

#pragma once

#include <lvd/abort.hpp>
// #include <lvd/call_site.hpp> // TEMP
#include <lvd/fmt.hpp>
// #include <lvd/g_log.hpp> // TEMP
#include "sept/core.hpp"
#include "sept/BaseArray_t.hpp"

namespace sept {

// This class is almost identical to BaseArray_t<TupleTerm_c> itself.
class TupleTerm_c : public BaseArray_t<TupleTerm_c> {
public:

    using ParentClass = BaseArray_t<TupleTerm_c>;

    TupleTerm_c () = default;
    TupleTerm_c (TupleTerm_c const &other) = default;
    TupleTerm_c (TupleTerm_c &&other) = default;
    explicit TupleTerm_c (DataVector const &elements) : ParentClass(elements) { }
    explicit TupleTerm_c (DataVector &&elements) : ParentClass(std::move(elements)) { }
    template <typename... Args_>
    explicit TupleTerm_c (Args_&&... args)
        :   ParentClass(make_DataVector(std::forward<Args_>(args)...))
    { }

    TupleTerm_c &operator = (TupleTerm_c const &other) = default;
    TupleTerm_c &operator = (TupleTerm_c &&other) = default;

    // Construct an element-wise inhabitant.
    TupleTerm_c operator() (TupleTerm_c const &arguments) const {
        if (arguments.size() != this->size())
            throw std::runtime_error(LVD_FMT("invalid number of elements to construct inhabitant of this tuple; expected " << this->size() << " but got " << arguments.size()));

        DataVector retval_elements;
        retval_elements.reserve(this->size());
        for (size_t i = 0; i < this->size(); ++i) {
            auto const &element = this->elements()[i];
            auto const &argument = arguments.elements()[i];
            retval_elements.emplace_back(element(argument));
            if (!inhabits_data(retval_elements.back(), element))
                LVD_ABORT(LVD_FMT("Tuple element " << element << " constructed a non-inhabitant " << retval_elements.back() << " from argument " << argument));
        }
        return TupleTerm_c(std::move(retval_elements));
    }
    TupleTerm_c operator() () const {
        if (0 != this->size())
            throw std::runtime_error(LVD_FMT("invalid number of elements to construct inhabitant of this tuple; expected " << this->size() << " but got " << 0));

        return this->operator()(TupleTerm_c());
    }
    template <typename First_, typename... Rest_, typename = std::enable_if_t<!std::is_same_v<First_,TupleTerm_c> || sizeof...(Rest_) != 0>>
    TupleTerm_c operator() (First_ &&first, Rest_&&... rest) const {
        if (1+sizeof...(Rest_) != this->size())
            throw std::runtime_error(LVD_FMT("invalid number of elements to construct inhabitant of this tuple; expected " << this->size() << " but got " << 1+sizeof...(Rest_)));

        return this->operator()(TupleTerm_c(std::forward<First_>(first), std::forward<Rest_>(rest)...));
    }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "TupleTerm_c" << elements();
        });
    }
};

// This is used to construct TupleTerm_c more efficiently (std::initializer_list lacks move semantics for some dumb
// reason), as well as to avoid a potential infinite loop in constructors between TupleTerm_c, Data, and std::any.
template <typename... Args_>
TupleTerm_c make_tuple (Args_&&... args) {
    return TupleTerm_c(std::forward<Args_>(args)...);
}

// TODO: This should be abstract_type_of, and concrete_type_of should be what returns Tuple.
inline TupleTerm_c _tuple_type_of (TupleTerm_c const &t) {
    DataVector element_types;
    element_types.reserve(t.size());
    for (auto const &element : t.elements()) {
        element_types.emplace_back(abstract_type_of_data(element));
    }
    return TupleTerm_c(std::move(element_types));
}

// NOTE: Currently, serialization of parametric types uses the abstract type instead of the concrete type,
// and in this case, that would cause an infinite recursion.
inline TupleTerm_c abstract_type_of (TupleTerm_c const &t) { return _tuple_type_of(t); }
// inline constexpr Tuple_c const &abstract_type_of (TupleTerm_c const &) { return Tuple; }

// A TupleTerm_c can inhabit another TupleTerm_c if there is element-wise inhabitation,
// e.g. (Float(1.23), Uint32(400)) inhabits (Float, Uint32).
inline bool inhabits (TupleTerm_c const &value, TupleTerm_c const &type) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << '\n';
//     lvd::IndentGuard ig(lvd::g_log);
//     lvd::g_log << lvd::Log::trc()
//                << LVD_REFLECT(value) << '\n'
//                << LVD_REFLECT(type) << '\n'
//                << '\n';
    if (value.size() != type.size()) {
//         lvd::g_log << lvd::Log::trc() << "early-out false because value.size() != type.size()\n";
        return false;
    }
    for (size_t i = 0; i < value.size(); ++i) {
//         lvd::g_log << lvd::Log::trc() << "checking inhabitation for elements " << i << ":\n";
//         lvd::IndentGuard ig2(lvd::g_log);
//         lvd::g_log << lvd::Log::trc()
//                    << LVD_REFLECT(value[i]) << '\n'
//                    << LVD_REFLECT(type[i]) << '\n';
        if (!inhabits_data(value[i], type[i])) {
//             lvd::g_log << lvd::Log::trc() << "early-out false because !inhabits_data(value[i], type[i])\n";
            return false;
        } else {
//             lvd::g_log << lvd::Log::trc() << "inhabitation satisfied\n";
        }
    }
//     lvd::g_log << lvd::Log::trc() << "returned true\n";
    return true;
}

void serialize (TupleTerm_c const &v, std::ostream &out);

// This assumes that the abstract_type portion following the SerializedTopLevelCode::PARAMETRIC_TERM
// has already been read in; that value is passed in as abstract_type.
TupleTerm_c deserialize_value_TupleTerm (Data &&abstract_type, std::istream &in);

// is_member is provided by the one for BaseArray_t; see BaseArray_t.hpp
// compare is provided by the one for BaseArray_t; see BaseArray_t.hpp

} // end namespace sept

namespace std {

// Template specialization(s) to define std::hash<Key_> for the types defined in this header file.
// This is generally done by combining the hashes of the typeid of the argument and its content.

template <>
struct hash<sept::TupleTerm_c> {
    size_t operator () (sept::TupleTerm_c const &t) const {
        return std::hash<sept::TupleTerm_c::ParentClass>()(t);
    }
};

} // end namespace std
