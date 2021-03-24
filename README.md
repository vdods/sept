# sept

Structured Expression Project Toolkit

The intent of this codebase is to manifest concepts discussed in [this article](https://itdont.work/2020/06/29/structured-expression-project-thesis-1-0/).

## Copyright and License

Copyright Victor Dods

Licensed under the Apache License, Version 2.0 (the "License"); you may not
use this software except in compliance with the License.  You may obtain a
copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

## Release Notes

Versioning system is [semantic versioning](https://semver.org/).

|Version|Notes|
|-------|-----|
|`v0.0.0`|Initial commit.|

## Building

### Configuration

This project uses `cmake`.  To create and configure the build system,

    cd <path/to/sept>
    mkdir build
    cd build
    cmake ..

or use `ccmake ..` instead for an interactive configuration, which allows one to specify certain
compilation/installation parameters.

### Building a distribution-specific package (e.g. for Ubuntu/Debian)

To create a Debian package named `sept-X.Y.Z.deb` (this may also create other distro-specific
packages on those distros), run the following command(s) from the `build` dir configured above.

    make package

This can then be installed via

    sudo dpkg -i sept-X.Y.Z.deb

This can be uninstalled via

    sudo dpkg -r sept

### Building and installing the old-fashioned way

To build, run the following command(s) from the `build` dir configured above.

    make

To install:

    make install

This will print out a log of which files were installed.  There doesn't appear to be an automatic way
to uninstall the package if it was installed via `make install`, but one can just go and delete the
files that were listed in the log.

## Notes

The main build targets:
-   `libsept` (library) : The library for `sept` itself.  Contains no GUI code.
-   `test-libsept` (binary) : A suite of unit tests for `libsept`, which will exit with return code 0
    and give no output upon success.  Run `./test-libsept --help` to see full usage.  The typical,
    medium-verbose output command is

        ./test-libsept --log-level=inf

    and for debugging purposes, it's useful to change the `--log-level` to `dbg` or `trc`.  Additionally,
    it's useful to have it abort upon error, using the command

        ./test-libsept --failure-behavior=abort

    Finally, a filtered set of tests can be run by supplying a prefix for the names of the tests to run, e.g.

        ./test-libsept --log-level=inf /320/ctl

-   `sept` (binary) : A QT-based GUI app meant for viewing, navigating, editing, and searching structured data.

Other, minor build targets:
-   `sept-cat` (binary) : Will read in a stream of serialized sept data from stdin and pretty-print
    it to stdout.  Run `./back | ./sept-cat` to see it in action.
-   `front` and `back` (binaries) : A simple demonstration of serialization of sept data.
    Run `./front ./back` to see it in action.

## To-dos

-   Eventually create `libseptqt`, which would be a library that provides QT support for sept.
-   Eventually separate into several cmake projects, each of which have its own package, in particular
    to separate out the libraries from binaries and to separate the QT from non-QT build targets.
