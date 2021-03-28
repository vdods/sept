// 2020.04.12 - Victor Dods

#pragma once

#include "sept/core.hpp"
#include "sept/hash.hpp"
#include "sept/NPTerm.hpp"

namespace sept {

// This is a sentinel value for use in the template parameters of NonParametricType_t to indicate that
// no term instantiator should be made.
struct TermNone {
    TermNone () = delete;
};

template <NPTerm ENUM_VALUE_, typename Term_ = TermNone, typename Derived_ = DerivedNone>
class NonParametricType_t : public NonParametricTerm_t<ENUM_VALUE_,lvd::static_if_t<std::is_same_v<Derived_,DerivedNone>,NonParametricType_t<ENUM_VALUE_,Term_>,Derived_>> {
public:

    // NOTE: Term_ being TermNone probably corresponds with this representing an abstract type
    // instead of having concrete inhabitants.

    template <typename... Args_, typename = std::enable_if<!std::is_same_v<Term_,TermNone>,Term_>>
    Term_ operator () (Args_&&... args) const {
        static_assert(!std::is_same_v<Term_,TermNone>, "This type can't instantiate a Term");
        // TODO: Add type checking
        return Term_(std::forward<Args_>(args)...);
    }

    // TODO: Should there be a check as to if this type is an abstract type?  Or just let the
    // inhabitation check below suffice?
//     template <typename T_>
//     AbstractTerm_c operator [] (T_ &&term) const {
//         // TODO: Add type checking
//         return AbstractTerm(SINGLETON, std::forward<T_>(term));
//     }
};

template <NPTerm ENUM_VALUE_, typename Term_, typename Derived_>
constexpr NonParametricTerm_c const &concrete_type_of (NonParametricType_t<ENUM_VALUE_,Term_,Derived_> const &) {
    return NonParametricTerm;
}

//
// Definitions of *_c classes using NonParametricType_t
//

using Term_c = NonParametricType_t<NPTerm::TERM>;
using Type_c = NonParametricType_t<NPTerm::TYPE,TermNone>;
using NonType_c = NonParametricType_t<NPTerm::NON_TYPE>;
using NonParametricType_c = NonParametricType_t<NPTerm::NON_PARAMETRIC_TYPE,TermNone>;
using VoidType_c = NonParametricType_t<NPTerm::VOID_TYPE,Void_c>;
using TrueType_c = NonParametricType_t<NPTerm::TRUE_TYPE,True_c>;
using FalseType_c = NonParametricType_t<NPTerm::FALSE_TYPE,False_c>;
using EmptyType_c = NonParametricType_t<NPTerm::EMPTY_TYPE>;
using Bool_c = NonParametricType_t<NPTerm::BOOL,BoolTerm_c>;
using Sint8_c = NonParametricType_t<NPTerm::SINT8,int8_t>; // Maybe make Sint8Term
using Sint16_c = NonParametricType_t<NPTerm::SINT16,int16_t>; // Maybe make Sint16Term
using Sint32_c = NonParametricType_t<NPTerm::SINT32,int32_t>; // Maybe make Sint32Term
using Sint64_c = NonParametricType_t<NPTerm::SINT64,int64_t>; // Maybe make Sint64Term
using Uint8_c = NonParametricType_t<NPTerm::UINT8,uint8_t>; // Maybe make Uint8Term
using Uint16_c = NonParametricType_t<NPTerm::UINT16,uint16_t>; // Maybe make Uint16Term
using Uint32_c = NonParametricType_t<NPTerm::UINT32,uint32_t>; // Maybe make Uint32Term
using Uint64_c = NonParametricType_t<NPTerm::UINT64,uint64_t>; // Maybe make Uint64Term
using Float32_c = NonParametricType_t<NPTerm::FLOAT32,float>; // Maybe make Float32Term
using Float64_c = NonParametricType_t<NPTerm::FLOAT64,double>; // Maybe make Float64Term
using BoolType_c = NonParametricType_t<NPTerm::BOOL_TYPE,Bool_c>;
using Sint8Type_c = NonParametricType_t<NPTerm::SINT8_TYPE,Sint8_c>;
using Sint16Type_c = NonParametricType_t<NPTerm::SINT16_TYPE,Sint16_c>;
using Sint32Type_c = NonParametricType_t<NPTerm::SINT32_TYPE,Sint32_c>;
using Sint64Type_c = NonParametricType_t<NPTerm::SINT64_TYPE,Sint64_c>;
using Uint8Type_c = NonParametricType_t<NPTerm::UINT8_TYPE,Uint8_c>;
using Uint16Type_c = NonParametricType_t<NPTerm::UINT16_TYPE,Uint16_c>;
using Uint32Type_c = NonParametricType_t<NPTerm::UINT32_TYPE,Uint32_c>;
using Uint64Type_c = NonParametricType_t<NPTerm::UINT64_TYPE,Uint64_c>;
using Float32Type_c = NonParametricType_t<NPTerm::FLOAT32_TYPE,Float32_c>;
using Float64Type_c = NonParametricType_t<NPTerm::FLOAT64_TYPE,Float64_c>;

//
// Definitions of the "actual" NonParametricType term objects.
// These are what will be used syntactically.
//

extern Term_c Term;
extern Type_c Type;
extern NonType_c NonType;
extern NonParametricType_c NonParametricType;
extern EmptyType_c EmptyType;
extern VoidType_c VoidType;
extern TrueType_c TrueType;
extern FalseType_c FalseType;
extern Bool_c Bool;
extern Sint8_c Sint8;
extern Sint16_c Sint16;
extern Sint32_c Sint32;
extern Sint64_c Sint64;
extern Uint8_c Uint8;
extern Uint16_c Uint16;
extern Uint32_c Uint32;
extern Uint64_c Uint64;
extern Float32_c Float32;
extern Float64_c Float64;
extern BoolType_c BoolType;
extern Sint8Type_c Sint8Type;
extern Sint16Type_c Sint16Type;
extern Sint32Type_c Sint32Type;
extern Sint64Type_c Sint64Type;
extern Uint8Type_c Uint8Type;
extern Uint16Type_c Uint16Type;
extern Uint32Type_c Uint32Type;
extern Uint64Type_c Uint64Type;
extern Float32Type_c Float32Type;
extern Float64Type_c Float64Type;

//
// Definitions of abstract_type_of
//

inline constexpr NonParametricType_c const &abstract_type_of (Term_c const &) { return NonParametricType; }
inline constexpr NonParametricType_c const &abstract_type_of (NonParametricType_c const &) { return NonParametricType; }
inline constexpr NonParametricType_c const &abstract_type_of (Type_c const &) { return NonParametricType; }
inline constexpr NonParametricType_c const &abstract_type_of (NonType_c const &) { return NonParametricType; }
inline constexpr NonParametricType_c const &abstract_type_of (EmptyType_c const &) { return NonParametricType; }
inline constexpr NonParametricType_c const &abstract_type_of (VoidType_c const &) { return NonParametricType; }
inline constexpr NonParametricType_c const &abstract_type_of (TrueType_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (FalseType_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (BoolType_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Sint8Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Sint16Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Sint32Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Sint64Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Uint8Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Uint16Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Uint32Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Uint64Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Float32Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?
inline constexpr NonParametricType_c const &abstract_type_of (Float64Type_c const &) { return NonParametricType; } // TODO: Should this be PODType?

//
// Finishing the definition of Void_c and BoolTerm_c.  Ideally, these would be in NPTerm.hpp,
// but they had to wait for the existence of the various Types.
//

inline constexpr Bool_c const &concrete_type_of (BoolTerm_c const &) { return Bool; }
inline constexpr Sint8_c const &concrete_type_of (int8_t) { return Sint8; }
inline constexpr Sint16_c const &concrete_type_of (int16_t) { return Sint16; }
inline constexpr Sint32_c const &concrete_type_of (int32_t) { return Sint32; }
inline constexpr Sint64_c const &concrete_type_of (int64_t) { return Sint64; }
inline constexpr Uint8_c const &concrete_type_of (uint8_t) { return Uint8; }
inline constexpr Uint16_c const &concrete_type_of (uint16_t) { return Uint16; }
inline constexpr Uint32_c const &concrete_type_of (uint32_t) { return Uint32; }
inline constexpr Uint64_c const &concrete_type_of (uint64_t) { return Uint64; }
inline constexpr Float32_c const &concrete_type_of (float) { return Float32; }
inline constexpr Float64_c const &concrete_type_of (double) { return Float64; }

inline constexpr NonParametricType_c const &abstract_type_of (NonParametricTerm_c const &) { return NonParametricType; }
inline constexpr VoidType_c const &abstract_type_of (Void_c const &) { return VoidType; }
inline constexpr TrueType_c const &abstract_type_of (True_c const &) { return TrueType; }
inline constexpr FalseType_c const &abstract_type_of (False_c const &) { return FalseType; }
inline constexpr Bool_c const &abstract_type_of (BoolTerm_c const &) { return Bool; }
inline constexpr Bool_c const &abstract_type_of (bool) { return Bool; }
inline constexpr Sint8_c const &abstract_type_of (int8_t) { return Sint8; }
inline constexpr Sint16_c const &abstract_type_of (int16_t) { return Sint16; }
inline constexpr Sint32_c const &abstract_type_of (int32_t) { return Sint32; }
inline constexpr Sint64_c const &abstract_type_of (int64_t) { return Sint64; }
inline constexpr Uint8_c const &abstract_type_of (uint8_t) { return Uint8; }
inline constexpr Uint16_c const &abstract_type_of (uint16_t) { return Uint16; }
inline constexpr Uint32_c const &abstract_type_of (uint32_t) { return Uint32; }
inline constexpr Uint64_c const &abstract_type_of (uint64_t) { return Uint64; }
inline constexpr Float32_c const &abstract_type_of (float) { return Float32; }
inline constexpr Float64_c const &abstract_type_of (double) { return Float64; }
inline constexpr BoolType_c const &abstract_type_of (Bool_c const &) { return BoolType; }
inline constexpr Sint8Type_c const &abstract_type_of (Sint8_c const &) { return Sint8Type; }
inline constexpr Sint16Type_c const &abstract_type_of (Sint16_c const &) { return Sint16Type; }
inline constexpr Sint32Type_c const &abstract_type_of (Sint32_c const &) { return Sint32Type; }
inline constexpr Sint64Type_c const &abstract_type_of (Sint64_c const &) { return Sint64Type; }
inline constexpr Uint8Type_c const &abstract_type_of (Uint8_c const &) { return Uint8Type; }
inline constexpr Uint16Type_c const &abstract_type_of (Uint16_c const &) { return Uint16Type; }
inline constexpr Uint32Type_c const &abstract_type_of (Uint32_c const &) { return Uint32Type; }
inline constexpr Uint64Type_c const &abstract_type_of (Uint64_c const &) { return Uint64Type; }
inline constexpr Float32Type_c const &abstract_type_of (Float32_c const &) { return Float32Type; }
inline constexpr Float64Type_c const &abstract_type_of (Float64_c const &) { return Float64Type; }

//
// inhabits definitions
//

// Everything is a Term.
template <typename T_>
inline constexpr True_c const &inhabits (T_ const &, Term_c const &) { return True; }

inline constexpr True_c const &inhabits (Void_c const &, VoidType_c const &) { return True; }
// TODO: In the future, derived "inhabits" relationships (where x : T <: U, i.e. T is a subtype of U)
// should/could be computed using the poset of subtype relationships.
inline constexpr True_c const &inhabits (Void_c const &, NonParametricTerm_c const &) { return True; }
inline constexpr True_c const &inhabits (Void_c const &, NonType_c const &) { return True; }

inline constexpr True_c const &inhabits (VoidType_c const &, NonParametricTerm_c const &) { return True; }
inline constexpr True_c const &inhabits (VoidType_c const &, Type_c const &) { return True; }
inline constexpr True_c const &inhabits (VoidType_c const &, NonParametricType_c const &) { return True; }

inline constexpr True_c const &inhabits (True_c const &, TrueType_c const &) { return True; }
inline constexpr True_c const &inhabits (False_c const &, FalseType_c const &) { return True; }
inline constexpr True_c const &inhabits (True_c const &, Bool_c const &) { return True; }
inline constexpr True_c const &inhabits (False_c const &, Bool_c const &) { return True; }
inline constexpr True_c const &inhabits (BoolTerm_c const &, Bool_c const &) { return True; }
inline constexpr True_c const &inhabits (bool const &, Bool_c const &) { return True; }

inline constexpr BoolTerm_c inhabits (BoolTerm_c const &value, TrueType_c const &) { return value == true; }
inline constexpr BoolTerm_c inhabits (BoolTerm_c const &value, FalseType_c const &) { return value == false; }
inline constexpr BoolTerm_c inhabits (bool const &value, TrueType_c const &) { return value == true; }
inline constexpr BoolTerm_c inhabits (bool const &value, FalseType_c const &) { return value == false; }

inline constexpr True_c const &inhabits (Bool_c const &, BoolType_c const &) { return True; }
inline constexpr True_c const &inhabits (TrueType_c const &, BoolType_c const &) { return True; }
inline constexpr True_c const &inhabits (FalseType_c const &, BoolType_c const &) { return True; }

inline constexpr True_c const &inhabits (int8_t const &, Sint8_c const &) { return True; }
inline constexpr True_c const &inhabits (int16_t const &, Sint16_c const &) { return True; }
inline constexpr True_c const &inhabits (int32_t const &, Sint32_c const &) { return True; }
inline constexpr True_c const &inhabits (int64_t const &, Sint64_c const &) { return True; }
inline constexpr True_c const &inhabits (Sint8_c const &, Sint8Type_c const &) { return True; }
inline constexpr True_c const &inhabits (Sint16_c const &, Sint16Type_c const &) { return True; }
inline constexpr True_c const &inhabits (Sint32_c const &, Sint32Type_c const &) { return True; }
inline constexpr True_c const &inhabits (Sint64_c const &, Sint64Type_c const &) { return True; }

inline constexpr True_c const &inhabits (uint8_t const &, Uint8_c const &) { return True; }
inline constexpr True_c const &inhabits (uint16_t const &, Uint16_c const &) { return True; }
inline constexpr True_c const &inhabits (uint32_t const &, Uint32_c const &) { return True; }
inline constexpr True_c const &inhabits (uint64_t const &, Uint64_c const &) { return True; }
inline constexpr True_c const &inhabits (Uint8_c const &, Uint8Type_c const &) { return True; }
inline constexpr True_c const &inhabits (Uint16_c const &, Uint16Type_c const &) { return True; }
inline constexpr True_c const &inhabits (Uint32_c const &, Uint32Type_c const &) { return True; }
inline constexpr True_c const &inhabits (Uint64_c const &, Uint64Type_c const &) { return True; }

inline constexpr True_c const &inhabits (float const &, Float32Type_c const &) { return True; }
inline constexpr True_c const &inhabits (double const &, Float64Type_c const &) { return True; }
inline constexpr True_c const &inhabits (Float32_c const &, Float32Type_c const &) { return True; }
inline constexpr True_c const &inhabits (Float64_c const &, Float64Type_c const &) { return True; }

inline constexpr True_c const &inhabits (Term_c const &, Type_c const &) { return True; }
inline constexpr True_c const &inhabits (NonParametricTerm_c const &, Type_c const &) { return True; }
// inline constexpr True_c const &inhabits (ParametricTerm_c const &, Type_c const &) { return True; }

inline constexpr True_c const &inhabits (Type_c const &, Type_c const &) { return True; }
inline constexpr True_c const &inhabits (NonType_c const &, Type_c const &) { return True; }
inline constexpr True_c const &inhabits (NonParametricType_c const &, Type_c const &) { return True; }
// inline constexpr True_c const &inhabits (ParametricType_c const &, Type_c const &) { return True; }

//
// serialize and deserialize definitions
//

template <typename T_, typename Derived_>
struct SerializationForParametricTerm {
    static void serialize (T_ const &value, std::ostream &out) {
        sept::serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
        sept::serialize(abstract_type_of(value), out);
        Derived_::serialize_value(value, out);
    }
};

template <typename T_>
struct SerializationForPOD : public SerializationForParametricTerm<T_,SerializationForPOD<T_>> {
//     static_assert(std::is_pod_v<T_>);
    static void serialize_value (T_ const &value, std::ostream &out) {
        // NOTE: This just uses machine endianness, so it's not portable.  TODO: Fix this later
        // by defining a serialization context which specifies endianness of the serialized content.
        // TODO: Handle error cases
        out.write(reinterpret_cast<char const *>(&value), sizeof(T_));
    }
    // This assumes that the abstract_type_of has already been deserialized.
    static T_ deserialize_value (std::istream &in) {
        // This awkward situation is to avoid problems where T_ doesn't have a default constructor.
        uint8_t value[sizeof(T_)];
        // TODO: Handle error cases
        in.read(reinterpret_cast<char *>(&value[0]), sizeof(T_));
        return *reinterpret_cast<T_ *>(&value[0]);
    }
};

inline void serialize (BoolTerm_c const &value, std::ostream &out) { SerializationForPOD<BoolTerm_c>::serialize(value, out); }
inline void serialize (bool const &value, std::ostream &out) { SerializationForPOD<bool>::serialize(value, out); }
inline void serialize (int8_t const &value, std::ostream &out) { SerializationForPOD<int8_t>::serialize(value, out); }
inline void serialize (int16_t const &value, std::ostream &out) { SerializationForPOD<int16_t>::serialize(value, out); }
inline void serialize (int32_t const &value, std::ostream &out) { SerializationForPOD<int32_t>::serialize(value, out); }
inline void serialize (int64_t const &value, std::ostream &out) { SerializationForPOD<int64_t>::serialize(value, out); }
inline void serialize (uint8_t const &value, std::ostream &out) { SerializationForPOD<uint8_t>::serialize(value, out); }
inline void serialize (uint16_t const &value, std::ostream &out) { SerializationForPOD<uint16_t>::serialize(value, out); }
inline void serialize (uint32_t const &value, std::ostream &out) { SerializationForPOD<uint32_t>::serialize(value, out); }
inline void serialize (uint64_t const &value, std::ostream &out) { SerializationForPOD<uint64_t>::serialize(value, out); }
inline void serialize (float const &value, std::ostream &out) { SerializationForPOD<float>::serialize(value, out); }
inline void serialize (double const &value, std::ostream &out) { SerializationForPOD<double>::serialize(value, out); }

} // end namespace sept

namespace std {

// Template specialization to define std::hash<sept::NonParametricType_t<...>>.
template <sept::NPTerm ENUM_VALUE_, typename Term_, typename Derived_>
struct hash<sept::NonParametricType_t<ENUM_VALUE_,Term_,Derived_>> {
    size_t operator () (sept::NonParametricType_t<ENUM_VALUE_,Term_,Derived_> const &) const {
        return sept::hash(typeid(Derived_), ENUM_VALUE_, typeid(Term_));
    }
};

} // end namespace std
