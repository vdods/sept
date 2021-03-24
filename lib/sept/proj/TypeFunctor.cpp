// 2020.04.04 - Victor Dods

#include "sept/proj/TypeFunctor.hpp"

#include "sept/ArrayTerm.hpp"
#include "sept/type/Conversion.hpp"
#include "sept/type/Conversions.hpp"

namespace sept {
namespace proj {

Data TypeFunctor::project (Data const &source, Data const &parameters) const {
    // TODO: Could specify type conversion parameters, such as "allow lossy" or "disallow lossy"
    return abstract_type_of_data(source);
}

Data TypeFunctor::project_with_complement (Data const &source, Data const &parameters) const {
    // TODO: Probably want to create some specific types for each kind of embedding
    return make_array(abstract_type_of_data(source), parameters);
}

bool TypeFunctor::can_embed (Data const &pwc_, Data const &target) const {
    auto const &pwc = pwc_.cast<ArrayTerm_c const &>();
    if (pwc.size() != 2) throw std::runtime_error("expected pwc to be ArrayTerm_c of size 2");

    Data const &target_type = pwc[0];
    Data const &source_type = abstract_type_of_data(target);
    type::Conversion const *conversion = nullptr;
    if (!type::Conversions::SINGLETON.can_convert(source_type, target_type, &conversion))
        return false;
    assert(conversion != nullptr);
    // TODO: Once the parameters specify "allow lossy" or "disallow lossy", then check that here.
    return true;
}

void TypeFunctor::embed (Data const &pwc_, Data &target) const {
    auto const &pwc = pwc_.cast<ArrayTerm_c const &>();
    if (pwc.size() != 2) throw std::runtime_error("expected pwc to be ArrayTerm_c of size 2");

    Data const &target_type = pwc[0];
    Data const &source_type = abstract_type_of_data(target);
    type::Conversion const &conversion = type::Conversions::SINGLETON.conversion_for(source_type, target_type);
    // TODO: Once the parameters specify "allow lossy" or "disallow lossy", then verify that here.
    target = conversion(target);
}

} // end namespace proj
} // end namespace sept
