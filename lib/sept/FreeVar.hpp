// 2021.05.16 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"
#include "sept/BaseArrayT_t.hpp"
#include "sept/BaseArray_S_t.hpp"

namespace sept {

// FreeVarType has one inhabitant, which is FreeVar.
// FreeVar has instances FreeVar(X) for any Data X.  For now, it will be used where X is std::string("...").
// FreeVar(X) (which is instantiated as FreeVarTerm_c{X}) is equivalent to Term in terms of inhabitation
// (i.e. everything inhabits FreeVar(X) for any X).

class FreeVarTerm_c : public BaseArray_S_t<1,FreeVarTerm_c> {
public:

    explicit FreeVarTerm_c (Data const &free_var_id) : BaseArray_S_t<1,FreeVarTerm_c>(free_var_id) { }
    explicit FreeVarTerm_c (Data &&free_var_id) : BaseArray_S_t<1,FreeVarTerm_c>(std::move(free_var_id)) { }

    // Constructs a Term of type FreeVar(free_var_id), which is functionally equivalent to Term,
    // i.e. it's an abstract constructor.
    template <typename T_>
    T_ operator () (T_ const &t) const {
        return t;
    }
    // Constructs a Term of type FreeVar(free_var_id), which is functionally equivalent to Term,
    // i.e. it's an abstract constructor.
    template <typename T_>
    T_ operator () (T_ &&t) const {
        return std::move(t);
    }

    Data const &free_var_id () const { return elements()[0]; }
    // TEMP: Convenience method to get the free_var_id as std::string (this assumes that the cast works)
    std::string const &free_var_id__as_string () const { return free_var_id().cast<std::string const &>(); }

    operator lvd::OstreamDelegate () const;
};

inline void print (std::ostream &out, DataPrintCtx &ctx, FreeVarTerm_c const &value) {
    out << "FreeVar(";
    print_data(out, ctx, value.free_var_id());
    out << ')';
}

using FreeVarType_c = NonParametricType_t<NPTerm::FREEVAR_TYPE>;

class FreeVar_c : public NonParametricType_t<NPTerm::FREEVAR, FreeVarTerm_c> {
public:

    // Constructs FreeVarTerm_c (abtractly, a Term of type FreeVar).
    FreeVarTerm_c operator () (Data const &x) const {
        return FreeVarTerm_c{x};
    }
    FreeVarTerm_c operator () (std::string const &s) const {
        return FreeVarTerm_c{Data{s}};
    }
    FreeVarTerm_c operator () (char const *s) const {
        return FreeVarTerm_c{Data{std::string{s}}};
    }
};

extern FreeVarType_c FreeVarType;
extern FreeVar_c FreeVar;

inline constexpr NonParametricType_c const &abstract_type_of (FreeVarType_c const &) { return NonParametricType; }
inline constexpr FreeVarType_c const &abstract_type_of (FreeVar_c const &) { return FreeVarType; }
inline constexpr FreeVar_c const &abstract_type_of (FreeVarTerm_c const &) { return FreeVar; }

inline bool inhabits (FreeVarTerm_c const &a, FreeVar_c const &t) { return true; }
inline bool inhabits (Data_t<FreeVarTerm_c> const &m, FreeVar_c const &t) { return true; }
inline bool inhabits (Data const &value, FreeVar_c const &t) { return value.type() == typeid(FreeVarTerm_c); }
// Everything inhabits FreeVar("X") (i.e. FreeVarTerm_c("X")) for any string "X".
inline bool constexpr inhabits (Data const &value, FreeVarTerm_c const &t) { return true; }

// TODO: Serialization (NonParametricType_t should take care of this)

FreeVarType_c deserialize_value_FreeVarType (Data &&abstract_type, std::istream &in);
FreeVar_c deserialize_value_FreeVar (Data &&abstract_type, std::istream &in);
FreeVarTerm_c deserialize_value_FreeVarTerm (Data &&abstract_type, std::istream &in);

} // end namespace sept

namespace std {

template <>
struct hash<sept::FreeVarType_c> {
    size_t operator () (sept::FreeVarType_c const &t) const {
        return lvd::hash(typeid(sept::FreeVarType_c));
    }
};

template <>
struct hash<sept::FreeVar_c> {
    size_t operator () (sept::FreeVar_c const &t) const {
        return lvd::hash(typeid(sept::FreeVar_c));
    }
};

template <>
struct hash<sept::FreeVarTerm_c> {
    size_t operator () (sept::FreeVarTerm_c const &t) const {
        return lvd::hash(typeid(sept::FreeVarTerm_c), t.free_var_id());
    }
};

} // end namespace std
