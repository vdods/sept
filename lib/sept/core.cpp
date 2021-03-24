// 2020.04.05 - Victor Dods

#include "sept/core.hpp"

#include <boost/core/demangle.hpp>
#include <typeinfo>

namespace std {

ostream &operator << (ostream &out, type_info const &ti) {
    return out << "std::type_info[name = \"" << boost::core::demangle(ti.name()) << "\"]";
}

} // end namespace std

namespace sept {

} // end namespace sept
