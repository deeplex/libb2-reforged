# Written in 2017, 2019, 2022-2023 by Henrik Steffen Gaßmann <henrik@gassmann.onl>
#
# To the extent possible under law, the author(s) have dedicated all
# copyright and related and neighboring rights to this software to the
# public domain worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication
# along with this software. If not, see
#
#     http://creativecommons.org/publicdomain/zero/1.0/
#
########################################################################
cmake_minimum_required(VERSION 3.23...4.0)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/tools/cmake/")

########################################################################
# configure vcpkg from environment vars if possible
include(VcpkgDefaults)

if (BUILD_TESTING)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

########################################################################
project(libb2-reforged
    VERSION 0.0.0.0
    LANGUAGES C
)

include(EnforceOutOfSourceBuilds)
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(GNUInstallDirs)

include(SourceHelpers)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
if (BUILD_TESTING)
    enable_language(CXX)
    enable_testing()
endif()


########################################################################
# options

option(DPLX_BLAKE2_DISABLE_WORKAROUNDS "Disable all workarounds" OFF)
option(DPLX_BLAKE2_FLAG_OUTDATED_WORKAROUNDS "Emit compiler errors for workarounds which are active, but haven't been validated for this version" OFF)

option(DPLX_BLAKE2_WITH_LIBB2_COMPAT "Provide a libb2 API compatibility layer" ON)

# architecture lists for which to enable assembly / SIMD sources
set(AMD64_NAMES amd64 AMD64 x86_64)
set(X86_NAMES i686 x86 X86)
set(ARMv8_NAMES aarch64 AArch64 arm64 ARM64 armv8 armv8a)

set(IMPLEMENTATIONS GENERIC SSE2 SSE41 AVX NEON)

# default SIMD compiler flag configuration; you can override the cache variables
# (without "_INIT") defined further below if needed.
if (MSVC)

    set(DPLX_BLAKE2_CFLAGS_SSE2_INIT  "/arch:SSE2")
    # MSVC has no dedicated sse4.1 flag (see https://learn.microsoft.com/en-us/cpp/build/reference/arch-x86?view=msvc-170)
    set(DPLX_BLAKE2_CFLAGS_SSE41_INIT "/arch:SSE4.2")
    set(DPLX_BLAKE2_CFLAGS_NEON_INIT  "/arch:AVX")

elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU"
        OR CMAKE_C_COMPILER_ID STREQUAL "Clang"
        OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")

    set(DPLX_BLAKE2_CFLAGS_SSE2_INIT  "-msse2")
    set(DPLX_BLAKE2_CFLAGS_SSE41_INIT "-msse4.1")
    set(DPLX_BLAKE2_CFLAGS_AVX_INIT   "-mavx")
    set(DPLX_BLAKE2_CFLAGS_XSAVE_INIT "-mxsave")
    # 32-bit ARMv8 needs NEON to be enabled explicitly
    if (CMAKE_SIZEOF_VOID_P LESS "8")
        set(DPLX_BLAKE2_CFLAGS_NEON_INIT "-mfpu=neon")
    else()
        set(DPLX_BLAKE2_CFLAGS_NEON_INIT "")
    endif()

endif()

if (CMAKE_C_COMPILER_ARCHITECTURE_ID)
    set(ARCHITECTURE_ID "${CMAKE_C_COMPILER_ARCHITECTURE_ID}")

else()
    set(ARCHITECTURE_ID "${CMAKE_SYSTEM_PROCESSOR}")

endif()
message(STATUS "BLAKE2 arch detected: ${ARCHITECTURE_ID}")

set(DPLX_BLAKE2_WITH_GENERIC ON CACHE BOOL "compile & include the generic implementation")
if (ARCHITECTURE_ID IN_LIST AMD64_NAMES
    OR ARCHITECTURE_ID IN_LIST X86_NAMES)
    message(STATUS "BLAKE2 applying x86 defaults")

    set(DPLX_BLAKE2_CFLAGS_SSE2  "${DPLX_BLAKE2_CFLAGS_SSE2_INIT}"  CACHE STRING "the compiler flags to enable SSE2")
    set(DPLX_BLAKE2_CFLAGS_SSE41 "${DPLX_BLAKE2_CFLAGS_SSE41_INIT}" CACHE STRING "the compiler flags to enable SSE4.1")
    set(DPLX_BLAKE2_CFLAGS_AVX   "${DPLX_BLAKE2_CFLAGS_AVX_INIT}"   CACHE STRING "the compiler flags to enable AVX")
    set(DPLX_BLAKE2_CFLAGS_XSAVE "${DPLX_BLAKE2_CFLAGS_XSAVE_INIT}" CACHE STRING "the compiler flags to enable _xgetbv")

    set(DPLX_BLAKE2_WITH_SSE2 ON CACHE BOOL "compile & include the SSE2 implementation")
    set(DPLX_BLAKE2_WITH_SSE41 ON CACHE BOOL "compile & include the SSE4.1 implementation")
    set(DPLX_BLAKE2_WITH_AVX ON CACHE BOOL "compile & include the SSE4.1 implementation in AVX mode (using the VEX prefix)")

elseif (ARCHITECTURE_ID IN_LIST ARMv8_NAMES)
    message(STATUS "BLAKE2 applying ARMv8 defaults")

    set(DPLX_BLAKE2_CFLAGS_NEON "${DPLX_BLAKE2_CFLAGS_NEON_INIT}" CACHE STRING "the compiler flags to enable NEON")
    set(DPLX_BLAKE2_WITH_NEON OFF CACHE BOOL "compile & include the NEON implementation")

endif()
mark_as_advanced(DPLX_BLAKE2_CFLAGS_SSE2 DPLX_BLAKE2_CFLAGS_SSE41 DPLX_BLAKE2_CFLAGS_AVX DPLX_BLAKE2_CFLAGS_XSAVE DPLX_BLAKE2_CFLAGS_NEON)

foreach (IMPL IN LISTS IMPLEMENTATIONS)
    if (DPLX_BLAKE2_WITH_${IMPL})
        list(APPEND ACTIVE_IMPLEMENTATIONS ${IMPL})
    endif()
endforeach()

list(LENGTH ACTIVE_IMPLEMENTATIONS NUM_ACTIVE_IMPLEMENTATIONS)
list(TRANSFORM ACTIVE_IMPLEMENTATIONS TOLOWER OUTPUT_VARIABLE ACTIVE_IMPLEMENTATION_FILE_PARTS)
if (NUM_ACTIVE_IMPLEMENTATIONS LESS "1")
    message(FATAL_ERROR "at least one implemenation must be compiled & included")

elseif (NUM_ACTIVE_IMPLEMENTATIONS EQUAL "1")
    set(DPLX_BLAKE2_NO_DISPATCH ON)

else()
    set(DPLX_BLAKE2_NO_DISPATCH OFF)
endif()


########################################################################
# dependencies

find_package(Catch2 CONFIG)
set_package_properties(Catch2 PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Allows to build the test suite"
)
find_package(nlohmann_json CONFIG)
set_package_properties(nlohmann_json PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Allows building the test suite (reading the kat json)"
)
cmake_dependent_option(BUILD_TESTING "Build the documentation using sphinx" OFF "(Catch2_FOUND AND nlohmann_json_FOUND)" OFF)

find_package(Sphinx)
set_package_properties(Sphinx PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Allows to build the documentation"
)
cmake_dependent_option(BUILD_DOCS "Build the documentation using sphinx" ON Sphinx_FOUND OFF)


########################################################################
# warning configuration
#include(CompilerWarnings)


########################################################################
# additional compiler options
set(CMAKE_C_EXTENSIONS OFF CACHE STRING "" FORCE)
set(CMAKE_CXX_EXTENSIONS OFF CACHE STRING "" FORCE)

add_library(compiler_settings INTERFACE)
add_library(Deeplex::libb2-reforged_compiler_settings ALIAS compiler_settings)
set_target_properties(compiler_settings PROPERTIES
    EXPORT_NAME libb2-reforged_compiler_settings
)

target_compile_features(compiler_settings INTERFACE cxx_std_20 c_std_17)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(compiler_settings INTERFACE
        /utf-8 # all sources are utf-8 encoded
        /Zc:__cplusplus # correctly define the __cplusplus macro
    )
endif()


########################################################################
# library
add_library(libb2-reforged)
add_library(Deeplex::libb2-reforged ALIAS libb2-reforged)
set_target_properties(libb2-reforged PROPERTIES
    PREFIX ""
    VERSION ${libb2-reforged_VERSION_MAJOR}.${libb2-reforged_VERSION_MINOR}.${libb2-reforged_VERSION_PATCH}
    SOVERSION 0
)

target_link_libraries(libb2-reforged PUBLIC Deeplex::libb2-reforged_compiler_settings)

target_include_directories(libb2-reforged PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated/src>
    $<INSTALL_INTERFACE:include>
)

########################################################################
# library test project
if (BUILD_TESTING)
    add_executable(libb2-reforged-tests)
    source_group(TREE blake2_tests PREFIX test_utils)

    target_compile_features(libb2-reforged-tests PUBLIC cxx_std_23)
    target_link_libraries(libb2-reforged-tests PRIVATE
        Deeplex::libb2-reforged

        nlohmann_json::nlohmann_json
        Catch2::Catch2 Catch2::Catch2WithMain
    )

    target_include_directories(libb2-reforged-tests PRIVATE
        src/blake2_tests
    )

    add_test(NAME libb2-reforged-tests COMMAND libb2-reforged-tests
             WORKING_DIRECTORY $<TARGET_FILE_DIR:libb2-reforged-tests>)
endif()

########################################################################
# source files
include(sources.cmake)
source_group(vcpkg REGULAR_EXPRESSION .*/vcpkg_installed/.*)


########################################################################
# docs
if (BUILD_DOCS)
    add_subdirectory(docs)
endif()

########################################################################
# cmake install
install(DIRECTORY src/dplx
    TYPE INCLUDE
    FILES_MATCHING
    REGEX ".*\\.h(pp)?"
    PATTERN "detail/*.h" EXCLUDE
)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated/src/dplx
    TYPE INCLUDE
    FILES_MATCHING
    REGEX ".*\\.h(pp)?"
)
if (DPLX_BLAKE2_WITH_LIBB2_COMPAT)
    install(FILES
            src/blake2.h
        TYPE INCLUDE
    )
endif()

install(TARGETS libb2-reforged compiler_settings EXPORT libb2-reforged-targets)
install(EXPORT libb2-reforged-targets
    NAMESPACE Deeplex::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libb2-reforged"
)

export(EXPORT libb2-reforged-targets NAMESPACE Deeplex::)
# note that you need to configure with -DCMAKE_EXPORT_PACKAGE_REGISTRY=ON
# for this to have any effect at all
export(PACKAGE libb2-reforged)

configure_package_config_file(tools/libb2-reforged-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/libb2-reforged-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libb2-reforged"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libb2-reforged-config-version.cmake"
    COMPATIBILITY SameMinorVersion
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libb2-reforged-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libb2-reforged-config-version.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libb2-reforged"
)


########################################################################
feature_summary(WHAT PACKAGES_FOUND PACKAGES_NOT_FOUND)
