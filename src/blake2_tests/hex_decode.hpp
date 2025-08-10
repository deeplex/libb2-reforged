
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
#include <type_traits>
#include <utility>

namespace blake2_tests
{

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

constexpr auto is_hex_digit(char const digit) noexcept -> bool
{
    unsigned const udigit = static_cast<unsigned char>(digit);
    return (udigit - '0' <= 9U) || ((udigit | 0x20U) - 'a' <= 5U);
}
template <std::input_iterator I,
          std::sentinel_for<I> S,
          class Proj = std::identity>
    requires std::same_as<typename std::projected<I, Proj>::value_type, char>
constexpr auto is_valid_hex_string(I in, S const inEnd, Proj proj = {}) -> bool
{
    if constexpr (std::sized_sentinel_for<S, I>)
    {
        auto const size = inEnd - in;
        if (size < 0 || size % 2 != 0)
        {
            return false;
        }
    }
    std::ptrdiff_t i = 0;
    for (; in != inEnd; ++in, ++i)
    {
        if (!blake2_tests::is_hex_digit(*proj(in)))
        {
            return false;
        }
    }
    if constexpr (!std::sized_sentinel_for<S, I>)
    {
        return i % 2 == 0;
    }
    return true;
}

constexpr auto value_of_hex_digit(char const digit) noexcept -> unsigned
{
    return (static_cast<unsigned>(digit) & 0xfU)
           + ((static_cast<unsigned>(digit) >> 6U) * 9U);
}

template <std::input_iterator I,
          std::sentinel_for<I> S,
          std::weakly_incrementable O,
          class Proj = std::identity>
    requires(std::indirectly_writable<O, std::uint8_t>
             || std::indirectly_writable<O, std::byte>)
            && std::same_as<typename std::projected<I, Proj>::value_type, char>
constexpr auto hex_decode(I in, S const inEnd, O result, Proj proj = {}) -> O
{
    using result_value_type
            = std::conditional_t<std::indirectly_writable<O, std::uint8_t>,
                                 std::uint8_t, std::byte>;

    for (; in != inEnd; ++in, ++result)
    {
        unsigned const hi = blake2_tests::value_of_hex_digit(*proj(in));
        unsigned const lo = blake2_tests::value_of_hex_digit(*proj(++in));
        *result = static_cast<result_value_type>((hi << 4) | lo);
    }
    return result;
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

template <std::ranges::input_range R,
          std::weakly_incrementable O,
          class Proj = std::identity>
    requires(std::indirectly_writable<O, std::uint8_t>
             || std::indirectly_writable<O, std::byte>)
            && std::same_as<typename std::projected<std::ranges::iterator_t<R>,
                                                    Proj>::value_type,
                            char>
constexpr auto hex_decode(R &&r, O result, Proj proj = {}) -> O
{
    if constexpr (std::ranges::contiguous_range<R>)
    {
        auto const in = std::ranges::data(r);
        return blake2_tests::hex_decode(in, in + std::ranges::size(r), result,
                                        std::move(proj));
    }
    else
    {
        return blake2_tests::hex_decode(std::ranges::begin(r),
                                        std::ranges::end(r), result,
                                        std::move(proj));
    }
}

template <std::input_iterator I,
          std::sized_sentinel_for<I> S,
          class Proj = std::identity>
    requires std::same_as<typename std::projected<I, Proj>::value_type, char>
constexpr auto hex_decode(I in, S const inEnd, Proj proj = {})
        -> std::vector<std::uint8_t>
{
    auto const hexSize = static_cast<std::size_t>(inEnd - in);
    std::vector<std::uint8_t> result(hexSize / 2U);
    blake2_tests::hex_decode(in, inEnd, result.data(), std::move(proj));
    return result;
}

template <std::ranges::input_range R, class Proj = std::identity>
    requires std::ranges::sized_range<R>
             && std::same_as<typename std::projected<std::ranges::iterator_t<R>,
                                                     Proj>::value_type,
                             char>
constexpr auto hex_decode(R &&r, Proj proj = {}) -> std::vector<std::uint8_t>
{
    std::vector<std::uint8_t> result(std::ranges::size(r) / 2U);
    blake2_tests::hex_decode(r, result.data(), std::move(proj));
    return result;
}

} // namespace blake2_tests
