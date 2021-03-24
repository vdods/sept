// 2020.03.13 - Victor Dods

#include "sept/Data.hpp"

#include <boost/core/demangle.hpp>
#include <ios>
#include "sept/ArrayTerm.hpp"
#include "sept/ArrayType.hpp"
#include "sept/ctl/ClearOutput.hpp"
#include "sept/ctl/EndOfFile.hpp"
#include "sept/ctl/Output.hpp"
#include "sept/ctl/RequestSyncInput.hpp"
#include "sept/hash.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"
#include "sept/OrderedMapTerm.hpp"
#include "sept/OrderedMapType.hpp"
#include "sept/Placeholder.hpp"
#include <sstream> // Needed by LVD_FMT

namespace sept {

Data::operator lvd::OstreamDelegate () const {
    assert(this->has_value());
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        assert(this->has_value());

        #define SEPT_HANDLE_TYPE_BY_VALUE(T) else if (this->type() == typeid(T)) out << this->cast<T>();
        #define SEPT_HANDLE_TYPE_BY_CREF(T) else if (this->type() == typeid(T)) out << this->cast<T const &>();

//         out << "Data(";
        if (false) { }
        SEPT_HANDLE_TYPE_BY_VALUE(int8_t)
        SEPT_HANDLE_TYPE_BY_VALUE(int16_t)
        SEPT_HANDLE_TYPE_BY_VALUE(int32_t)
        SEPT_HANDLE_TYPE_BY_VALUE(int64_t)
        SEPT_HANDLE_TYPE_BY_VALUE(uint8_t)
        SEPT_HANDLE_TYPE_BY_VALUE(uint16_t)
        SEPT_HANDLE_TYPE_BY_VALUE(uint32_t)
        SEPT_HANDLE_TYPE_BY_VALUE(uint64_t)
        SEPT_HANDLE_TYPE_BY_VALUE(float)
        SEPT_HANDLE_TYPE_BY_VALUE(double)
        SEPT_HANDLE_TYPE_BY_VALUE(char)
        SEPT_HANDLE_TYPE_BY_VALUE(bool)
        // TODO: Some of these correspond to non-parametric terms (i.e. singletons), which
        // could be handled by value.
        SEPT_HANDLE_TYPE_BY_CREF(Term_c)
        SEPT_HANDLE_TYPE_BY_CREF(NonParametricTerm_c)
//         SEPT_HANDLE_TYPE_BY_CREF(ParametricTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(NonType_c)
        SEPT_HANDLE_TYPE_BY_CREF(NonParametricType_c)
//         SEPT_HANDLE_TYPE_BY_CREF(ParametricType_c)
        SEPT_HANDLE_TYPE_BY_CREF(EmptyType_c)
        SEPT_HANDLE_TYPE_BY_CREF(VoidType_c)
        SEPT_HANDLE_TYPE_BY_CREF(Void_c)
        SEPT_HANDLE_TYPE_BY_CREF(TrueType_c)
        SEPT_HANDLE_TYPE_BY_CREF(True_c)
        SEPT_HANDLE_TYPE_BY_CREF(FalseType_c)
        SEPT_HANDLE_TYPE_BY_CREF(False_c)
        SEPT_HANDLE_TYPE_BY_CREF(BoolType_c)
        SEPT_HANDLE_TYPE_BY_CREF(Bool_c)
        SEPT_HANDLE_TYPE_BY_CREF(Sint8Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Sint8_c)
        SEPT_HANDLE_TYPE_BY_CREF(Sint16Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Sint16_c)
        SEPT_HANDLE_TYPE_BY_CREF(Sint32Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Sint32_c)
        SEPT_HANDLE_TYPE_BY_CREF(Sint64Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Sint64_c)
        SEPT_HANDLE_TYPE_BY_CREF(Uint8Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Uint8_c)
        SEPT_HANDLE_TYPE_BY_CREF(Uint16Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Uint16_c)
        SEPT_HANDLE_TYPE_BY_CREF(Uint32Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Uint32_c)
        SEPT_HANDLE_TYPE_BY_CREF(Uint64Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Uint64_c)
        SEPT_HANDLE_TYPE_BY_CREF(Float32Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Float32_c)
        SEPT_HANDLE_TYPE_BY_CREF(Float64Type_c)
        SEPT_HANDLE_TYPE_BY_CREF(Float64_c)
        SEPT_HANDLE_TYPE_BY_CREF(ArrayESTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(ArrayETerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(ArraySTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(ArrayTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(ArrayES_c)
        SEPT_HANDLE_TYPE_BY_CREF(ArrayE_c)
        SEPT_HANDLE_TYPE_BY_CREF(ArrayS_c)
        SEPT_HANDLE_TYPE_BY_CREF(Array_c)
        SEPT_HANDLE_TYPE_BY_CREF(ArrayType_c)
        SEPT_HANDLE_TYPE_BY_CREF(OrderedMapDCTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(OrderedMapDTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(OrderedMapCTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(OrderedMapTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(OrderedMapDC_c)
        SEPT_HANDLE_TYPE_BY_CREF(OrderedMapD_c)
        SEPT_HANDLE_TYPE_BY_CREF(OrderedMapC_c)
        SEPT_HANDLE_TYPE_BY_CREF(OrderedMap_c)
        SEPT_HANDLE_TYPE_BY_CREF(OrderedMapType_c)
        SEPT_HANDLE_TYPE_BY_CREF(PlaceholderType_c)
        SEPT_HANDLE_TYPE_BY_CREF(Placeholder_c)
        SEPT_HANDLE_TYPE_BY_CREF(PlaceholderTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::OutputType_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::Output_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::OutputTerm_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::ClearOutputType_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::ClearOutput_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::EndOfFileType_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::EndOfFile_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::RequestSyncInputType_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::RequestSyncInput_c)
        SEPT_HANDLE_TYPE_BY_CREF(ctl::RequestSyncInputTerm_c)
        else out << "Data[type = \"" << boost::core::demangle(this->type().name()) << "\"]";
//         else out << "type=" << this->type().name();
//         out << ')';

        #undef SEPT_HANDLE_TYPE_BY_VALUE
        #undef SEPT_HANDLE_TYPE_BY_CREF
    });
}

bool is_member (Data const &value, Data const &container) {
//     assert(value.has_value());
    assert(container.has_value());

    #define SEPT_HANDLE_TYPE_BY_VALUE(T) else if (container.type() == typeid(T)) return is_member(value, container.cast<T>());
    #define SEPT_HANDLE_TYPE_BY_CREF(T) else if (container.type() == typeid(T)) return is_member(value, container.cast<T const &>());

    if (false) { }
    SEPT_HANDLE_TYPE_BY_CREF(Term_c)
    SEPT_HANDLE_TYPE_BY_CREF(NonParametricTerm_c)
//     SEPT_HANDLE_TYPE_BY_CREF(ParametricTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(NonType_c)
    SEPT_HANDLE_TYPE_BY_CREF(NonParametricType_c)
//     SEPT_HANDLE_TYPE_BY_CREF(ParametricType_c)
    SEPT_HANDLE_TYPE_BY_CREF(EmptyType_c)
    SEPT_HANDLE_TYPE_BY_CREF(VoidType_c)
    SEPT_HANDLE_TYPE_BY_CREF(Void_c)
    SEPT_HANDLE_TYPE_BY_CREF(TrueType_c)
    SEPT_HANDLE_TYPE_BY_CREF(True_c)
    SEPT_HANDLE_TYPE_BY_CREF(FalseType_c)
    SEPT_HANDLE_TYPE_BY_CREF(False_c)
    SEPT_HANDLE_TYPE_BY_CREF(BoolType_c)
    SEPT_HANDLE_TYPE_BY_CREF(Bool_c)
    SEPT_HANDLE_TYPE_BY_CREF(Sint8Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Sint8_c)
    SEPT_HANDLE_TYPE_BY_CREF(Sint16Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Sint16_c)
    SEPT_HANDLE_TYPE_BY_CREF(Sint32Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Sint32_c)
    SEPT_HANDLE_TYPE_BY_CREF(Sint64Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Sint64_c)
    SEPT_HANDLE_TYPE_BY_CREF(Uint8Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Uint8_c)
    SEPT_HANDLE_TYPE_BY_CREF(Uint16Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Uint16_c)
    SEPT_HANDLE_TYPE_BY_CREF(Uint32Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Uint32_c)
    SEPT_HANDLE_TYPE_BY_CREF(Uint64Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Uint64_c)
    SEPT_HANDLE_TYPE_BY_CREF(Float32Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Float32_c)
    SEPT_HANDLE_TYPE_BY_CREF(Float64Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(Float64_c)
    SEPT_HANDLE_TYPE_BY_CREF(Array_c)
    SEPT_HANDLE_TYPE_BY_CREF(ArrayESTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(ArrayETerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(ArraySTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(ArrayTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(OrderedMap_c)
    SEPT_HANDLE_TYPE_BY_CREF(OrderedMapDCTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(OrderedMapDTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(OrderedMapCTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(OrderedMapTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(PlaceholderType_c)
    SEPT_HANDLE_TYPE_BY_CREF(Placeholder_c)
    SEPT_HANDLE_TYPE_BY_CREF(PlaceholderTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::OutputType_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::Output_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::OutputTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::ClearOutputType_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::ClearOutput_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::EndOfFileType_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::EndOfFile_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::RequestSyncInputType_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::RequestSyncInput_c)
    SEPT_HANDLE_TYPE_BY_CREF(ctl::RequestSyncInputTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(BaseArray_t<>)
    SEPT_HANDLE_TYPE_BY_CREF(BaseArrayT_t<>)
    SEPT_HANDLE_TYPE_BY_CREF(BaseArray_S_t<1>)

    #undef SEPT_HANDLE_TYPE_BY_VALUE
    #undef SEPT_HANDLE_TYPE_BY_CREF

    throw std::runtime_error(LVD_FMT("is_member not implemented yet for " << container.type()));
}

Data element_type_of (Data const &array_type) {
    assert(array_type.has_value());

    if (array_type.type() == typeid(ArrayESTerm_c)) return element_type_of(array_type.cast<ArrayESTerm_c const &>());
    else if (array_type.type() == typeid(ArrayETerm_c)) return element_type_of(array_type.cast<ArrayETerm_c const &>());
    else if (array_type.type() == typeid(ArraySTerm_c)) return element_type_of(array_type.cast<ArraySTerm_c const &>());
    else if (array_type.type() == typeid(Array_c)) return element_type_of(array_type.cast<Array_c const &>());
    else throw std::runtime_error(LVD_FMT("element_type_of not implemented for " << array_type.type()));
}

Data element_type_of (Data &&array_type) {
    assert(array_type.has_value());

    if (array_type.type() == typeid(ArrayESTerm_c)) return element_type_of(std::move(array_type.cast<ArrayESTerm_c &>()));
    else if (array_type.type() == typeid(ArrayETerm_c)) return element_type_of(std::move(array_type.cast<ArrayETerm_c &>()));
    else if (array_type.type() == typeid(ArraySTerm_c)) return element_type_of(std::move(array_type.cast<ArraySTerm_c &>()));
    else if (array_type.type() == typeid(Array_c)) return element_type_of(std::move(array_type.cast<Array_c &>()));
    else throw std::runtime_error(LVD_FMT("element_type_of not implemented for " << array_type.type()));
}

Data domain_of (Data const &ordered_map_type) {
    assert(ordered_map_type.has_value());

    if (ordered_map_type.type() == typeid(OrderedMapDCTerm_c)) return domain_of(ordered_map_type.cast<OrderedMapDCTerm_c const &>());
    else if (ordered_map_type.type() == typeid(OrderedMapDTerm_c)) return domain_of(ordered_map_type.cast<OrderedMapDTerm_c const &>());
    else if (ordered_map_type.type() == typeid(OrderedMapCTerm_c)) return domain_of(ordered_map_type.cast<OrderedMapCTerm_c const &>());
    else if (ordered_map_type.type() == typeid(OrderedMap_c)) return domain_of(ordered_map_type.cast<OrderedMap_c const &>());
    else throw std::runtime_error(LVD_FMT("domain_of not implemented for " << ordered_map_type.type()));
}

Data domain_of (Data &&ordered_map_type) {
    assert(ordered_map_type.has_value());

    if (ordered_map_type.type() == typeid(OrderedMapDCTerm_c)) return domain_of(std::move(ordered_map_type.cast<OrderedMapDCTerm_c &>()));
    else if (ordered_map_type.type() == typeid(OrderedMapDTerm_c)) return domain_of(std::move(ordered_map_type.cast<OrderedMapDTerm_c &>()));
    else if (ordered_map_type.type() == typeid(OrderedMapCTerm_c)) return domain_of(std::move(ordered_map_type.cast<OrderedMapCTerm_c &>()));
    else if (ordered_map_type.type() == typeid(OrderedMap_c)) return domain_of(std::move(ordered_map_type.cast<OrderedMap_c &>()));
    else throw std::runtime_error(LVD_FMT("domain_of not implemented for " << ordered_map_type.type()));
}

Data codomain_of (Data const &ordered_map_type) {
    assert(ordered_map_type.has_value());

    if (ordered_map_type.type() == typeid(OrderedMapDCTerm_c)) return codomain_of(ordered_map_type.cast<OrderedMapDCTerm_c const &>());
    else if (ordered_map_type.type() == typeid(OrderedMapDTerm_c)) return codomain_of(ordered_map_type.cast<OrderedMapDTerm_c const &>());
    else if (ordered_map_type.type() == typeid(OrderedMapCTerm_c)) return codomain_of(ordered_map_type.cast<OrderedMapCTerm_c const &>());
    else if (ordered_map_type.type() == typeid(OrderedMap_c)) return codomain_of(ordered_map_type.cast<OrderedMap_c const &>());
    else throw std::runtime_error(LVD_FMT("codomain_of not implemented for " << ordered_map_type.type()));
}

Data codomain_of (Data &&ordered_map_type) {
    assert(ordered_map_type.has_value());

    if (ordered_map_type.type() == typeid(OrderedMapDCTerm_c)) return codomain_of(std::move(ordered_map_type.cast<OrderedMapDCTerm_c &>()));
    else if (ordered_map_type.type() == typeid(OrderedMapDTerm_c)) return codomain_of(std::move(ordered_map_type.cast<OrderedMapDTerm_c &>()));
    else if (ordered_map_type.type() == typeid(OrderedMapCTerm_c)) return codomain_of(std::move(ordered_map_type.cast<OrderedMapCTerm_c &>()));
    else if (ordered_map_type.type() == typeid(OrderedMap_c)) return codomain_of(std::move(ordered_map_type.cast<OrderedMap_c &>()));
    else throw std::runtime_error(LVD_FMT("codomain_of not implemented for " << ordered_map_type.type()));
}

//
// Registrations for Data functions
//

SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(bool)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(char)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(int8_t)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(int16_t)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(int32_t)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(int64_t)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(uint8_t)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(uint16_t)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(uint32_t)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(uint64_t)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(float)
SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE(double)
// SEPT_DATA_OPERATOR_EQ_REGISTER_TYPE_EXPLICIT(std::string, std__string)

} // end namespace sept

namespace std {

size_t std::hash<sept::Data>::operator () (sept::Data const &data) const {
    assert(data.has_value());

    #define SEPT_HANDLE_TYPE_BY_VALUE(T) else if (data.type() == typeid(T)) return sept::hash(data.cast<T>());
    #define SEPT_HANDLE_TYPE_BY_CREF(T) else if (data.type() == typeid(T)) return sept::hash(data.cast<T const &>());

    if (false) { }
    SEPT_HANDLE_TYPE_BY_VALUE(int8_t)
    SEPT_HANDLE_TYPE_BY_VALUE(int16_t)
    SEPT_HANDLE_TYPE_BY_VALUE(int32_t)
    SEPT_HANDLE_TYPE_BY_VALUE(int64_t)
    SEPT_HANDLE_TYPE_BY_VALUE(uint8_t)
    SEPT_HANDLE_TYPE_BY_VALUE(uint16_t)
    SEPT_HANDLE_TYPE_BY_VALUE(uint32_t)
    SEPT_HANDLE_TYPE_BY_VALUE(uint64_t)
    SEPT_HANDLE_TYPE_BY_VALUE(float)
    SEPT_HANDLE_TYPE_BY_VALUE(double)
    SEPT_HANDLE_TYPE_BY_VALUE(char)
    SEPT_HANDLE_TYPE_BY_VALUE(bool)
    // TODO: Some of these correspond to non-parametric terms (i.e. singletons), which
    // could be handled by value.
    SEPT_HANDLE_TYPE_BY_CREF(sept::Term_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::NonParametricTerm_c)
//     SEPT_HANDLE_TYPE_BY_CREF(sept::ParametricTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::NonType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::NonParametricType_c)
//     SEPT_HANDLE_TYPE_BY_CREF(sept::ParametricType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::EmptyType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::VoidType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Void_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::TrueType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::True_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::FalseType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::False_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::BoolType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Bool_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Sint8Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Sint8_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Sint16Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Sint16_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Sint32Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Sint32_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Sint64Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Sint64_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Uint8Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Uint8_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Uint16Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Uint16_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Uint32Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Uint32_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Uint64Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Uint64_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Float32Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Float32_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Float64Type_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Float64_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ArrayESTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ArrayETerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ArraySTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ArrayTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ArrayES_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ArrayE_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ArrayS_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Array_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ArrayType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMapDCTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMapDTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMapCTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMapTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMapType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMapDC_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMapD_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMapC_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMap_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::OrderedMapType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::PlaceholderType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::Placeholder_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::OutputType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::Output_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::OutputTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::ClearOutputType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::ClearOutput_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::EndOfFileType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::EndOfFile_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::RequestSyncInputType_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::RequestSyncInput_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::ctl::RequestSyncInputTerm_c)
    SEPT_HANDLE_TYPE_BY_CREF(sept::BaseArray_t<>)
    SEPT_HANDLE_TYPE_BY_CREF(sept::BaseArrayT_t<>)
    SEPT_HANDLE_TYPE_BY_CREF(sept::BaseArray_S_t<1>)

    throw std::runtime_error(LVD_FMT("Data type " << data.type().name() << " not (yet?) supported in std::hash"));

    #undef SEPT_HANDLE_TYPE_BY_VALUE
    #undef SEPT_HANDLE_TYPE_BY_CREF
}

} // end namespace std
