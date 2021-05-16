// 2021.05.08 - Victor Dods

#pragma once

#include <lvd/fmt.hpp>
#include <lvd/hash.hpp>
#include "sept/FormalTypeOf.hpp"
#include "sept/NPType.hpp"
#include <stdexcept>
#include <string>
#include <utility>

#define ABSTRACT_TYPE_CONSTRUCTOR \
    template <typename Argument_> \
    Argument_ operator() (Argument_ &&argument) const { \
        if (!inhabits(argument, *this)) \
            throw std::runtime_error(LVD_FMT("argument " << argument << " does not inhabit abstract type " << *this)); \
        return std::forward<Argument_>(argument); \
    }

//
// Misc
//

inline int compare (uint8_t lhs, uint8_t rhs) {
    return int(lhs) - int(rhs);
}

//
// Non-parametric terms for this AST POC
//

// TODO: Could potentially split these up to make them distinct C++ types,
// which would make certain code be constexpr.
using ThinkyNPTerm_CType = uint8_t;
enum class ThinkyNPTerm : ThinkyNPTerm_CType {
    //
    // BinOp
    //

    // Verbs
    IMPLIES = 0,    // Used in rules of inference
    IS_A,
    HAS_ENTITY,     // Has the specific Entity
    HAS_A,          // Has at least one instance of
    HAS_EVERY,      // Has all instances of
    HAS_PROPERTY,   // Has the named property
    LIKES_ENTITY,
    LIKES_A,
    LIKES_EVERY,
    HATES_ENTITY,
    HATES_A,
    HATES_EVERY,
    SAYS,
    TAKES,
    DROPS,
    GIVES,
    __Verb_LOWEST__ = IMPLIES,
    __Verb_HIGHEST__ = GIVES,

    __BinOp_LOWEST__ = __Verb_LOWEST__,
    __BinOp_HIGHEST__ = __Verb_HIGHEST__,
    // TODO: Define more categories for the BinOp poset

    //
    // UnOp
    //

    // Logical
    NOT,
    AND, // Expects the argument to be an array/tuple; (And, ()) evaluates as True (i.e. identity value w.r.t. And)
    OR, // Expects the argument to be an array/tuple; (Or, ()) evaluates as False (i.e. identity value w.r.t. Or)
    XOR, // Expects the argument to be an array/tuple; (Xor, ()) evaluates as False (i.e. identity value w.r.t. Xor)
    __BoolBinOp_LOWEST__ = AND,
    __BoolBinOp_HIGHEST__ = XOR,

    // There's probably some linguistic word for these.
    SHOULD,
    COULD,
    WOULD,

    // Adjectives
    COOL,
    LAME,
    BIG,
    SMALL,
    STUPID,
    SMART,
    FAST,
    SLOW,
    // Colors < Adjectives
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    BLUE,
    INDIGO,
    VIOLET,
    __Adjective_LOWEST__ = NOT,
    __Adjective_HIGHEST__ = VIOLET,
    // TODO: Could have "PositiveAdjective" and "NegativeAdjective" subtypes

    __Color_LOWEST__ = RED,
    __Color_HIGHEST__ = VIOLET,

    __UnOp_LOWEST__ = NOT,
    __UnOp_HIGHEST__ = VIOLET,

    // Misc
    IF,
    THEN,
    OTHERWISE,

    //
    // Terminals
    //

    // Objects
    BOOK,
    BOX,
    CUP,
    HAT,
    __Object_LOWEST__ = BOOK,
    __Object_HIGHEST__ = HAT,

    // People
    ALICE,
    BOB,
    CHARLIE,
    DAVE,
    __Person_LOWEST__ = ALICE,
    __Person_HIGHEST__ = DAVE,

    // Animals
    CAT,
    DOG,
    __Animal_LOWEST__ = CAT,
    __Animal_HIGHEST__ = DOG,

    // An Entity is a Person or Object or any concrete thing.
    __Entity_LOWEST__ = __Object_LOWEST__,
    __Entity_HIGHEST__ = __Animal_HIGHEST__,

    __LOWEST__ = __BinOp_LOWEST__,
    __HIGHEST__ = __Entity_HIGHEST__,
};

size_t constexpr THINKY_NP_TERM_COUNT = size_t(ThinkyNPTerm::__HIGHEST__)+1 - size_t(ThinkyNPTerm::__LOWEST__);

std::string const &as_string (ThinkyNPTerm t);

inline std::ostream &operator<< (std::ostream &out, ThinkyNPTerm const &t) {
    return out << as_string(t);
}

inline int compare (ThinkyNPTerm const &lhs, ThinkyNPTerm const &rhs) {
    return compare(ThinkyNPTerm_CType(lhs), ThinkyNPTerm_CType(rhs));
}

inline ThinkyNPTerm constexpr Implies = ThinkyNPTerm::IMPLIES;
inline ThinkyNPTerm constexpr IsA = ThinkyNPTerm::IS_A;
inline ThinkyNPTerm constexpr HasEntity = ThinkyNPTerm::HAS_ENTITY;
inline ThinkyNPTerm constexpr HasA = ThinkyNPTerm::HAS_A;
inline ThinkyNPTerm constexpr HasEvery = ThinkyNPTerm::HAS_EVERY;
inline ThinkyNPTerm constexpr HasProperty = ThinkyNPTerm::HAS_PROPERTY;
inline ThinkyNPTerm constexpr LikesEntity = ThinkyNPTerm::LIKES_ENTITY;
inline ThinkyNPTerm constexpr LikesA = ThinkyNPTerm::LIKES_A;
inline ThinkyNPTerm constexpr LikesEvery = ThinkyNPTerm::LIKES_EVERY;
inline ThinkyNPTerm constexpr HatesEntity = ThinkyNPTerm::HATES_ENTITY;
inline ThinkyNPTerm constexpr HatesA = ThinkyNPTerm::HATES_A;
inline ThinkyNPTerm constexpr HatesEvery = ThinkyNPTerm::HATES_EVERY;
inline ThinkyNPTerm constexpr Says = ThinkyNPTerm::SAYS;
inline ThinkyNPTerm constexpr Takes = ThinkyNPTerm::TAKES;
inline ThinkyNPTerm constexpr Drops = ThinkyNPTerm::DROPS;

inline ThinkyNPTerm constexpr Not = ThinkyNPTerm::NOT;
inline ThinkyNPTerm constexpr And = ThinkyNPTerm::AND;
inline ThinkyNPTerm constexpr Or = ThinkyNPTerm::OR;
inline ThinkyNPTerm constexpr Xor = ThinkyNPTerm::XOR;

inline ThinkyNPTerm constexpr Should = ThinkyNPTerm::SHOULD;
inline ThinkyNPTerm constexpr Could = ThinkyNPTerm::COULD;
inline ThinkyNPTerm constexpr Would = ThinkyNPTerm::WOULD;

inline ThinkyNPTerm constexpr Cool = ThinkyNPTerm::COOL;
inline ThinkyNPTerm constexpr Lame = ThinkyNPTerm::LAME;
inline ThinkyNPTerm constexpr Big = ThinkyNPTerm::BIG;
inline ThinkyNPTerm constexpr Small = ThinkyNPTerm::SMALL;
inline ThinkyNPTerm constexpr Stupid = ThinkyNPTerm::STUPID;
inline ThinkyNPTerm constexpr Smart = ThinkyNPTerm::SMART;
inline ThinkyNPTerm constexpr Fast = ThinkyNPTerm::FAST;
inline ThinkyNPTerm constexpr Slow = ThinkyNPTerm::SLOW;

inline ThinkyNPTerm constexpr Red = ThinkyNPTerm::RED;
inline ThinkyNPTerm constexpr Orange = ThinkyNPTerm::ORANGE;
inline ThinkyNPTerm constexpr Yellow = ThinkyNPTerm::YELLOW;
inline ThinkyNPTerm constexpr Green = ThinkyNPTerm::GREEN;
inline ThinkyNPTerm constexpr Blue = ThinkyNPTerm::BLUE;
inline ThinkyNPTerm constexpr Indigo = ThinkyNPTerm::INDIGO;
inline ThinkyNPTerm constexpr Violet = ThinkyNPTerm::VIOLET;

inline ThinkyNPTerm constexpr If = ThinkyNPTerm::IF;
inline ThinkyNPTerm constexpr Then = ThinkyNPTerm::THEN;
inline ThinkyNPTerm constexpr Otherwise = ThinkyNPTerm::OTHERWISE;

inline ThinkyNPTerm constexpr Book = ThinkyNPTerm::BOOK;
inline ThinkyNPTerm constexpr Box = ThinkyNPTerm::BOX;
inline ThinkyNPTerm constexpr Cup = ThinkyNPTerm::CUP;
inline ThinkyNPTerm constexpr Hat = ThinkyNPTerm::HAT;

inline ThinkyNPTerm constexpr Alice = ThinkyNPTerm::ALICE;
inline ThinkyNPTerm constexpr Bob = ThinkyNPTerm::BOB;
inline ThinkyNPTerm constexpr Charlie = ThinkyNPTerm::CHARLIE;
inline ThinkyNPTerm constexpr Dave = ThinkyNPTerm::DAVE;

inline ThinkyNPTerm constexpr Cat = ThinkyNPTerm::CAT;
inline ThinkyNPTerm constexpr Dog = ThinkyNPTerm::DOG;

//
// Sigils that represent different categories of things.  This is sort of vaguely defined at the moment.
//

struct Adjective_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct Animal_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct BinOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct BoolBinOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct Color_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct Entity_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct Object_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct Person_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct UnOp_c { ABSTRACT_TYPE_CONSTRUCTOR };
struct Verb_c { ABSTRACT_TYPE_CONSTRUCTOR };

// These are all singletons
inline bool constexpr operator== (Adjective_c const &, Adjective_c const &) { return true; }
inline bool constexpr operator== (Animal_c const &, Animal_c const &) { return true; }
inline bool constexpr operator== (BinOp_c const &, BinOp_c const &) { return true; }
inline bool constexpr operator== (BoolBinOp_c const &, BoolBinOp_c const &) { return true; }
inline bool constexpr operator== (Color_c const &, Color_c const &) { return true; }
inline bool constexpr operator== (Entity_c const &, Entity_c const &) { return true; }
inline bool constexpr operator== (Object_c const &, Object_c const &) { return true; }
inline bool constexpr operator== (Person_c const &, Person_c const &) { return true; }
inline bool constexpr operator== (UnOp_c const &, UnOp_c const &) { return true; }
inline bool constexpr operator== (Verb_c const &, Verb_c const &) { return true; }

inline std::ostream &operator<< (std::ostream &out, Adjective_c const &) { return out << "Adjective"; }
inline std::ostream &operator<< (std::ostream &out, Animal_c const &) { return out << "Animal"; }
inline std::ostream &operator<< (std::ostream &out, BinOp_c const &) { return out << "BinOp"; }
inline std::ostream &operator<< (std::ostream &out, BoolBinOp_c const &) { return out << "BoolBinOp"; }
inline std::ostream &operator<< (std::ostream &out, Color_c const &) { return out << "Color"; }
inline std::ostream &operator<< (std::ostream &out, Entity_c const &) { return out << "Entity"; }
inline std::ostream &operator<< (std::ostream &out, Object_c const &) { return out << "Object"; }
inline std::ostream &operator<< (std::ostream &out, Person_c const &) { return out << "Person"; }
inline std::ostream &operator<< (std::ostream &out, UnOp_c const &) { return out << "UnOp"; }
inline std::ostream &operator<< (std::ostream &out, Verb_c const &) { return out << "Verb"; }

inline sept::NonParametricType_c abstract_type_of (Adjective_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (Animal_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (BinOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (BoolBinOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (Color_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (Entity_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (Object_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (Person_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (UnOp_c const &) { return sept::NonParametricType; }
inline sept::NonParametricType_c abstract_type_of (Verb_c const &) { return sept::NonParametricType; }

inline auto constexpr Adjective = Adjective_c{};
inline auto constexpr Animal = Adjective_c{};
inline auto constexpr BinOp = BinOp_c{};
inline auto constexpr BoolBinOp = BoolBinOp_c{};
inline auto constexpr Color = Color_c{};
inline auto constexpr Entity = Entity_c{}; // TODO: Entity could be sept::Union(Object, Person)
inline auto constexpr Object = Object_c{};
inline auto constexpr Person = Person_c{};
inline auto constexpr UnOp = UnOp_c{};
inline auto constexpr Verb = Verb_c{};

//
// Definitions of inhabits
//

inline bool constexpr inhabits (ThinkyNPTerm t, Adjective_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__Adjective_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__Adjective_HIGHEST__);
}

inline bool constexpr inhabits (ThinkyNPTerm t, Animal_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__Animal_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__Animal_HIGHEST__);
}

inline bool constexpr inhabits (ThinkyNPTerm t, BinOp_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__BinOp_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__BinOp_HIGHEST__);
}

inline bool constexpr inhabits (ThinkyNPTerm t, BoolBinOp_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__BoolBinOp_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__BoolBinOp_HIGHEST__);
}

inline bool constexpr inhabits (ThinkyNPTerm t, Color_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__Color_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__Color_HIGHEST__);
}

inline bool constexpr inhabits (ThinkyNPTerm t, Entity_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__Entity_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__Entity_HIGHEST__);
}

inline bool constexpr inhabits (ThinkyNPTerm t, Object_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__Object_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__Object_HIGHEST__);
}

inline bool constexpr inhabits (ThinkyNPTerm t, Person_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__Person_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__Person_HIGHEST__);
}

inline bool constexpr inhabits (ThinkyNPTerm t, UnOp_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__UnOp_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__UnOp_HIGHEST__);
}

inline bool constexpr inhabits (ThinkyNPTerm t, Verb_c const &) {
    return ThinkyNPTerm_CType(ThinkyNPTerm::__Verb_LOWEST__) <= ThinkyNPTerm_CType(t)
        && ThinkyNPTerm_CType(t) <= ThinkyNPTerm_CType(ThinkyNPTerm::__Verb_HIGHEST__);
}

inline bool inhabits (ThinkyNPTerm t, sept::FormalTypeOf_Term_c const &f) {
    return inhabits(sept::Data(t), f);
}

namespace std {

template <>
struct hash<ThinkyNPTerm> {
    size_t operator () (ThinkyNPTerm const &t) const {
        return lvd::hash(typeid(ThinkyNPTerm), ThinkyNPTerm_CType(t));
    }
};

template <>
struct hash<Adjective_c> {
    size_t operator () (Adjective_c const &t) const {
        return lvd::hash(typeid(Adjective_c));
    }
};

template <>
struct hash<Animal_c> {
    size_t operator () (Animal_c const &t) const {
        return lvd::hash(typeid(Animal_c));
    }
};

template <>
struct hash<BinOp_c> {
    size_t operator () (BinOp_c const &t) const {
        return lvd::hash(typeid(BinOp_c));
    }
};

template <>
struct hash<BoolBinOp_c> {
    size_t operator () (BoolBinOp_c const &t) const {
        return lvd::hash(typeid(BoolBinOp_c));
    }
};

template <>
struct hash<Color_c> {
    size_t operator () (Color_c const &t) const {
        return lvd::hash(typeid(Color_c));
    }
};

template <>
struct hash<Entity_c> {
    size_t operator () (Entity_c const &t) const {
        return lvd::hash(typeid(Entity_c));
    }
};

template <>
struct hash<Object_c> {
    size_t operator () (Object_c const &t) const {
        return lvd::hash(typeid(Object_c));
    }
};

template <>
struct hash<Person_c> {
    size_t operator () (Person_c const &t) const {
        return lvd::hash(typeid(Person_c));
    }
};

template <>
struct hash<UnOp_c> {
    size_t operator () (UnOp_c const &t) const {
        return lvd::hash(typeid(UnOp_c));
    }
};

template <>
struct hash<Verb_c> {
    size_t operator () (Verb_c const &t) const {
        return lvd::hash(typeid(Verb_c));
    }
};

} // end namespace std
