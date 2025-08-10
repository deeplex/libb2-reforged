
// Copyright Henrik Steffen Gaßmann 2023
//
// Distributed under the Boost Software License, Version 1.0.
//         (See accompanying file LICENSE or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <ranges>
#include <string>
#include <utility>

namespace blake2_tests
{

inline constexpr char hex_digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

template <std::input_iterator I,
          std::sentinel_for<I> S,
          std::weakly_incrementable O,
          class Proj = std::identity>
    requires std::indirectly_writable<O, char>
             && std::same_as<typename std::projected<I, Proj>::value_type,
                             std::uint8_t>
constexpr auto hex_encode(I in, S const inEnd, O result, Proj proj = {}) -> O
{
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
    for (; in != inEnd; ++in, ++result)
    {
        std::uint8_t const byte = proj(*in);
        *result = hex_digits[(byte >> 4) & 0x0f];
        *(++result) = hex_digits[byte & 0x0f];
    }
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
    return result;
}
template <std::ranges::input_range R,
          std::weakly_incrementable O,
          class Proj = std::identity>
    requires std::indirectly_writable<O, char>
             && std::same_as<typename std::projected<std::ranges::iterator_t<R>,
                                                     Proj>::value_type,
                             std::uint8_t>
constexpr auto hex_encode(R &&r, O result, Proj proj = {}) -> O
{
    if constexpr (std::ranges::contiguous_range<R>)
    {
        auto const in = std::ranges::data(r);
        return blake2_tests::hex_encode(in, in + std::ranges::size(r), result,
                                        std::move(proj));
    }
    else
    {
        return blake2_tests::hex_encode(std::ranges::begin(r),
                                        std::ranges::end(r), result,
                                        std::move(proj));
    }
}

template <std::input_iterator I,
          std::sized_sentinel_for<I> S,
          class Proj = std::identity>
    requires std::same_as<typename std::projected<I, Proj>::value_type,
                          std::uint8_t>
inline auto hex_encode(I in, S inEnd, Proj proj = {}) noexcept -> std::string
{
    auto const binarySize = static_cast<std::size_t>(inEnd - in);

    std::string encoded;
    encoded.resize_and_overwrite(
            binarySize * 2U, [&](char *buf, std::size_t bufSize) noexcept {
                blake2_tests::hex_encode(in, inEnd, buf, std::move(proj));
                return bufSize;
            });
    return encoded;
}
template <std::ranges::input_range R, class Proj = std::identity>
    requires std::ranges::sized_range<R>
             && std::same_as<typename std::projected<std::ranges::iterator_t<R>,
                                                     Proj>::value_type,
                             std::uint8_t>
inline auto hex_encode(R &&r, Proj proj = {}) noexcept -> std::string
{
    std::string encoded;
    encoded.resize_and_overwrite(std::ranges::size(r) * 2U,
                                 [&](char *buf, std::size_t bufSize) noexcept {
                                     blake2_tests::hex_encode(r, buf,
                                                              std::move(proj));
                                     return bufSize;
                                 });
    return encoded;
}

} // namespace blake2_tests
