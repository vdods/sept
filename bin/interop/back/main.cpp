#include <fstream>
#include <iostream>
#include "sept/ArrayTerm.hpp"
#include "sept/ArrayType.hpp"
#include "sept/ctl/ClearOutput.hpp"
#include "sept/ctl/Output.hpp"
#include "sept/ctl/RequestSyncInput.hpp"
#include "sept/NPTerm.hpp"
#include "sept/NPType.hpp"
#include "sept/OrderedMapTerm.hpp"
#include "sept/OrderedMapType.hpp"

int do_stuff (std::istream &in, std::ostream &out, std::ostream &err) {
    sept::serialize(sept::True, out);
    sept::serialize(sept::False, out);
    sept::serialize(sept::EmptyType, out);
    sept::serialize(sept::Term, out);
    sept::serialize(sept::NonParametricType, out);
    sept::serialize(sept::ArrayES(sept::Array,3)(sept::Array(), sept::Array(true, 3, 4.4), sept::Array(sept::Term)), out);
    sept::serialize(sept::ArrayS(2)(int16_t(888), int16_t(20202)), out);
    sept::serialize(sept::ArrayS(4)(sept::Void, sept::Void, sept::Void, sept::Void), out);
    sept::serialize(sept::OrderedMapDC(sept::VoidType,sept::Term)(std::pair(sept::Void,56.89)), out);
    sept::serialize(
        sept::OrderedMapDC(sept::Array,sept::Uint64)(
            std::pair(sept::Array(false,true,false),uint64_t(3)),
            std::pair(sept::Array(true,true,true,false,false),uint64_t(5))
        ),
        out
    );
    sept::serialize(sept::ctl::Output(sept::Array(50,60,70.01,true,sept::True,sept::VoidType)), out);
    sept::serialize(sept::ctl::Output(sept::EmptyType), out);
    sept::serialize(sept::ctl::Output(sept::OrderedMapDC(sept::Sint32,sept::Float32)), out);
    sept::serialize(sept::ctl::ClearOutput, out);
    sept::serialize(sept::ctl::Output(sept::Array('e', 'n', 't', 'e', 'r', ' ', 'i', 'n', 't')), out);
    sept::serialize(sept::ctl::Output(sept::Sint32), out);
//     err << "back: about to flush...\n";
    out.flush();
//     err << "back: done with flush\n";

    // Request some input, then read the response.
    sept::serialize(sept::ctl::RequestSyncInput(sept::Sint32), out);
//     err << "back: about to flush...\n";
    out.flush();
//     err << "back: done with flush\n";

//     err << "back: requesting response ...\n";
    auto response = sept::deserialize_data(in);
//     err << "back: response was " << response << ", echoing within an Output ...\n";
    sept::serialize(sept::ctl::Output(response), out);
//     err << "back: done echoing\n";
    // These are technically unnecessary because we're about to exit, so they'll be flushed anyway.
    err.flush();
    out.flush();

    return 0;
}

int main (int argc, char **argv) {
    std::cerr << "back: " << LVD_REFLECT(argc) << '\n';
    if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [log-file]\n";
        return -1;
    }
    std::ofstream err_;
    if (argc == 2) {
        std::cerr << "logging to file \"" << argv[1] << "\"\n";
        err_.open(argv[1]);
    }
    std::ostream &err = argc == 2 ? err_ : std::cerr;
    auto retval = do_stuff(std::cin, std::cout, err);
    err << "back: returning with " << retval << '\n';
    return retval;
}
