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

if (BUILD_TESTING OR NOT DEFINED BUILD_TESTING)
    list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

########################################################################
project(libb2-reforged
    VERSION 0.0.0.0
    LANGUAGES CXX
)

include(EnforceOutOfSourceBuilds)
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(GNUInstallDirs)

include(SourceHelpers)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
if (BUILD_TESTING)
    enable_testing()
endif()


########################################################################
# options

option(DPLX_BLAKE2_DISABLE_WORKAROUNDS "Disable all workarounds" OFF)
option(DPLX_BLAKE2_FLAG_OUTDATED_WORKAROUNDS "Emit compiler errors for workarounds which are active, but haven't been validated for this version" OFF)

########################################################################
# dependencies

find_package(Catch2 CONFIG)
set_package_properties(Catch2 PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Allows to build the test suite"
)
cmake_dependent_option(BUILD_TESTING "Build the documentation using sphinx" ON Catch2_FOUND OFF)

find_package(Sphinx)
set_package_properties(Sphinx PROPERTIES
    TYPE OPTIONAL
    PURPOSE "Allows to build the documentation"
)
cmake_dependent_option(BUILD_DOCS "Build the documentation using sphinx" ON Sphinx_FOUND OFF)


########################################################################
# warning configuration
include(CompilerWarnings)


########################################################################
# additional compiler options
set(CMAKE_CXX_EXTENSIONS OFF CACHE STRING "" FORCE)

add_library(compiler_settings INTERFACE)
add_library(Deeplex::libb2-reforged_compiler_settings ALIAS compiler_settings)
set_target_properties(compiler_settings PROPERTIES
    EXPORT_NAME libb2-reforged_compiler_settings
)

target_compile_features(compiler_settings INTERFACE cxx_std_20)
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

    target_link_libraries(libb2-reforged-tests PRIVATE
        Deeplex::libb2-reforged

        Catch2::Catch2 Catch2::Catch2WithMain
    )

    target_include_directories(libb2-reforged-tests PRIVATE
        src/blake2_tests
    )

    add_test(NAME libb2-reforged-tests COMMAND libb2-reforged-tests)
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
)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated/src/dplx
    TYPE INCLUDE
    FILES_MATCHING
    REGEX ".*\\.h(pp)?"
)

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
