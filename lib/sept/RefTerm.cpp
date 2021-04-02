// 2021.03.31 - Victor Dods

#include "sept/RefTerm.hpp"

#include "sept/Data.hpp"
#include "sept/NPTerm.hpp"
#include "sept/Ref.hpp"

namespace sept {

Data RefTerm_c::referenced_data () && {
    return std::move(*m_ptr);
}

Data RefTerm_c::operator() (Data const &argument) const {
    return referenced_data()(argument);
}

Data RefTerm_c::operator[] (Data const &param) const {
    return referenced_data()[param];
}

RefTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        // TODO: This will print a cyclic reference in an infinite loop unless something stops it
        out << "RefTerm_c(" << *m_ptr << ')';
    });
}

//
// Data model functions
//

bool operator== (RefTerm_c const &lhs, RefTerm_c const &rhs) {
    return &lhs.referenced_data() == &rhs.referenced_data() || lhs.referenced_data() == rhs.referenced_data();
}
// // NOTE: These aren't yet present in DataEq registration, since it isn't yet build to take asymmetric type equality
// bool operator== (RefTerm_c const &lhs, Data const &rhs) {
//     return lhs.referenced_data() == rhs;
// }
// bool operator== (Data const &lhs, RefTerm_c const &rhs) {
//     return lhs == rhs.referenced_data();
// }

// Forward to referenced_data
Data abstract_type_of (RefTerm_c const &r) {
    return abstract_type_of_data(r.referenced_data());
}

Data element_of (RefTerm_c const &r, int8_t index) { return r[index]; }
Data element_of (RefTerm_c const &r, int16_t index) { return r[index]; }
Data element_of (RefTerm_c const &r, int32_t index) { return r[index]; }
Data element_of (RefTerm_c const &r, int64_t index) { return r[index]; }
Data element_of (RefTerm_c const &r, uint8_t index) { return r[index]; }
Data element_of (RefTerm_c const &r, uint16_t index) { return r[index]; }
Data element_of (RefTerm_c const &r, uint32_t index) { return r[index]; }
Data element_of (RefTerm_c const &r, uint64_t index) { return r[index]; }

// Forward inhabit to referenced_data of each.
bool inhabits (RefTerm_c const &value, RefTerm_c const &type) {
    return inhabits_data(value.referenced_data(), type.referenced_data());
}

// //
// // Registrations for Data functions
// //
//
// SEPT__REGISTER__PRINT(RefTerm_c)
//
// SEPT__REGISTER__EQ(RefTerm_c)
//
// // TODO
// // SEPT__REGISTER__COMPARE(RefTerm_c, RefTerm_c)
//
// SEPT__REGISTER__INHABITS__NONDATA(RefTerm_c, RefTerm_c)
//
// // TODO
// // SEPT__REGISTER__SERIALIZE(RefTerm_c)
//
// SEPT__REGISTER__ELEMENT_OF__NONDATA(RefTerm_c, int8_t)
// SEPT__REGISTER__ELEMENT_OF__NONDATA(RefTerm_c, int16_t)
// SEPT__REGISTER__ELEMENT_OF__NONDATA(RefTerm_c, int32_t)
// SEPT__REGISTER__ELEMENT_OF__NONDATA(RefTerm_c, int64_t)
// SEPT__REGISTER__ELEMENT_OF__NONDATA(RefTerm_c, uint8_t)
// SEPT__REGISTER__ELEMENT_OF__NONDATA(RefTerm_c, uint16_t)
// SEPT__REGISTER__ELEMENT_OF__NONDATA(RefTerm_c, uint32_t)
// SEPT__REGISTER__ELEMENT_OF__NONDATA(RefTerm_c, uint64_t)
//
// SEPT__REGISTER__CONSTRUCT_INHABITANT_OF(RefTerm_c, RefTerm_c)

} // end namespace sept
