// 2020.04.04 - Victor Dods

#pragma once

#include <functional>
#include "sept/abstract_type_of.hpp"
#include "sept/core.hpp"
#include "sept/Data.hpp"
#include "sept/hash.hpp"
#include <sstream>
#include <typeindex>

namespace sept {
namespace type {

class Conversions;

// Conversion provides a rich model for type conversions, being able to determine if a conversion will
// result in a loss of information, among other things.
class Conversion {
public:

    // TODO: Consider making separate enum classes for Injectivity { UNSPECIFIED, INJECTIVE, CONDITIONALLY_NON_INJECTIVE, UNCONDITIONALLY_NON_INJECTIVE }
    // Surjectivity { UNSPECIFIED, SURJECTIVE, NON_SURJECTIVE } (are there other kinds here?)

    using FlagsRepr = uint8_t;
    enum class Flags : FlagsRepr {
        NOT_POSSIBLE            = 0,

        IS_SAME_TYPE            = (1 << 0), // Types are actually the same (or perhaps this could mean representationally the same)
        INJECTIVE               = (1 << 1), // Conversion is known to be reversible (no information loss)
        SURJECTIVE              = (1 << 2), // Conversion covers all possible values in target type.
        CONDITIONALLY_LOSSY     = (1 << 3), // Conversion loses information for some source values, but not all.
        UNCONDITIONALLY_LOSSY   = (1 << 4), // Conversion always loses information.

        ISOMORPHIC              = INJECTIVE|SURJECTIVE,
        IDENTITY                = IS_SAME_TYPE|ISOMORPHIC
    };

    using QualityRepr = uint8_t;
    enum class Quality : QualityRepr {
        LOSSY = 0,  // Indicates the conversion undergoes some loss (this may depend on the value to be converted).
        NOT_LOSSY   // Indicates the conversion undergoes no loss (this may depend on the value to be converted).
    };

    using QualityCheck = std::function<Quality(Data const &)>;
    using EvalWithCopy = std::function<Data(Data const &)>;
    using EvalWithMove = std::function<Data(Data &&)>;

    // TODO: Somehow add a function which checks the conditionally lossy case, where
    // the possibilities are:
    // -    this value does not lose information, e.g.
    //      -   int64_t(123) -> char(123)
    //      -   double(800.0) -> int64_t(800)
    //      -   int64_t(0) -> bool(false)
    //      -   int64_t(1) -> bool(true)
    // - this value loses information, e.g.
    //      -   int64_t(4096) -> char(0)
    //      -   int64_t(10) -> bool(true)
    //      -   double(1.7) -> int64_t(2) (? depends on rounding?)

    Conversion () = delete;
    Conversion (Conversion const &) = default;
    Conversion (Conversion &&) = default;
    template <typename QualityCheck_, typename EvalWithCopy_, typename EvalWithMove_>
    Conversion (
        Data const &source_type,
        Data const &target_type,
        Flags flags,
        QualityCheck_ &&quality_check,
        EvalWithCopy_ &&eval_with_copy,
        EvalWithMove_ &&eval_with_move
    )
        :   m_source_type(source_type)
        ,   m_target_type(target_type)
        ,   m_flags(flags)
        ,   m_quality_check(std::forward<QualityCheck_>(quality_check))
        ,   m_eval_with_copy(std::forward<EvalWithCopy_>(eval_with_copy))
        ,   m_eval_with_move(std::forward<EvalWithMove_>(eval_with_move))
    {
        assert(m_flags != Flags::NOT_POSSIBLE);
        // TODO: More validation of m_flags

        // TODO: Potentially provide default implementations of QualityCheck and others
        // in certain cases (e.g. CONDITIONALLY_LOSSY default implementation could always return true
        // for the checker, even though that's not totally accurate; it would give false positives)
    }

    bool operator == (Conversion const &other) const { return m_source_type == other.m_source_type && m_target_type == other.m_target_type; }

    Data const &source_type () const { return m_source_type; }
    Data const &target_type () const { return m_target_type; }
    Flags flags () const { return m_flags; }
    QualityCheck const &quality_check () const { return m_quality_check; }
    EvalWithCopy const &eval_with_copy () const { return m_eval_with_copy; }
    EvalWithMove const &eval_with_move () const { return m_eval_with_move; }

    Data operator () (Data const &data) const {
        verify_source_type_or_throw(abstract_type_of_data(data));
        return m_eval_with_copy(data);
    }
    Data operator () (Data &&data) const {
        verify_source_type_or_throw(abstract_type_of_data(data));
        return m_eval_with_move(std::move(data));
    }

    // This uses the QualityCheck to determine if the particular value
    // would experience information loss in the conversion or not.  Returns true iff
    // there would be information loss.
    Quality quality_check (Data const &data) const;

private:

    // Used only in class Conversions, for use in finding conversions (really this just carries
    // the source_type and target_type, and the other values are dummies; this is sorta kludgey,
    // but for now that's fine).
    Conversion (Data const &source_type, Data const &target_type)
        :   m_source_type(source_type)
        ,   m_target_type(target_type)
        ,   m_flags(Flags::NOT_POSSIBLE) // Just a dummy
        ,   m_quality_check(nullptr) // Just a dummy
        ,   m_eval_with_copy(nullptr) // Just a dummy
        ,   m_eval_with_move(nullptr) // Just a dummy
    { }

    void verify_source_type_or_throw (Data const &type) const {
        if (type != m_source_type)
            throw std::runtime_error(LVD_FMT("expected type " << m_source_type << " but got " << type));
    }

    Data m_source_type;
    Data m_target_type;
    Flags m_flags;
    QualityCheck m_quality_check;
    EvalWithCopy m_eval_with_copy;
    EvalWithMove m_eval_with_move;

    friend class sept::type::Conversions;
};

inline Conversion::Flags operator & (Conversion::Flags lhs, Conversion::Flags rhs) {
    return Conversion::Flags(Conversion::FlagsRepr(lhs) & Conversion::FlagsRepr(rhs));
}

inline Conversion::Flags operator | (Conversion::Flags lhs, Conversion::Flags rhs) {
    return Conversion::Flags(Conversion::FlagsRepr(lhs) | Conversion::FlagsRepr(rhs));
}

inline Conversion make_identity_conversion (Data const &type) {
    return Conversion(
        type, type, // source and target type
        Conversion::Flags::IDENTITY,
        nullptr, // quality_check
        [](Data const &data)->Data{ return data; },
        [](Data &&data)->Data{ return std::move(data); }
    );
}

} // end namespace type
} // end namespace sept

// Specialization for defining hash value of a Conversion, for use in unordered_set.
template <>
struct std::hash<sept::type::Conversion> {
    size_t operator () (sept::type::Conversion const &c) const {
        return sept::hash(c.source_type(), c.target_type());
    }
};
