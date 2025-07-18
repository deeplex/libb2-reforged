dplx_target_sources(libb2-reforged
    TEST_TARGET libb2-reforged-tests
    MODE SMART_SOURCE MERGED_LAYOUT
    BASE_DIR dplx

    PUBLIC
        blake2
)

dplx_target_sources(libb2-reforged
    TEST_TARGET libb2-reforged-tests
    MODE SMART_HEADER_ONLY MERGED_LAYOUT
    BASE_DIR dplx

    PUBLIC
        blake2/config
        blake2/detail/workaround
)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated/src/dplx/blake2/detail")
configure_file(tools/config.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/generated/src/dplx/blake2/detail/config.hpp" @ONLY)
target_sources(libb2-reforged PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated/src/dplx/blake2/detail/config.hpp>)

if (BUILD_TESTING)
    dplx_target_sources(libb2-reforged-tests PRIVATE
        MODE VERBATIM
        BASE_DIR blake2_tests

        PRIVATE
            test_utils.hpp
    )
endif ()
