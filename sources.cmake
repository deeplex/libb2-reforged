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

target_sources(libb2-reforged
    PRIVATE
        src/blake2.h
        src/dplx/blake2.h
        src/dplx/blake2/detail/blake2-impl.h
        src/dplx/blake2/detail/blake2b-common.c.inc
        src/dplx/blake2/detail/blake2s-common.c.inc
        src/dplx/blake2/detail/blake2xb-generic.c
        src/dplx/blake2/detail/blake2xs-generic.c
)

set(DISPATCH_DEFS "")
foreach (IMPL IN LISTS IMPLEMENTATIONS)
    string(TOLOWER "${IMPL}" IMPL_NAME)
    list(APPEND DISPATCH_DEFS "DPLX_BLAKE2_DISPATCH_${IMPL_NAME}=$<IN_LIST:${IMPL},${ACTIVE_IMPLEMENTATIONS}>")
endforeach()
if (DPLX_BLAKE2_NO_DISPATCH)
    target_sources(libb2-reforged
        PRIVATE
            src/dplx/blake2/detail/blake2-dispatch-stub.c
    )
    set_source_files_properties(
            src/dplx/blake2/detail/blake2-dispatch-stub.c

        PROPERTIES
            COMPILE_DEFINITIONS "${DISPATCH_DEFS}"
    )

else()
    target_sources(libb2-reforged
        PRIVATE
            src/dplx/blake2/detail/blake2-dispatch.c
    )
    set_source_files_properties(
            src/dplx/blake2/detail/blake2-dispatch.c

        PROPERTIES
            COMPILE_FLAGS "$<$<IN_LIST:AVX,${ACTIVE_IMPLEMENTATIONS}>:${DPLX_BLAKE2_CFLAGS_XSAVE}>"
            COMPILE_DEFINITIONS "${DISPATCH_DEFS}"
    )

endif()
if ("SSE2" IN_LIST ACTIVE_IMPLEMENTATIONS
    OR "SSE41" IN_LIST ACTIVE_IMPLEMENTATIONS
    OR "AVX" IN_LIST ACTIVE_IMPLEMENTATIONS)

    target_sources(libb2-reforged
        PRIVATE
            src/dplx/blake2/detail/blake2-x86-config.h
            src/dplx/blake2/detail/blake2b-x86-round.h
            src/dplx/blake2/detail/blake2s-x86-round.h

            src/dplx/blake2/detail/blake2b-sse2-load.h
            src/dplx/blake2/detail/blake2b-sse41-load.h
            src/dplx/blake2/detail/blake2s-sse2-load.h
            src/dplx/blake2/detail/blake2s-sse41-load.h
    )
endif()
if ("NEON" IN_LIST ACTIVE_IMPLEMENTATIONS)

    target_sources(libb2-reforged
        PRIVATE
            src/dplx/blake2/detail/blake2b-neon-load.h
            src/dplx/blake2/detail/blake2b-neon-round.h
            src/dplx/blake2/detail/blake2s-neon-load.h
            src/dplx/blake2/detail/blake2s-neon-round.h
    )
endif()
foreach(IMPL IMPL_FILE_PART IN ZIP_LISTS ACTIVE_IMPLEMENTATIONS ACTIVE_IMPLEMENTATION_FILE_PARTS)
    target_sources(libb2-reforged
        PRIVATE
            src/dplx/blake2/detail/blake2b-${IMPL_FILE_PART}.c
            src/dplx/blake2/detail/blake2s-${IMPL_FILE_PART}.c
    )
    set_source_files_properties(
        src/dplx/blake2/detail/blake2b-${IMPL_FILE_PART}.c
        src/dplx/blake2/detail/blake2s-${IMPL_FILE_PART}.c

        PROPERTIES
            COMPILE_FLAGS "${DPLX_BLAKE2_CFLAGS_${IMPL}}"
            COMPILE_DEFINITIONS "DPLX_BLAKE2_IMPL_NAME=${IMPL_FILE_PART};DPLX_BLAKE2_NO_DISPATCH=$<BOOL:${DPLX_BLAKE2_NO_DISPATCH}>"
    )
endforeach()

if (DPLX_BLAKE2_WITH_LIBB2_COMPAT)
    target_sources(libb2-reforged
        PRIVATE
            src/dplx/blake2/detail/libb2-compat.c
    )
endif()

if (BUILD_TESTING)
    dplx_target_sources(libb2-reforged-tests PRIVATE
        MODE VERBATIM
        BASE_DIR blake2_tests

        PRIVATE
            test_utils.hpp
    )
endif ()
