// 2021.04.02 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/NPType.hpp"

namespace sept {

class FormalTypeOf_Term_c {
public:

    FormalTypeOf_Term_c () = default;
    FormalTypeOf_Term_c (FormalTypeOf_Term_c const &other) = default;
    FormalTypeOf_Term_c (FormalTypeOf_Term_c &&other) = default;
    explicit FormalTypeOf_Term_c (Data const &term) : m_term(term) { }
    explicit FormalTypeOf_Term_c (Data &&term) : m_term(std::move(term)) { }

    FormalTypeOf_Term_c &operator = (FormalTypeOf_Term_c const &other) = default;
    FormalTypeOf_Term_c &operator = (FormalTypeOf_Term_c &&other) = default;

    Data const &term () const { return m_term; }

    // It would appear that the canonical construct_inhabitant_of for FormalTypeOf(x) would
    // be to produce x itself, since there's precisely one inhabitant of FormalTypeOf(x).
    // Thus the argument passed in must be Void.
    Data const &operator() (Data const &argument) const {
        if (!eq_data(argument, Void))
            throw std::runtime_error(LVD_FMT("must pass Void as sole parameter to construction of inhabitant of " << *this << "; it has precisely one inhabitant, so there are no parameters needed for construction"));
        return m_term;
    }

    operator lvd::OstreamDelegate () const;

private:

    Data m_term;
};

inline bool operator== (FormalTypeOf_Term_c const &lhs, FormalTypeOf_Term_c const &rhs) {
    return lhs.term() == rhs.term();
}

inline void print (std::ostream &out, DataPrintCtx &ctx, FormalTypeOf_Term_c const &value) {
    out << "FormalTypeOf(";
    print_data(out, ctx, value.term());
    out << ')';
}

using FormalTypeOf_c = NonParametricType_t<NPTerm::FORMAL_TYPE_OF,FormalTypeOf_Term_c>;

extern FormalTypeOf_c FormalTypeOf;

inline constexpr NonParametricType_c const &abstract_type_of (FormalTypeOf_c const &) { return NonParametricType; }
inline FormalTypeOf_Term_c abstract_type_of (FormalTypeOf_Term_c const &f) {
    return FormalTypeOf(abstract_type_of_data(f.term()));
}

// E.g. FormalTypeOf(x) : FormalTypeOf(FormalTypeOf(x))
inline bool inhabits (FormalTypeOf_Term_c const &a, FormalTypeOf_c const &t) { return true; }
inline bool inhabits (Data const &value, FormalTypeOf_c const &t) {
    return value.can_cast<FormalTypeOf_Term_c>() && inhabits(value.cast<FormalTypeOf_Term_c const &>(), t);
}

inline bool inhabits (Data const &value, FormalTypeOf_Term_c const &t) {
    return eq_data(value, t.term());
}

} // end namespace sept

namespace std {

template <>
struct hash<sept::FormalTypeOf_c> {
    size_t operator () (sept::FormalTypeOf_c const &t) const {
        return lvd::hash(typeid(sept::FormalTypeOf_c));
    }
};

template <>
struct hash<sept::FormalTypeOf_Term_c> {
    size_t operator () (sept::FormalTypeOf_Term_c const &t) const {
        return lvd::hash(typeid(sept::FormalTypeOf_Term_c), t.term());
    }
};

} // end namespace std
