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
        "Union",                        // 0x2C  44
        "Intersection",                 // 0x2D  45
        "Negation",                     // 0x2E  46
        "Difference",                   // 0x2F  47
        "ArrayType",                    // 0x30  48
        "ArrayES",                      // 0x31  49
        "ArrayE",                       // 0x32  50
        "ArrayS",                       // 0x33  51
        "Array",                        // 0x34  52
        "OrderedMapType",               // 0x35  53
        "OrderedMapES",                 // 0x36  54
        "OrderedMapE",                  // 0x37  55
        "OrderedMapS",                  // 0x38  56
        "OrderedMap",                   // 0x39  57
        "PlaceholderType",              // 0x3A  58
        "Placeholder",                  // 0x3B  59
        "OutputType",                   // 0x3C  60
        "Output",                       // 0x3D  61
        "ClearOutputType",              // 0x3E  62
        "ClearOutput",                  // 0x3F  63
        "EndOfFileType",                // 0x40  64
        "EndOfFile",                    // 0x41  65
        "RequestSyncInputType",         // 0x42  66
        "RequestSyncInput",             // 0x43  67
    };
    return TABLE.at(size_t(t));
}

NonParametricTerm_c NonParametricTerm;
Void_c Void;
True_c True;
False_c False;

} // end namespace sept
