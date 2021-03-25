#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <fstream>
#include <iostream>
#include <lvd/Pipe.hpp>
#include <random>
#include "sept/ArrayTerm.hpp"
#include "sept/abstract_type_of.hpp"
#include "sept/ctl/ClearOutput.hpp"
#include "sept/ctl/EndOfFile.hpp"
#include "sept/ctl/Output.hpp"
#include "sept/ctl/RequestSyncInput.hpp"
#include "sept/deserialize.hpp"
#include <system_error>

sept::ArrayTerm_c Array_from_string (std::string const &s) {
    sept::DataVector v;
    v.reserve(s.size());
    for (auto c : s) {
        v.push_back(c);
    }
    return sept::ArrayTerm_c(std::move(v));
}

int run (std::istream &in, std::ostream &out) {
//     std::cerr << "front: run starting\n";
    size_t i = 0;
    while (true) {
//         std::cerr << "front: run start loop\n";
//         std::cerr << "front: run about to deserialize_data\n";
        auto value = sept::deserialize_data(in);
        std::cerr << "front: value " << i << " deserialized from stdin : " << value << "; " << LVD_REFLECT(sept::abstract_type_of_data(value)) << '\n';
        if (value == sept::ctl::EndOfFile) {
//             std::cerr << "front: " << value << " -- exiting.\n";
            return 0;
        } else if (sept::inhabits(value, sept::ctl::Output)) {
            std::cerr << "front: output : " << value.cast<sept::ctl::OutputTerm_c const &>().value() << '\n';
        } else if (value == sept::ctl::ClearOutput) {
            std::cerr << "front: clearing output\n";
        } else if (sept::inhabits(value, sept::ctl::RequestSyncInput)) {
            std::cerr << "\n\nfront: requesting input > ";
            std::string s;
            std::getline(std::cin, s, '\n');
            std::cerr << "\n\n";
//             std::cerr << "front: input was \"" << s << "\"\n";
            auto a = Array_from_string(s);
//             std::cerr << "front: serializing " << a << '\n';
            sept::serialize(a, out);
            out.flush();
        }
        ++i;
    }
    return 0;
}

int main (int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <backend-program> [arg1] [arg2] ...\n";
        return -1;
    }
    assert(argv[argc] == nullptr); // TEMP HACK -- this technically should segfault

    lvd::Pipe pipe_parent_to_child;
    lvd::Pipe pipe_child_to_parent;

    // Fork the process and run the backend program as the child.
    auto child_pid = fork();
    if (child_pid == 0) {
        // This is the child process.

        // Set up the pipe ends.
        pipe_child_to_parent.dup2(lvd::Pipe::End::WRITE, lvd::Fd::STDOUT);
        pipe_parent_to_child.dup2(lvd::Pipe::End::READ, lvd::Fd::STDIN);
        // Close the unused ends.
        pipe_child_to_parent.close(lvd::Pipe::End::READ);
        pipe_parent_to_child.close(lvd::Pipe::End::WRITE);

        // Exec the child process -- `v` means use `argv` convention, `p` means search executable path.
        std::cerr << "child: " << LVD_REFLECT(argc) << '\n';
        for (int i = 0; i < argc; ++i)
            std::cerr << "child: " << LVD_REFLECT(i) << ", " << LVD_REFLECT(argv[i]) << '\n';
        execvp(argv[1], &argv[1]);
        // If execvp returned, this indicates an error, so return with error.
        std::cerr << "execvp failed\n";
        return -1;
    } else {
        // This is the parent process

        // Set up the pipe ends, creating an istream and ostream for input and output.
        boost::iostreams::stream_buffer<boost::iostreams::file_descriptor_source> fd_buffer_in(pipe_child_to_parent.descriptor(lvd::Pipe::End::READ), boost::iostreams::never_close_handle);
        std::istream in(&fd_buffer_in);
        boost::iostreams::stream_buffer<boost::iostreams::file_descriptor_sink> fd_buffer_out(pipe_parent_to_child.descriptor(lvd::Pipe::End::WRITE), boost::iostreams::never_close_handle);
        std::ostream out(&fd_buffer_out);
        // Close the unused ends.
        pipe_child_to_parent.close(lvd::Pipe::End::WRITE);
        pipe_parent_to_child.close(lvd::Pipe::End::READ);

        // Run the actual parent program itself.
        return run(in, out);
    }
}
