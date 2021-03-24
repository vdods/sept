// 2020.04.05 - Victor Dods

#pragma once

#include "sept/core.hpp"

#include "sept/type/Conversion.hpp"
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <type_traits>
#include <unordered_set>

namespace sept {
namespace type {

// Used as a container to helpfully instantiate the built-in type conversions statically,
// to which other type conversions can be added later.
//
// TODO: Could potentially have a stack of these which form a layered definition, in case you want to
// override some of the defaults.
class Conversions {
public:

    static Conversions SINGLETON;

    // Populates this Conversions instance with all the built-in type conversions.
    Conversions ();
    // TODO: Could make another constructor that starts out with no conversions.

    // Returns true iff there exists a conversion (in this instance of Conversions).  If conversion != nullptr,
    // then the [address of the] found Conversion will be assigned to *conversion.
    bool can_convert (Data const &source_type, Data const &target_type, Conversion const **conversion = nullptr) const;
    // Return the requested conversion, or throw if not found.
    Conversion const &conversion_for (Data const &source_type, Data const &target_type) const;

    // Add a Conversion
    template <typename... Args_>
    void add (Args_&&... args) {
        auto [it, success] = m_conversions.emplace(std::forward<Args_>(args)...);
        if (!success)
            throw std::runtime_error(LVD_FMT("can't add Conversion for " << it->source_type() << " to " << it->target_type() << "; Conversion already present"));
    }

    // TODO: Figure out if it's possible to return Data_t<U_> for some U_
    Data convert_to (Data const &target_type, Data const &data, Conversion::Quality quality = Conversion::Quality::NOT_LOSSY) const {
        Conversion const &conversion = conversion_for(abstract_type_of_data(data), target_type);
        if (quality == Conversion::Quality::NOT_LOSSY
            && (conversion.flags() & Conversion::Flags::CONDITIONALLY_LOSSY) != Conversion::Flags(0)
            && conversion.quality_check(data) == Conversion::Conversion::Quality::LOSSY
        )
            throw std::runtime_error("convert_to specified Conversion::Quality::NOT_LOSSY, but Conversion is CONDITIONALLY_LOSSY and this value would undergo information loss");
        return conversion(data);
    }

private:

    std::unordered_set<Conversion> m_conversions;
};

} // end namespace type
} // end namespace sept
