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

Data abstract_type_of_data (Data const &value_data) {
    // Look up the type in the evaluator map.
    auto const &evaluator_map = lvd::static_association_singleton<sept::AbstractTypeOf>();
    auto it = evaluator_map.find(std::type_index(value_data.type()));
    if (it == evaluator_map.end())
        return Term; // Fallback is Term -- everything is a Term.

    return it->second(value_data);
}

bool inhabits_data (Data const &value_data, Data const &type_data) {
    // Everything is a Term
    if (type_data.type() == typeid(Term_c))
        return true;

    // Look up the type pair in the predicate map.
    auto const &predicate_map = lvd::static_association_singleton<sept::InhabitsData>();
    auto it = predicate_map.find(TypeIndexPair{std::type_index(value_data.type()), std::type_index(type_data.type())});
    // If the (value,type) type pair isn't found, then it's assumed that value does not inhabit type.
    if (it == predicate_map.end())
        return false;

    auto const &predicate = it->second;
    // If predicate == nullptr, then by convention, value always inhabits type, and it doesn't depend on any runtime value.
    if (predicate == nullptr)
        return true;

    // Otherwise delegate to predicate.
    return predicate(value_data, type_data);
}

int compare_data (Data const &lhs, Data const &rhs) {
    // Look up the type pair in the evaluator map.
    auto const &evaluator_map = lvd::static_association_singleton<sept::CompareData>();
    auto it = evaluator_map.find(TypeIndexPair{std::type_index(lhs.type()), std::type_index(rhs.type())});
    // If the (lhs,rhs) type pair isn't found, then it's assumed that lhs and rhs are incomparable.
    // TODO: For a total order, this is an error.  But for a partial order, this would just return
    // "incomparable".
    // NOTE TEMP HACK: For now, if the pair isn't found, then order them based on the type name pointers.
    // NOTE that this is implementation dependent.
    if (it == evaluator_map.end()) {
        auto const *lhs_type_name = lhs.type().name();
        auto const *rhs_type_name = rhs.type().name();
        if (lhs_type_name < rhs_type_name)
            return -1;
        else if (lhs_type_name == rhs_type_name)
            return 0;
        else
            return 1;
    }
//     // If the (lhs,rhs) type pair isn't found, then it's assumed that lhs and rhs are incomparable.
//     // TODO: For a total order, this is an error.  But for a partial order, this would just return
//     // "incomparable".
//     if (it == evaluator_map.end())
//         throw std::runtime_error(LVD_FMT("no compare evaluator registered for LHS type " << lhs.type() << " and RHS type " << rhs.type()));

    auto const &evaluator = it->second;
    // If evaluator == nullptr, then by convention, the values always compare as equal, and it doesn't depend on any runtime value.
    if (evaluator == nullptr)
        return 0;

    // Otherwise delegate to evaluator.
    return evaluator(lhs, rhs);
}

void serialize_data (Data const &value, std::ostream &out) {
    // Look up the type in the serialization function map.
    auto const &serialize_function_map = lvd::static_association_singleton<sept::SerializeData>();
    auto it = serialize_function_map.find(std::type_index(value.type()));
    if (it == serialize_function_map.end())
        throw std::runtime_error(LVD_FMT("no serialize function registered for type " << value.type()));

    // At this point we know the type of the data, and should serialize it, but the existing implementation
    // doesn't do that, which seems wrong.  TODO: Figure this out -- probably serialize the type here before
    // serializing the value.

    auto const &serialize_function = it->second;
    serialize_function(value, out);
}

//
// deserialize_data
//

// This assumes that in.peek() will return SerializedTopLevelCode::NON_PARAMETRIC_TERM.
// This function is essentially a switch statement on all possible NPTerm values.
Data deserialize_NonParametricTerm (std::istream &in) {
    auto stlc = SerializedTopLevelCode(in.get());
    assert(stlc == SerializedTopLevelCode::NON_PARAMETRIC_TERM && "pre-condition for this function was not satisfied");
    std::ignore = stlc;

    // TODO: Handle error cases
    auto np_term_enum = NPTerm(in.get());
    if (in.eof()) {
        return ctl::EndOfFile;
    }
    switch (np_term_enum) {
        case NPTerm::TERM: return Term;
        case NPTerm::NON_PARAMETRIC_TERM: return NonParametricTerm;
//         case NPTerm::PARAMETRIC_TERM: return ParametricTerm;

        case NPTerm::TYPE: return Type;
        case NPTerm::NON_TYPE: return NonType;
        case NPTerm::NON_PARAMETRIC_TYPE: return NonParametricType;
//         case NPTerm::PARAMETRIC_TYPE: return ParametricType;

        case NPTerm::VOID: return Void;
        case NPTerm::TRUE: return True;
        case NPTerm::FALSE: return False;

        case NPTerm::VOID_TYPE: return VoidType;
        case NPTerm::TRUE_TYPE: return TrueType;
        case NPTerm::FALSE_TYPE: return FalseType;
        case NPTerm::EMPTY_TYPE: return EmptyType;

        case NPTerm::BOOL: return Bool;
        case NPTerm::SINT8: return Sint8;
        case NPTerm::SINT16: return Sint16;
        case NPTerm::SINT32: return Sint32;
        case NPTerm::SINT64: return Sint64;
        case NPTerm::UINT8: return Uint8;
        case NPTerm::UINT16: return Uint16;
        case NPTerm::UINT32: return Uint32;
        case NPTerm::UINT64: return Uint64;
        case NPTerm::FLOAT32: return Float32;
        case NPTerm::FLOAT64: return Float64;

        case NPTerm::BOOL_TYPE: return BoolType;
        case NPTerm::SINT8_TYPE: return Sint8Type;
        case NPTerm::SINT16_TYPE: return Sint16Type;
        case NPTerm::SINT32_TYPE: return Sint32Type;
        case NPTerm::SINT64_TYPE: return Sint64Type;
        case NPTerm::UINT8_TYPE: return Uint8Type;
        case NPTerm::UINT16_TYPE: return Uint16Type;
        case NPTerm::UINT32_TYPE: return Uint32Type;
        case NPTerm::UINT64_TYPE: return Uint64Type;
        case NPTerm::FLOAT32_TYPE: return Float32Type;
        case NPTerm::FLOAT64_TYPE: return Float64Type;

        case NPTerm::ARRAY_TYPE: return ArrayType;
        case NPTerm::ARRAY_ES: return ArrayES;
        case NPTerm::ARRAY_E: return ArrayE;
        case NPTerm::ARRAY_S: return ArrayS;
        case NPTerm::ARRAY: return Array;

        case NPTerm::ORDERED_MAP_TYPE: return OrderedMapType;
        case NPTerm::ORDERED_MAP_DC: return OrderedMapDC;
        case NPTerm::ORDERED_MAP_D: return OrderedMapD;
        case NPTerm::ORDERED_MAP_C: return OrderedMapC;
        case NPTerm::ORDERED_MAP: return OrderedMap;

        case NPTerm::OUTPUT_TYPE: return ctl::OutputType;
        case NPTerm::OUTPUT: return ctl::Output;
        case NPTerm::CLEAR_OUTPUT_TYPE: return ctl::ClearOutputType;
        case NPTerm::CLEAR_OUTPUT: return ctl::ClearOutput;
        case NPTerm::END_OF_FILE_TYPE: return ctl::EndOfFileType;
        case NPTerm::END_OF_FILE: return ctl::EndOfFile;
        case NPTerm::REQUEST_SYNC_INPUT_TYPE: return ctl::RequestSyncInputType;
        case NPTerm::REQUEST_SYNC_INPUT: return ctl::RequestSyncInput;

        default: throw std::runtime_error(LVD_FMT("deserialize_NonParametricTerm; invalid NPTerm " << int(np_term_enum)));
    }
}

// This assumes that in.peek() will return SerializedTopLevelCode::PARAMETRIC_TERM.
Data deserialize_ParametricTerm (std::istream &in) {
    auto stlc = SerializedTopLevelCode(in.get());
    assert(!in.eof());
    assert(stlc == SerializedTopLevelCode::PARAMETRIC_TERM && "pre-condition for this function was not satisfied");
    std::ignore = stlc;

    Data abstract_type = deserialize_data(in);

    // Look up the type in the deserialization function map.
    auto const &deserialize_function_map = lvd::static_association_singleton<sept::DeserializeData>();
    auto it = deserialize_function_map.find(std::type_index(abstract_type.type()));
    if (it == deserialize_function_map.end())
        throw std::runtime_error(LVD_FMT("no deserialize function registered for type " << abstract_type.type()));

    auto const &deserialize_function = it->second;
    return deserialize_function(std::move(abstract_type), in);
}

Data deserialize_data (std::istream &in) {
    // TODO: Handle error cases
    auto stlc = SerializedTopLevelCode(in.peek());
    if (in.eof()) {
        return ctl::EndOfFile;
    }
    switch (stlc) {
        case SerializedTopLevelCode::NON_PARAMETRIC_TERM: return deserialize_NonParametricTerm(in);
        case SerializedTopLevelCode::PARAMETRIC_TERM: return deserialize_ParametricTerm(in);
        default: throw std::runtime_error(LVD_FMT("invalid SerializedTopLevelCode " << int(stlc)));
    }
}

//
// Other stuff
//

bool is_member (Data const &value, Data const &container) {
//     assert(value.has_value());
    assert(container.has_value());

//     #define SEPT_HANDLE_TYPE_BY_VALUE(T) else if (container.type() == typeid(T)) return is_member(value, container.cast<T>());
    #define SEPT_HANDLE_TYPE_BY_VALUE(T) else if (container.type() == typeid(T)) return is_member(value, container.cast<T const &>());
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

SEPT_DATA_PRINTING_REGISTER_TYPE(bool)
SEPT_DATA_PRINTING_REGISTER_TYPE(char)
SEPT_DATA_PRINTING_REGISTER_TYPE(int8_t)
SEPT_DATA_PRINTING_REGISTER_TYPE(int16_t)
SEPT_DATA_PRINTING_REGISTER_TYPE(int32_t)
SEPT_DATA_PRINTING_REGISTER_TYPE(int64_t)
SEPT_DATA_PRINTING_REGISTER_TYPE(uint8_t)
SEPT_DATA_PRINTING_REGISTER_TYPE(uint16_t)
SEPT_DATA_PRINTING_REGISTER_TYPE(uint32_t)
SEPT_DATA_PRINTING_REGISTER_TYPE(uint64_t)
SEPT_DATA_PRINTING_REGISTER_TYPE(float)
SEPT_DATA_PRINTING_REGISTER_TYPE(double)
SEPT_DATA_PRINTING_REGISTER_TYPE_EXPLICIT(std::string, std__string)

SEPT_EQ_DATA_REGISTER_TYPE(bool)
SEPT_EQ_DATA_REGISTER_TYPE(char)
SEPT_EQ_DATA_REGISTER_TYPE(int8_t)
SEPT_EQ_DATA_REGISTER_TYPE(int16_t)
SEPT_EQ_DATA_REGISTER_TYPE(int32_t)
SEPT_EQ_DATA_REGISTER_TYPE(int64_t)
SEPT_EQ_DATA_REGISTER_TYPE(uint8_t)
SEPT_EQ_DATA_REGISTER_TYPE(uint16_t)
SEPT_EQ_DATA_REGISTER_TYPE(uint32_t)
SEPT_EQ_DATA_REGISTER_TYPE(uint64_t)
SEPT_EQ_DATA_REGISTER_TYPE(float)
SEPT_EQ_DATA_REGISTER_TYPE(double)
SEPT_EQ_DATA_REGISTER_TYPE_EXPLICIT(std::string, std__string)

} // end namespace sept

namespace std {

size_t std::hash<sept::Data>::operator () (sept::Data const &data) const {
    assert(data.has_value());

//     #define SEPT_HANDLE_TYPE_BY_VALUE(T) else if (data.type() == typeid(T)) return sept::hash(data.cast<T>());
    #define SEPT_HANDLE_TYPE_BY_VALUE(T) else if (data.type() == typeid(T)) return sept::hash(data.cast<T const &>());
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
