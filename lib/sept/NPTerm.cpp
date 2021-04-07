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
        "FormalTypeOf",                 // 0x0E  14
        "Bool",                         // 0x0F  15
        "Sint8",                        // 0x10  16
        "Sint16",                       // 0x11  17
        "Sint32",                       // 0x12  18
        "Sint64",                       // 0x13  19
        "Uint8",                        // 0x14  20
        "Uint16",                       // 0x15  21
        "Uint32",                       // 0x16  22
        "Uint64",                       // 0x17  23
        "Float32",                      // 0x18  24
        "Float64",                      // 0x19  25
        "BoolType",                     // 0x1A  26
        "Sint8Type",                    // 0x1B  27
        "Sint16Type",                   // 0x1C  28
        "Sint32Type",                   // 0x1D  29
        "Sint64Type",                   // 0x1E  30
        "Uint8Type",                    // 0x1F  31
        "Uint16Type",                   // 0x20  32
        "Uint32Type",                   // 0x21  33
        "Uint64Type",                   // 0x22  34
        "Float32Type",                  // 0x23  35
        "Float64Type",                  // 0x24  36
        "SintType",                     // 0x25  37
        "Sint",                         // 0x26  38
        "UintType",                     // 0x27  39
        "Uint",                         // 0x28  40
        "FloatType",                    // 0x29  41
        "Float",                        // 0x2A  42
        "PODType",                      // 0x2B  43
        "POD",                          // 0x2C  44
        "UnionType",                    // 0x2D  45
        "Union",                        // 0x2E  46
        "Intersection",                 // 0x2F  47
        "Negation",                     // 0x30  48
        "Difference",                   // 0x31  49
        "ArrayType",                    // 0x32  50
        "ArrayES",                      // 0x33  51
        "ArrayE",                       // 0x34  52
        "ArrayS",                       // 0x35  53
        "Array",                        // 0x36  54
        "OrderedMapType",               // 0x37  55
        "OrderedMapES",                 // 0x38  56
        "OrderedMapE",                  // 0x39  57
        "OrderedMapS",                  // 0x3A  58
        "OrderedMap",                   // 0x3B  59
        "TupleType",                    // 0x3C  60
        "Tuple",                        // 0x3D  61
        "MemRefType",                   // 0x3E  62
        "MemRef",                       // 0x3F  63
        "GlobalSymRefType",             // 0x40  64
        "GlobalSymRef",                 // 0x41  65
        "PlaceholderType",              // 0x42  66
        "Placeholder",                  // 0x43  67
        "OutputType",                   // 0x44  68
        "Output",                       // 0x45  69
        "ClearOutputType",              // 0x46  70
        "ClearOutput",                  // 0x47  71
        "EndOfFileType",                // 0x48  72
        "EndOfFile",                    // 0x49  73
        "RequestSyncInputType",         // 0x4A  74
        "RequestSyncInput",             // 0x4B  75
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
