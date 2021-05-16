// 2021.03.31 - Victor Dods

#include "sept/RefTerm.hpp"

#include "sept/Data.hpp"
#include "sept/NPTerm.hpp"

namespace sept {

Data RefTerm_c::move_referenced_data () && {
    LVD_ABORT("RefTerm_c::move_referenced_data has been disabled to see if it's actually necessary");
}

Data RefTerm_c::operator() (Data const &argument) const {
    return referenced_data()(argument);
}

Data RefTerm_c::operator[] (Data const &param) const {
    return referenced_data()[param];
}

RefTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

//
// Data model functions
//

bool operator== (RefTerm_c const &lhs, RefTerm_c const &rhs) {
//     lvd::g_log << lvd::Log::trc() << LVD_CALL_SITE() << lvd::IndentGuard() << '\n'
//                << LVD_REFLECT(&lhs.referenced_data()) << '\n'
//                << LVD_REFLECT(&rhs.referenced_data()) << '\n';
    // TODO: Because references can be stacked, this should/could check the innermost reference values for equality first.
    return lhs.referenced_data() == rhs.referenced_data();
}
// // NOTE: These aren't yet present in DataEq registration, since it isn't yet built to take asymmetric type equality
// bool operator== (RefTerm_c const &lhs, Data const &rhs) {
//     return lhs.referenced_data() == rhs;
// }
// bool operator== (Data const &lhs, RefTerm_c const &rhs) {
//     return lhs == rhs.referenced_data();
// }

void print (std::ostream &out, DataPrintCtx &ctx, RefTerm_c const &value) {
    out << "RefTerm_c(" << value.ref_base_get() << "; ";
    print_data(out, ctx, value.referenced_data());
    out << ')';
}

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
// SEPT__REGISTER__HASH(RefTerm_c)
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
