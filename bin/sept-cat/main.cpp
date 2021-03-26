#include <iostream>
#include "sept/ctl/EndOfFile.hpp"

// TODO: Make two modes -- one where it reads in serialized data and prints human-readable,
// the other where it reads in ascii strings and outputs serialized ArrayE(uint8_t) terms,
// or eventually perhaps parse text-based syntax for all the ADTs, and serialize those.
int main (int argc, char **argv) {
    while (true) {
        auto value = sept::deserialize_data(std::cin);
        if (value == sept::ctl::EndOfFile)
            return 0;
        std::cout << value << '\n';
    }
    return 0;
}
