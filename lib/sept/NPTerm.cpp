// 2020.04.18 - Victor Dods

#include "sept/NPTerm.hpp"

#include <string>
#include <vector>

namespace sept {

std::string const &as_string (NPTerm t) {
    // Index values in comments for convenience.
    static std::vector<std::string> const TABLE{
        "Term",                         // 0x00   0
        "NonParametricTerm",            // 0x01   1
        "ParametricTerm",               // 0x02   2
        "Type",                         // 0x03   3
        "NonType",                      // 0x04   4
        "NonParametricType",            // 0x05   5
        "ParametricType",               // 0x06   6
        "Void",                         // 0x07   7
        "True",                         // 0x08   8
        "False",                        // 0x09   9
        "VoidType",                     // 0x0A  10
        "TrueType",                     // 0x0B  11
        "FalseType",                    // 0x0C  12
        "EmptyType",                    // 0x0D  13
        "Bool",                         // 0x0E  14
        "Sint8",                        // 0x0F  15
        "Sint16",                       // 0x10  16
        "Sint32",                       // 0x11  17
        "Sint64",                       // 0x12  18
        "Uint8",                        // 0x13  19
        "Uint16",                       // 0x14  20
        "Uint32",                       // 0x15  21
        "Uint64",                       // 0x16  22
        "Float32",                      // 0x17  23
        "Float64",                      // 0x18  24
        "BoolType",                     // 0x19  25
        "Sint8Type",                    // 0x1A  26
        "Sint16Type",                   // 0x1B  27
        "Sint32Type",                   // 0x1C  28
        "Sint64Type",                   // 0x1D  29
        "Uint8Type",                    // 0x1E  30
        "Uint16Type",                   // 0x1F  31
        "Uint32Type",                   // 0x20  32
        "Uint64Type",                   // 0x21  33
        "Float32Type",                  // 0x22  34
        "Float64Type",                  // 0x23  35
        "SintType",                     // 0x24  36
        "Sint",                         // 0x25  37
        "UintType",                     // 0x26  38
        "Uint",                         // 0x27  39
        "FloatType",                    // 0x28  40
        "Float",                        // 0x29  41
        "PODType",                      // 0x2A  42
        "POD",                          // 0x2B  43
        "UnionType",                    // 0x2C  44
        "Union",                        // 0x2D  45
        "Intersection",                 // 0x2E  46
        "Negation",                     // 0x2F  47
        "Difference",                   // 0x30  48
        "ArrayType",                    // 0x31  49
        "ArrayES",                      // 0x32  50
        "ArrayE",                       // 0x33  51
        "ArrayS",                       // 0x34  52
        "Array",                        // 0x35  53
        "OrderedMapType",               // 0x36  54
        "OrderedMapES",                 // 0x37  55
        "OrderedMapE",                  // 0x38  56
        "OrderedMapS",                  // 0x39  57
        "OrderedMap",                   // 0x3A  58
        "TupleType",                    // 0x3B  59
        "Tuple",                        // 0x3C  60
        "PlaceholderType",              // 0x3D  61
        "Placeholder",                  // 0x3E  62
        "OutputType",                   // 0x3F  63
        "Output",                       // 0x40  64
        "ClearOutputType",              // 0x41  65
        "ClearOutput",                  // 0x42  66
        "EndOfFileType",                // 0x43  67
        "EndOfFile",                    // 0x44  68
        "RequestSyncInputType",         // 0x45  69
        "RequestSyncInput",             // 0x46  70
    };
    return TABLE.at(size_t(t));
}

NonParametricTerm_c NonParametricTerm;
Void_c Void;
True_c True;
False_c False;

//
// Registrations for Data functions
//

SEPT__REGISTER__PRINT(NonParametricTerm_c)
SEPT__REGISTER__PRINT(Void_c)
SEPT__REGISTER__PRINT(BoolTerm_c)
SEPT__REGISTER__PRINT(True_c)
SEPT__REGISTER__PRINT(False_c)

SEPT__REGISTER__EQ(NonParametricTerm_c)
SEPT__REGISTER__EQ(Void_c)
SEPT__REGISTER__EQ(BoolTerm_c)
SEPT__REGISTER__EQ(True_c)
SEPT__REGISTER__EQ(False_c)

SEPT__REGISTER__COMPARE__SINGLETON(NonParametricTerm_c)
SEPT__REGISTER__COMPARE__SINGLETON(Void_c)
SEPT__REGISTER__COMPARE__SINGLETON(True_c)
SEPT__REGISTER__COMPARE__SINGLETON(False_c)
SEPT__REGISTER__COMPARE(True_c, BoolTerm_c)
SEPT__REGISTER__COMPARE(False_c, BoolTerm_c)
SEPT__REGISTER__COMPARE(BoolTerm_c, BoolTerm_c)

SEPT__REGISTER__SERIALIZE(NonParametricTerm_c)
SEPT__REGISTER__SERIALIZE(Void_c)
// SEPT__REGISTER__SERIALIZE(BoolTerm_c)
SEPT__REGISTER__SERIALIZE(True_c)
SEPT__REGISTER__SERIALIZE(False_c)

} // end namespace sept
