cmake_minimum_required(VERSION 3.18)
project(sept)
# NOTE: After cutting a release, you should increment the patch number and append `-DEV` so that
# the deb packages produced while developing are distinguished from the actual tagged release.
set(sept_VERSION 0.0.1-DEV)

option(BUILD_septcat "Build sept-cat binary" ON)
option(BUILD_testlibsept "Build test-libsept binary" ON)
option(BUILD_sept "Build sept binary (requires Qt5)" ON)
option(BUILD_interop "Build interop binaries: front and back (requires Boost)" ON)
option(BUILD_SHARED_LIBS "Build shared libraries (instead of static libraries)" ON)

# NOTE: There's something wonky with this option -- when you toggle it, it freaks out and lies
# a bit, and you may have to run configure twice, setting this option to the desired setting
# both times.  Hit 't' to view the advanced vars to verify that the desired compiler is configured.
option(USE_CLANG "Use clang++-10 C++ compiler")

if(USE_CLANG)
    # Use clang++-9 and libc++ (there's a bug in libstdc++ 8.3.0 regarding a particular use of std::any and std::is_copy_constructible)
    # Compiling with g++ against libc++ is nontrivial, so just use clang++.  Note that clang++-5 and later fully support C++17 --
    # see https://clang.llvm.org/cxx_status.html -- though for some reason clang++-6.0 seemed to not know about the <any> header.
    set(CMAKE_CXX_COMPILER "clang++-10" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS "-stdlib=libc++" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "-lc++abi" CACHE STRING "" FORCE)
else()
    set(CMAKE_CXX_COMPILER "g++-8" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS "" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "" FORCE)
endif()

# Use C++17 standard, but not extensions.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options to correctly link the standard C++ lib on Mac.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # GCC ("GNU") probably would require a different option
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
endif()

# Helper target(s)

add_library(Strict INTERFACE)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_compile_options(Strict INTERFACE -Wall -Werror)
endif()

add_library(SaveTemps INTERFACE)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_compile_options(SaveTemps INTERFACE -save-temps)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # TODO
endif()

add_library(CppStdFilesystem INTERFACE)
if(NOT USE_CLANG)
    target_link_libraries(CppStdFilesystem INTERFACE -lstdc++fs)
endif()

find_package(lvd 0.11.0 REQUIRED)

#
# Libraries
#

# libsept

set(libsept_HEADERS
    lib/sept/ArrayTerm.hpp
    lib/sept/ArrayType.hpp
    lib/sept/BaseArray_t.hpp
    lib/sept/BaseArrayT_t.hpp
    lib/sept/BaseArray_S_t.hpp
    lib/sept/ctl/ClearOutput.hpp
    lib/sept/ctl/EndOfFile.hpp
    lib/sept/ctl/Output.hpp
    lib/sept/ctl/RequestSyncInput.hpp
    lib/sept/core.hpp
    lib/sept/Data.hpp
    lib/sept/Data_t.hpp
    lib/sept/DataArray_t.hpp
    lib/sept/DataVector.hpp
    lib/sept/hash.hpp
    lib/sept/NPTerm.hpp
    lib/sept/NPType.hpp
    lib/sept/OrderedMapTerm.hpp
    lib/sept/OrderedMapType.hpp
    lib/sept/Placeholder.hpp
    lib/sept/Projection.hpp
    lib/sept/proj/ElementExtraction.hpp
    lib/sept/proj/TypeFunctor.hpp
    lib/sept/TreeNode_t.hpp
    lib/sept/type/Conversion.hpp
    lib/sept/type/Conversions.hpp
)
set(libsept_SOURCES
    lib/sept/ArrayTerm.cpp
    lib/sept/ArrayType.cpp
    lib/sept/BaseArray_t.cpp
    lib/sept/BaseArrayT_t.cpp
    lib/sept/BaseArray_S_t.cpp
    lib/sept/ctl/ClearOutput.cpp
    lib/sept/ctl/EndOfFile.cpp
    lib/sept/ctl/Output.cpp
    lib/sept/ctl/RequestSyncInput.cpp
    lib/sept/core.cpp
    lib/sept/Data.cpp
    lib/sept/DataVector.cpp
    lib/sept/NPTerm.cpp
    lib/sept/NPType.cpp
    lib/sept/OrderedMapTerm.cpp
    lib/sept/OrderedMapType.cpp
    lib/sept/Placeholder.cpp
    lib/sept/proj/ElementExtraction.cpp
    lib/sept/proj/TypeFunctor.cpp
    lib/sept/type/Conversion.cpp
    lib/sept/type/Conversions.cpp
)
add_library(libsept ${libsept_HEADERS} ${libsept_SOURCES})
# This is to prevent cmake from prepending `lib` to `libsept`.
set_target_properties(libsept PROPERTIES PREFIX "")

# Copied from https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages
set_property(TARGET libsept PROPERTY VERSION ${sept_VERSION})
set_property(TARGET libsept PROPERTY SOVERSION 0)
set_property(TARGET libsept PROPERTY INTERFACE_sept_MAJOR_VERSION 0)
set_property(TARGET libsept APPEND PROPERTY COMPATIBLE_INTERFACE_STRING sept_MAJOR_VERSION)

target_include_directories(libsept PUBLIC ${sept_SOURCE_DIR}/lib)
target_link_libraries(libsept PUBLIC Strict lvd)

#
# Executables
#

# test-libsept

if(BUILD_testlibsept)
    set(testlibsept_SOURCES
        bin/test-libsept/main.cpp
        bin/test-libsept/req.hpp
        bin/test-libsept/test_abstract_type_of.cpp
        bin/test-libsept/test_Array.cpp
        bin/test-libsept/test_ctl.cpp
        bin/test-libsept/test_Data.cpp
        bin/test-libsept/test_element_of.cpp
        bin/test-libsept/test_inhabits.cpp
        bin/test-libsept/test_NPTerm.cpp
        bin/test-libsept/test_NPType.cpp
        bin/test-libsept/test_OrderedMap.cpp
        bin/test-libsept/test_Placeholder.cpp
        bin/test-libsept/test_proj.cpp
        bin/test-libsept/test_serialization.cpp
        bin/test-libsept/test_TreeNode_t.cpp
        bin/test-libsept/test_type_Conversion.cpp
        bin/test-libsept/test_type_Conversions.cpp
    )
    add_executable(test-libsept ${testlibsept_SOURCES})
    target_include_directories(test-libsept PUBLIC ${sept_SOURCE_DIR}/bin/test-libsept)
    target_link_libraries(test-libsept PUBLIC Strict libsept)
endif()

# interop -- front and back

if(BUILD_interop)
    find_package(Boost 1.73 REQUIRED COMPONENTS iostreams)

    set(front_SOURCES
        bin/interop/front/main.cpp
    )
    add_executable(front ${front_SOURCES})
    target_include_directories(front PUBLIC ${sept_SOURCE_DIR}/bin/interop/front)
    target_link_libraries(front PUBLIC Strict libsept Boost::iostreams)

    set(back_SOURCES
        bin/interop/back/main.cpp
    )
    add_executable(back ${back_SOURCES})
    target_include_directories(back PUBLIC ${sept_SOURCE_DIR}/bin/interop/back)
    target_link_libraries(back PUBLIC Strict libsept)
endif()

# sept-cat -- utility for deserializing and printing serialized data just for readability's sake.

if(BUILD_septcat)
    set(septcat_SOURCES
        bin/sept-cat/main.cpp
    )
    add_executable(sept-cat ${septcat_SOURCES})
    target_include_directories(sept-cat PUBLIC ${sept_SOURCE_DIR}/bin/sept-cat)
    target_link_libraries(sept-cat PUBLIC Strict CppStdFilesystem libsept)
endif()

# sept

if(BUILD_sept)
    # Needed in order to automatically run `moc` on files containing one of the macros defined in the
    # ;-list AUTOMOC_MACRO_NAMES (this is initialized from CMAKE_AUTOMOC_MACRO_NAMES, whose default
    # value is `Q_OBJECT;Q_GADGET;Q_NAMESPACE`)
    set(CMAKE_AUTOMOC ON)

    find_package(Qt5 COMPONENTS Widgets REQUIRED)

    set(sept_SOURCES
        bin/sept/main.cpp
        bin/sept/SEPTItem.cpp
        bin/sept/SEPTItem.hpp
        bin/sept/SEPTItemDelegate.cpp
        bin/sept/SEPTItemDelegate.hpp
        bin/sept/SEPTModel.cpp
        bin/sept/SEPTModel.hpp
        bin/sept/SEPTTreeView.cpp
        bin/sept/SEPTTreeView.hpp
        bin/sept/MainWindow.cpp
        bin/sept/MainWindow.hpp
        bin/sept/RowIndexSet.hpp
    )
    add_executable(sept ${sept_SOURCES})
    target_include_directories(sept PUBLIC ${sept_SOURCE_DIR}/bin)
    target_link_libraries(sept PUBLIC Strict Qt5::Widgets libsept)
endif()

###############################################################################
# Install rules
###############################################################################

install(
    TARGETS libsept                              # Libraries produced by this package
    RUNTIME DESTINATION lib                     # This is relative to CMAKE_INSTALL_PREFIX
)
install(
    FILES ${libsept_HEADERS}
    DESTINATION include/sept                     # This is relative to CMAKE_INSTALL_PREFIX
)
install(
    FILES                                       # Important files
#         LICENSE
#         README.md
        sept-config.cmake                        # sept-config[-version].cmake is how find_package(sept) works.
        "${CMAKE_CURRENT_BINARY_DIR}/sept-config-version.cmake"
    DESTINATION lib/sept                         # This is relative to CMAKE_INSTALL_PREFIX
)

###############################################################################
# Package config stuff
###############################################################################

# See https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/sept-config-version.cmake"
    VERSION ${sept_VERSION}
    COMPATIBILITY AnyNewerVersion
)

###############################################################################
# CPack rules for creating distributions
###############################################################################

# Reference: https://github.com/geoffmcl/cpack-test

if(WIN32)
    if(USE_WIX_TOOLSET)
        set(CPACK_GENERATOR "WIX") # this need WiX Tooset installed and a path to candle.exe
    else ()
        set(CPACK_GENERATOR "NSIS") # this needs NSIS installed, and available
    endif ()
    set(CPACK_SOURCE_GENERATOR "ZIP")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CPACK_GENERATOR "PackageMake")
else()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_SOURCE_GENERATOR "TGZ")
endif()

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Victor Dods")

string(REPLACE "." ";" VERSION_LIST ${sept_VERSION})
list(GET VERSION_LIST 0 sept_VERSION_MAJOR)
list(GET VERSION_LIST 1 sept_VERSION_MINOR)
list(GET VERSION_LIST 2 sept_VERSION_PATCH)

set(CPACK_PACKAGE_VERSION "${sept_VERSION_MAJOR}.${sept_VERSION_MINOR}.${sept_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${sept_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${sept_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${sept_VERSION_PATCH}")

# set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.html")
# set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
# set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
# set(CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/README.html")

set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${sept_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${sept_VERSION}")

set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_SOURCE_DIR}/build/;${PROJECT_SOURCE_DIR}/.git/;${PROJECT_SOURCE_DIR}/.gitignore;${PROJECT_SOURCE_DIR}/sept.kdev4;${PROJECT_SOURCE_DIR}/.kdev4")

include(CPack)
