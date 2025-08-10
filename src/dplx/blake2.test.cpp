
// Copyright 2025 Henrik Steffen Gaßmann
//
// Distributed under the Boost Software License, Version 1.0.
//         (See accompanying file LICENSE or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#include "dplx/blake2.hpp"

#include <array>
#include <cstdint>

#include <blake2.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <dplx/blake2.h>

#include "blob_matcher.hpp"
#include "kat_json_generator.hpp"

namespace blake2_tests
{

TEST_CASE("dplx_blake2s() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2s"));

    INFO(ka);

    std::array<std::uint8_t, DPLX_BLAKE2S_OUTBYTES> out{};

    REQUIRE(!dplx_blake2s(out.data(), out.size(), ka.in.data(), ka.in.size(),
                          ka.key.data(), ka.key.size()));

    CHECK_BLOB_EQ(out, ka.out);
}

TEST_CASE("dplx_blake2b() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2b"));

    INFO(ka);

    std::array<std::uint8_t, DPLX_BLAKE2B_OUTBYTES> out{};

    REQUIRE(!dplx_blake2b(out.data(), out.size(), ka.in.data(), ka.in.size(),
                          ka.key.data(), ka.key.size()));

    CHECK_BLOB_EQ(out, ka.out);
}

} // namespace blake2_tests
