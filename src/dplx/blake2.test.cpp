
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
#include "impl_id_generator.hpp"
#include "kat_json_generator.hpp"

namespace blake2_tests
{

TEST_CASE("dplx_blake2s() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2s"));

    INFO(ka);

    dplx_blake2_implementation_id implId
            = GENERATE(available_blake2_impl_ids());
    INFO("impl id: " << implId);
    REQUIRE(dplx_blake2_use_implementation(implId) == 0);

    std::array<std::uint8_t, DPLX_BLAKE2S_OUTBYTES> out{};

    REQUIRE(dplx_blake2s(out.data(), out.size(), ka.in.data(), ka.in.size(),
                         ka.key.data(), ka.key.size())
            == 0);

    CHECK_BLOB_EQ(out, ka.out);
}

TEST_CASE("dplx_blake2s_*() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2s"));

    INFO(ka);

    dplx_blake2_implementation_id implId
            = GENERATE(available_blake2_impl_ids());
    INFO("impl id: " << implId);
    REQUIRE(dplx_blake2_use_implementation(implId) == 0);

    dplx_blake2s_state ctx{};
    if (ka.key.size() > 0)
    {
        REQUIRE(dplx_blake2s_init_key(&ctx, ka.out.size(), ka.key.data(),
                                      ka.key.size())
                == 0);
    }
    else
    {
        REQUIRE(dplx_blake2s_init(&ctx, ka.out.size()) == 0);
    }

    REQUIRE(dplx_blake2s_update(&ctx, ka.in.data(), ka.in.size()) == 0);

    std::array<std::uint8_t, DPLX_BLAKE2S_OUTBYTES> out{};
    REQUIRE(dplx_blake2s_final(&ctx, out.data(), out.size()) == 0);

    CHECK_BLOB_EQ(out, ka.out);
}

TEST_CASE("dplx_blake2xs() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2xs"));

    INFO(ka);

    dplx_blake2_implementation_id implId
            = GENERATE(available_blake2_impl_ids());
    INFO("impl id: " << implId);
    REQUIRE(dplx_blake2_use_implementation(implId) == 0);

    std::vector<std::uint8_t> out(ka.out.size());

    REQUIRE(dplx_blake2xs(out.data(), out.size(), ka.in.data(), ka.in.size(),
                          ka.key.data(), ka.key.size())
            == 0);

    CHECK_BLOB_EQ(out, ka.out);
}

TEST_CASE("dplx_blake2xs_*() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2xs"));

    INFO(ka);

    dplx_blake2_implementation_id implId
            = GENERATE(available_blake2_impl_ids());
    INFO("impl id: " << implId);
    REQUIRE(dplx_blake2_use_implementation(implId) == 0);

    dplx_blake2xs_state ctx{};
    if (ka.key.size() > 0)
    {
        REQUIRE(dplx_blake2xs_init_key(&ctx, ka.out.size(), ka.key.data(),
                                       ka.key.size())
                == 0);
    }
    else
    {
        REQUIRE(dplx_blake2xs_init(&ctx, ka.out.size()) == 0);
    }

    REQUIRE(dplx_blake2xs_update(&ctx, ka.in.data(), ka.in.size()) == 0);

    std::vector<std::uint8_t> out(ka.out.size());
    REQUIRE(dplx_blake2xs_final(&ctx, out.data(), out.size()) == 0);

    CHECK_BLOB_EQ(out, ka.out);
}

TEST_CASE("dplx_blake2b() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2b"));

    INFO(ka);

    dplx_blake2_implementation_id implId
            = GENERATE(available_blake2_impl_ids());
    INFO("impl id: " << implId);
    REQUIRE(dplx_blake2_use_implementation(implId) == 0);

    std::array<std::uint8_t, DPLX_BLAKE2B_OUTBYTES> out{};

    REQUIRE(dplx_blake2b(out.data(), out.size(), ka.in.data(), ka.in.size(),
                         ka.key.data(), ka.key.size())
            == 0);

    CHECK_BLOB_EQ(out, ka.out);
}

TEST_CASE("dplx_blake2b_*() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2b"));

    INFO(ka);

    dplx_blake2_implementation_id implId
            = GENERATE(available_blake2_impl_ids());
    INFO("impl id: " << implId);
    REQUIRE(dplx_blake2_use_implementation(implId) == 0);

    dplx_blake2b_state ctx{};
    if (ka.key.size() > 0)
    {
        REQUIRE(dplx_blake2b_init_key(&ctx, ka.out.size(), ka.key.data(),
                                      ka.key.size())
                == 0);
    }
    else
    {
        REQUIRE(dplx_blake2b_init(&ctx, ka.out.size()) == 0);
    }

    REQUIRE(dplx_blake2b_update(&ctx, ka.in.data(), ka.in.size()) == 0);

    std::array<std::uint8_t, DPLX_BLAKE2B_OUTBYTES> out{};
    REQUIRE(dplx_blake2b_final(&ctx, out.data(), out.size()) == 0);

    CHECK_BLOB_EQ(out, ka.out);
}

TEST_CASE("dplx_blake2xb() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2xb"));

    INFO(ka);

    dplx_blake2_implementation_id implId
            = GENERATE(available_blake2_impl_ids());
    INFO("impl id: " << implId);
    REQUIRE(dplx_blake2_use_implementation(implId) == 0);

    std::vector<std::uint8_t> out(ka.out.size());

    REQUIRE(dplx_blake2xb(out.data(), out.size(), ka.in.data(), ka.in.size(),
                          ka.key.data(), ka.key.size())
            == 0);

    CHECK_BLOB_EQ(out, ka.out);
}

TEST_CASE("dplx_blake2xb_*() should correctly compute the official testvectors")
{
    b2_known_answer_dto ka
            = GENERATE(load_kat_from_json("blake2-kat.json", "blake2xb"));

    INFO(ka);

    dplx_blake2_implementation_id implId
            = GENERATE(available_blake2_impl_ids());
    INFO("impl id: " << implId);
    REQUIRE(dplx_blake2_use_implementation(implId) == 0);

    dplx_blake2xb_state ctx{};
    if (ka.key.size() > 0)
    {
        REQUIRE(dplx_blake2xb_init_key(&ctx, ka.out.size(), ka.key.data(),
                                       ka.key.size())
                == 0);
    }
    else
    {
        REQUIRE(dplx_blake2xb_init(&ctx, ka.out.size()) == 0);
    }

    REQUIRE(dplx_blake2xb_update(&ctx, ka.in.data(), ka.in.size()) == 0);

    std::vector<std::uint8_t> out(ka.out.size());
    REQUIRE(dplx_blake2xb_final(&ctx, out.data(), out.size()) == 0);

    CHECK_BLOB_EQ(out, ka.out);
}

} // namespace blake2_tests
