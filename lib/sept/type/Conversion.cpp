// 2020.04.04 - Victor Dods

#include "sept/type/Conversion.hpp"

#include <sstream>

namespace sept {
namespace type {

Conversion::Quality Conversion::quality_check (Data const &data) const {
    verify_source_type_or_throw(abstract_type_of_data(data));

    if ((m_flags & (Flags::IS_SAME_TYPE|Flags::INJECTIVE)) != Flags(0))
        return Quality::NOT_LOSSY;
    if ((m_flags & Flags::UNCONDITIONALLY_LOSSY) != Flags(0))
        return Quality::LOSSY;
    if ((m_flags & Flags::CONDITIONALLY_LOSSY) != Flags(0))
        return m_quality_check(data);

    throw std::runtime_error("this should never happen; some validation on combinations of Flags must need to be implemented");
}

} // end namespace type
} // end namespace sept
