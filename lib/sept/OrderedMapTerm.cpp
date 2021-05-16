// 2020.07.15 - Victor Dods

#include "sept/OrderedMapTerm.hpp"

#include "sept/OrderedMapType.hpp"
#include <sstream> // Needed by LVD_FMT

namespace sept {

OrderedMapConstraint::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

std::ostream &operator << (std::ostream &out, DataOrderedMap const &m) {
    out << '{';
    size_t i = 0;
    for (auto const &pair : m) {
        out << pair.first << ':' << pair.second;
        if (i+1 < m.size())
            out << ", ";
        ++i;
    }
    return out << '}';
}

void OrderedMapConstraint::verify_constraint_or_throw (DataOrderedMap const &m) const {
    if (m_ordered_map_type.type() == typeid(OrderedMapDCTerm_c)) {
        m_ordered_map_type.cast<OrderedMapDCTerm_c const &>().verify_constraint_or_throw(m);
    } else if (m_ordered_map_type.type() == typeid(OrderedMapDTerm_c)) {
        m_ordered_map_type.cast<OrderedMapDTerm_c const &>().verify_constraint_or_throw(m);
    } else if (m_ordered_map_type.type() == typeid(OrderedMapCTerm_c)) {
        m_ordered_map_type.cast<OrderedMapCTerm_c const &>().verify_constraint_or_throw(m);
    } else if (m_ordered_map_type.type() == typeid(OrderedMap_c)) {
        // No constraint to enforce.
    } else {
        throw std::runtime_error(LVD_FMT("invalid array type " << m_ordered_map_type.type()));
    }
}

OrderedMapTerm_c::operator lvd::OstreamDelegate () const {
    return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
        DataPrintCtx ctx;
        print(out, ctx, *this);
    });
}

void serialize (OrderedMapTerm_c const &v, std::ostream &out) {
    serialize(SerializedTopLevelCode::PARAMETRIC_TERM, out);
    serialize_data(v.ordered_map_type(), out);
    serialize(v.size(), out);
    for (auto const &pair : v.pairs()) {
        serialize_data(pair.first, out);
        serialize_data(pair.second, out);
    }
}

OrderedMapTerm_c deserialize_value_OrderedMapTerm (Data &&abstract_type, std::istream &in) {
    auto size = deserialize_data(in);
    // TODO: Make this uint64_t instead, so it's not architecture dependent.
    if (size.type() != typeid(size_t))
        throw std::runtime_error(LVD_FMT("deserialize_value_OrderedMapTerm expected array size to be size_t, but it was " << size.type()));

    DataOrderedMap pairs;
    for (size_t i = 0; i < size.cast<size_t>(); ++i) {
        // These can't be done in-line in the construction of std::pair because C++ doesn't
        // guarantee that the arguments will be evaluated in the expected, left-to-right, order.
        auto key = deserialize_data(in);
        auto value = deserialize_data(in);
        pairs.emplace(std::pair(std::move(key), std::move(value)));
    }

    return OrderedMapTerm_c(std::move(pairs)).with_constraint(std::move(abstract_type));
}

bool is_member_key (Data const &value, OrderedMapTerm_c const &container) {
    return container.pairs().find(value) != container.pairs().end();
}

int compare (OrderedMapTerm_c const &lhs, OrderedMapTerm_c const &rhs) {
    for (auto lhs_it = lhs.begin(), rhs_it = rhs.begin(); lhs_it != lhs.end() && rhs_it != rhs.end(); ++lhs_it, ++rhs_it) {
        auto c = compare_data(lhs_it->first, rhs_it->first);
        if (c != 0)
            return c;
        c = compare_data(lhs_it->second, rhs_it->second);
        if (c != 0)
            return c;
    }
    // If we got this far, then they match on their common length, so the shorter one is "less".
    return lhs.size() < rhs.size() ? -1 : (lhs.size() == rhs.size() ? 0 : 1);
}

// This has to accept Data, because the domain of the map isn't known at compile time.
Data element_of (OrderedMapTerm_c const &m, Data const &key) { return m[key]; }

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(OrderedMapTerm_c)

SEPT__REGISTER__HASH(OrderedMapTerm_c)

SEPT__REGISTER__EQ(OrderedMapTerm_c)

SEPT__REGISTER__COMPARE(OrderedMapTerm_c, OrderedMapTerm_c)

SEPT__REGISTER__SERIALIZE(OrderedMapTerm_c)

SEPT__REGISTER__ELEMENT_OF__DATA(OrderedMapTerm_c, Data)

} // end namespace sept
