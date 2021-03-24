// 2020.04.05 - Victor Dods

#include "sept/type/Conversions.hpp"

#include <cmath>
#include "sept/ArrayType.hpp"
#include "sept/NPType.hpp"
#include "sept/OrderedMapType.hpp"
#include <sstream>

namespace sept {
namespace type {

Conversions Conversions::SINGLETON;

// Returns the number of least significant zeros before the lowest set bit in n, or
// 8*sizeof(size_t) if n == 0.
template <typename Uint_>
Uint_ trailing_zero_bit_count (Uint_ n) {
    if (n == 0)
        return 8*sizeof(Uint_);

    Uint_ retval = 0;
    size_t constexpr LOWEST_BIT = 1;
    while ((n & LOWEST_BIT) == 0) {
        ++retval;
        n >>= 1;
    }
    return retval;
}

// Returns the number of most significant zeros before the highest set bit in n, or
// 8*sizeof(size_t) if n == 0.
template <typename Uint_>
Uint_ leading_zero_bit_count (Uint_ n) {
    if (n == 0)
        return 8*sizeof(Uint_);

    Uint_ retval = 0;
    size_t constexpr HIGHEST_BIT = Uint_(1) << (8*sizeof(Uint_)-1);
    while ((n & HIGHEST_BIT) == 0) {
        ++retval;
        n <<= 1;
    }
    return retval;
}

// Returns the number of significant bits in n, which is the number of bits necessary
// to store the contiguous sequence of nonzero bits (i.e. the total number of bits
// minus the leading and trailing zero bit counts of n), or 0 if n == 0.
template <typename Uint_>
Uint_ significant_bit_count (Uint_ n) {
    if (n == 0)
        return 0;

    return 8*sizeof(Uint_) - leading_zero_bit_count(n) - trailing_zero_bit_count(n);
}

Conversions::Conversions () {
//     add(make_identity_conversion<NonParametricType_t<...>>()); -- however this could be implemented

    //
    // Add identity conversions for everything
    //

    // Not sure if these abstract types actually call for an identity conversion, but why not.
    add(make_identity_conversion(Term));
    add(make_identity_conversion(Type));
    add(make_identity_conversion(NonType));
    add(make_identity_conversion(NonParametricType));
    add(make_identity_conversion(EmptyType));
    // These types have concrete inhabitants.
    add(make_identity_conversion(VoidType));
    add(make_identity_conversion(TrueType));
    add(make_identity_conversion(FalseType));
    add(make_identity_conversion(Bool));
    add(make_identity_conversion(Sint8));
    add(make_identity_conversion(Sint16));
    add(make_identity_conversion(Sint32));
    add(make_identity_conversion(Sint64));
    add(make_identity_conversion(Uint8));
    add(make_identity_conversion(Uint16));
    add(make_identity_conversion(Uint32));
    add(make_identity_conversion(Uint64));
    add(make_identity_conversion(Float32));
    add(make_identity_conversion(Float64));
    // Higher order types
    add(make_identity_conversion(BoolType));
    add(make_identity_conversion(Sint8Type));
    add(make_identity_conversion(Sint16Type));
    add(make_identity_conversion(Sint32Type));
    add(make_identity_conversion(Sint64Type));
    add(make_identity_conversion(Uint8Type));
    add(make_identity_conversion(Uint16Type));
    add(make_identity_conversion(Uint32Type));
    add(make_identity_conversion(Uint64Type));
    add(make_identity_conversion(Float32Type));
    add(make_identity_conversion(Float64Type));

    // Array types
    add(make_identity_conversion(ArrayType));
    add(make_identity_conversion(ArrayES));
    add(make_identity_conversion(ArrayE));
    add(make_identity_conversion(ArrayS));
    add(make_identity_conversion(Array));
    // The other array types (ArrayESTerm_c, ArrayETerm_c, ArraySTerm_c) each have parameters, so they can't have
    // fixed conversions; they have to generated dynamically.

    add(make_identity_conversion(OrderedMapType));
    add(make_identity_conversion(OrderedMapDC));
    add(make_identity_conversion(OrderedMapD));
    add(make_identity_conversion(OrderedMapC));
    add(make_identity_conversion(OrderedMap));
    // The other ordered map types (OrderedMapDCTerm_c, OrderedMapDTerm_c, OrderedMapCTerm_c) each have parameters, so they can't have
    // fixed conversions; they have to generated dynamically.

    //
    // Sint64 -> <othertypes>
    //

    add(Sint64, Float64,
        Conversion::Flags::CONDITIONALLY_LOSSY,
        [](Data const &data)->Conversion::Quality{ // QualityCheck
            int64_t n = data.cast<int64_t>();
            // Too many bits, and can't negate this value to get a positive value.
            if (n == std::numeric_limits<int64_t>::min())
                return Conversion::Quality::LOSSY;
            // The conversion is not lossy if the number of leading nonzero bits will fit in the mantissa.
            // Convert to unsigned abs value for bit operations.
            size_t abs_n = n < 0 ? size_t(-n) : size_t(n);
            // double-precision has 53 bit mantissa (this includes the implicitly stored leading 1)
            // See https://en.wikipedia.org/wiki/Double-precision_floating-point_format
            // If significant bits exceed mantissa bits, then the conversion is lossy.
            return significant_bit_count(abs_n) <= 53 ? Conversion::Quality::NOT_LOSSY : Conversion::Quality::LOSSY;
        },
        [](Data const &data)->Data{ // EvalWithCopy
            return double(data.cast<int64_t>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return double(data.cast<int64_t>());
        }
    );
    add(Sint64, Sint8, // NOTE: This was originally meant to be conversion to CHAR; Sint8 is playing the part of CHAR here
        Conversion::Flags::SURJECTIVE|Conversion::Flags::CONDITIONALLY_LOSSY,
        [](Data const &data)->Conversion::Quality{ // QualityCheck
            int64_t n = data.cast<int64_t>();
            // Simple range check
            using limits = std::numeric_limits<char>;
            return (int64_t(limits::min()) <= n && n <= int64_t(limits::max())) ? Conversion::Quality::NOT_LOSSY : Conversion::Quality::LOSSY;
        },
        [](Data const &data)->Data{ // EvalWithCopy
            return char(data.cast<int64_t>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return char(data.cast<int64_t>());
        }
    );
    add(Sint64, Bool,
        Conversion::Flags::SURJECTIVE|Conversion::Flags::CONDITIONALLY_LOSSY,
        [](Data const &data)->Conversion::Quality{ // QualityCheck
            static_assert(int64_t(false) == int64_t(0));
            static_assert(int64_t(true) == int64_t(1));
            int64_t n = data.cast<int64_t>();
            // Simple value check -- C++ specifies that int64_t(false) == 0 and int64_t(true) == 1.
            // This is also the only way to make it compatible with other type conversions (in the sense
            // that there is a diagram that commutes).
            return (n == int64_t(false) || n == int64_t(true)) ? Conversion::Quality::NOT_LOSSY : Conversion::Quality::LOSSY;
        },
        [](Data const &data)->Data{ // EvalWithCopy
            return bool(data.cast<int64_t>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return bool(data.cast<int64_t>());
        }
    );

    //
    // double -> <othertypes>
    //

    add(Float64, Sint64,
        Conversion::Flags::CONDITIONALLY_LOSSY,
        [](Data const &data)->Conversion::Quality{ // QualityCheck
            double x = data.cast<double>();
            // There is a loss if x has a fractional part, or if it is out of range of int64_t.
            if (std::trunc(x) != x)
                return Conversion::Quality::LOSSY;
            else if (x < -std::exp2(8*sizeof(int64_t)-1)) // Min int64_t
                return Conversion::Quality::LOSSY;
            else if (x >= std::exp2(8*sizeof(int64_t)-1)) // Max int64_t (plus one; notice inclusion)
                return Conversion::Quality::LOSSY;
            else
                return Conversion::Quality::NOT_LOSSY;
        },
        [](Data const &data)->Data{ // EvalWithCopy
            // NOTE: This does whatever C++ does, which might not be what we want.
            return int64_t(data.cast<double>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            // NOTE: This does whatever C++ does, which might not be what we want.
            return int64_t(data.cast<double>());
        }
    );
    add(Float64, Sint8, // NOTE: This was originally meant to be conversion to CHAR; Sint8 is playing the part of CHAR here
        Conversion::Flags::SURJECTIVE|Conversion::Flags::CONDITIONALLY_LOSSY,
        [](Data const &data)->Conversion::Quality{ // QualityCheck
            double x = data.cast<double>();
            // Simple range check
            using limits = std::numeric_limits<char>;
            return (double(limits::min()) <= x && x <= double(limits::max())) ? Conversion::Quality::NOT_LOSSY : Conversion::Quality::LOSSY;
        },
        [](Data const &data)->Data{ // EvalWithCopy
            // NOTE: This does whatever C++ does, which might not be what we want.
            return char(data.cast<double>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            // NOTE: This does whatever C++ does, which might not be what we want.
            return char(data.cast<double>());
        }
    );
    add(Float64, Bool,
        Conversion::Flags::SURJECTIVE|Conversion::Flags::CONDITIONALLY_LOSSY,
        [](Data const &data)->Conversion::Quality{ // QualityCheck
            double x = data.cast<double>();
            // Simple value check -- C++ specifies that double(false) == 0 and double(true) == 1.
            // This is also the only way to make it compatible with other type conversions (in the sense
            // that there is a diagram that commutes).
            return (x == double(false) || x == double(true)) ? Conversion::Quality::NOT_LOSSY : Conversion::Quality::LOSSY;
        },
        [](Data const &data)->Data{ // EvalWithCopy
            return bool(data.cast<double>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return bool(data.cast<double>());
        }
    );

    //
    // char -> <othertypes>
    //

    add(Sint8, Sint64, // NOTE: This was originally meant to be conversion from CHAR; Sint8 is playing the part of CHAR here
        Conversion::Flags::INJECTIVE,
        nullptr, // QualityCheck (not needed for INJECTIVE)
        [](Data const &data)->Data{ // EvalWithCopy
            return int64_t(data.cast<char>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return int64_t(data.cast<char>());
        }
    );
    add(Sint8, Float64, // NOTE: This was originally meant to be conversion from CHAR; Sint8 is playing the part of CHAR here
        Conversion::Flags::INJECTIVE,
        nullptr, // QualityCheck (not needed for INJECTIVE)
        [](Data const &data)->Data{ // EvalWithCopy
            return double(data.cast<char>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return double(data.cast<char>());
        }
    );
    add(Sint8, Bool, // NOTE: This was originally meant to be conversion from CHAR; Sint8 is playing the part of CHAR here
        Conversion::Flags::SURJECTIVE|Conversion::Flags::CONDITIONALLY_LOSSY,
        [](Data const &data)->Conversion::Quality{ // QualityCheck
            static_assert(char(false) == char(0));
            static_assert(char(true) == char(1));
            char n = data.cast<char>();
            // Simple value check -- C++ specifies that int64_t(false) == 0 and int64_t(true) == 1.
            // This is also the only way to make it compatible with other type conversions (in the sense
            // that there is a diagram that commutes).
            return (n == char(false) || n == char(true)) ? Conversion::Quality::NOT_LOSSY : Conversion::Quality::LOSSY;
        },
        [](Data const &data)->Data{ // EvalWithCopy
            return bool(data.cast<char>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return bool(data.cast<char>());
        }
    );

    //
    // Bool -> <othertypes>
    //

    add(Bool, Sint64,
        Conversion::Flags::INJECTIVE,
        nullptr, // QualityCheck (not needed for INJECTIVE)
        [](Data const &data)->Data{ // EvalWithCopy
            return int64_t(data.cast<bool>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return int64_t(data.cast<bool>());
        }
    );
    add(Bool, Float64,
        Conversion::Flags::INJECTIVE,
        nullptr, // QualityCheck (not needed for INJECTIVE)
        [](Data const &data)->Data{ // EvalWithCopy
            return double(data.cast<bool>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return double(data.cast<bool>());
        }
    );
    add(Bool, Sint8, // NOTE: This was originally meant to be conversion to CHAR; Sint8 is playing the part of CHAR here
        Conversion::Flags::INJECTIVE,
        nullptr, // QualityCheck (not needed for INJECTIVE)
        [](Data const &data)->Data{ // EvalWithCopy
            return char(data.cast<bool>());
        },
        [](Data &&data)->Data{ // EvalWithMove
            return char(data.cast<bool>());
        }
    );

//     static CONVERSION_MAP const std::unordered_map<std::pair<std::type_info,std::type_info>, TypeConversionFlags>{
//         { {typeid(ArrayESTerm_c), typeid(ArrayESTerm_c)}, TypeConversionFlags::IDENTITY },
//         { {typeid(ArrayESTerm_c), typeid(ArrayETerm_c)},  TypeConversionFlags::UNCONDITIONALLY_LOSSY }, // the constraint is being weakened
//         { {typeid(ArrayESTerm_c), typeid(ArraySTerm_c)},  TypeConversionFlags::UNCONDITIONALLY_LOSSY }, // the constraint is being weakened
//         { {typeid(ArrayESTerm_c), typeid(Array_c)},   TypeConversionFlags::UNCONDITIONALLY_LOSSY }, // the constraint is being weakened
//
//         { {typeid(ArrayETerm_c), typeid(ArrayESTerm_c)},  TypeConversionFlags::INJECTIVE }, // the array size is used to set the constraint
//         { {typeid(ArrayETerm_c), typeid(ArrayETerm_c)},   TypeConversionFlags::IDENTITY },
//         { {typeid(ArrayETerm_c), typeid(ArraySTerm_c)},   TypeConversionFlags::UNCONDITIONALLY_LOSSY }, // the constraint is being changed
//         { {typeid(ArrayETerm_c), typeid(Array_c)},    TypeConversionFlags::UNCONDITIONALLY_LOSSY }, // the constraint is being weakened
//
//         { {typeid(ArraySTerm_c), typeid(ArrayESTerm_c)},  TypeConversionFlags::INJECTIVE }, // the array element type is used to set the constraint
//         { {typeid(ArraySTerm_c), typeid(ArrayETerm_c)},   TypeConversionFlags::UNCONDITIONALLY_LOSSY }, // the constraint is being changed
//         { {typeid(ArraySTerm_c), typeid(ArraySTerm_c)},   TypeConversionFlags::IDENTITY },
//         { {typeid(ArraySTerm_c), typeid(Array_c)},    TypeConversionFlags::UNCONDITIONALLY_LOSSY }, // the constraint is being weakened
//
//         { {typeid(Array_c), typeid(ArrayESTerm_c)},   TypeConversionFlags::INJECTIVE }, // the array element type and size is used to set the constraint
//         { {typeid(Array_c), typeid(ArrayETerm_c)},    TypeConversionFlags::INJECTIVE }, // the array element type is used to set the constraint
//         { {typeid(Array_c), typeid(ArraySTerm_c)},    TypeConversionFlags::INJECTIVE }, // the array size is used to set the constraint
//         { {typeid(Array_c), typeid(Array_c)},     TypeConversionFlags::IDENTITY },
//     };
//
//     if (source_type.type() == Sint64) {
//         if (target_type.type()
//     }
}

bool Conversions::can_convert (Data const &source_type, Data const &target_type, Conversion const **conversion) const {
    auto it = m_conversions.find(Conversion(source_type, target_type));
    if (it == m_conversions.end()) {
        // Check if the requested conversion can be dynamically generated.
        // TODO: Figure out a more general framework for this.
        if (source_type.type() == typeid(ArrayESTerm_c)) {

        }

        return false;
    }

    if (conversion != nullptr)
        *conversion = &*it; // This looks dumb but the iterator dereference changes the type.
    return true;
}

Conversion const &Conversions::conversion_for (Data const &source_type, Data const &target_type) const {
    auto it = m_conversions.find(Conversion(source_type, target_type));
    if (it == m_conversions.end())
        throw std::runtime_error(LVD_FMT("conversion_for(" << source_type << ", " << target_type << ") not found"));
    return *it;
}

} // end namespace type
} // end namespace sept
