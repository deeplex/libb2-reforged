
// Copyright 2025 Henrik Steffen Gaßmann
//
// Distributed under the Boost Software License, Version 1.0.
//         (See accompanying file LICENSE or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#include <utility>

#include <catch2/generators/catch_generators.hpp>

#include <dplx/blake2.h>

#include "test_utils.hpp"

namespace blake2_tests
{

class available_blake2_impl_id_generator final
    : public Catch::Generators::IGenerator<dplx_blake2_implementation_id>
{
    dplx_blake2_implementation_id mCurrent = DPLX_BLAKE2_IMPL_FALLBACK;

public:
    available_blake2_impl_id_generator() noexcept = default;

    [[nodiscard]] auto get() const
            -> dplx_blake2_implementation_id const & override
    {
        return mCurrent;
    }
    [[nodiscard]] auto next() -> bool override
    {
        while (mCurrent != DPLX_BLAKE2_IMPL_COUNT)
        {
            mCurrent = static_cast<dplx_blake2_implementation_id>(
                    std::to_underlying(mCurrent) + 1);
            if (dplx_blake2_has_implementation(mCurrent))
            {
                break;
            }
        }
        return mCurrent != DPLX_BLAKE2_IMPL_COUNT;
    }
};

inline auto available_blake2_impl_ids() -> Catch::Generators::GeneratorWrapper<dplx_blake2_implementation_id>
{
    return {new available_blake2_impl_id_generator()};
}

} // namespace blake2_tests
