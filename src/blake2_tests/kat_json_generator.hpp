
// Copyright 2025 Henrik Steffen Gaßmann
//
// Distributed under the Boost Software License, Version 1.0.
//         (See accompanying file LICENSE or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <format>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include <catch2/generators/catch_generator_exception.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <nlohmann/json.hpp>

#include <dplx/blake2.h>

#include "hex_decode.hpp"
#include "hex_encode.hpp"
#include "test_utils.hpp"

namespace blake2_tests
{

struct b2_known_answer_dto
{
    std::string variant;
    std::vector<std::uint8_t> in;
    std::vector<std::uint8_t> key;
    std::vector<std::uint8_t> out;

    friend inline auto operator<<(std::ostream &os,
                                  b2_known_answer_dto const &ka)
            -> std::ostream &
    {
        std::print(os, "{{known_answer {} '{}' '{}'}}", ka.variant,
                   hex_encode(ka.in), hex_encode(ka.key));
        return os;
    }
};

void from_json(nlohmann::json const &j, b2_known_answer_dto &p)
{
    j.at("hash").get_to(p.variant);
    p.in = blake2_tests::hex_decode(j.at("in").get<std::string>());
    p.key = blake2_tests::hex_decode(j.at("key").get<std::string>());
    p.out = blake2_tests::hex_decode(j.at("out").get<std::string>());
}

class json_kat_loader_exception : public Catch::GeneratorException
{
    std::unique_ptr<char[]> mMessage;

public:
    json_kat_loader_exception(json_kat_loader_exception const &other)
        : json_kat_loader_exception(other.mMessage.get())
    {
    }

    explicit json_kat_loader_exception(std::string_view message)
        : json_kat_loader_exception(alloc_message(message))
    {
    }

private:
    json_kat_loader_exception(std::unique_ptr<char[]> message)
        : GeneratorException(message.get())
        , mMessage(std::move(message))
    {
    }

    static auto alloc_message(std::string_view content)
            -> std::unique_ptr<char[]>
    {
        auto allocation
                = std::make_unique_for_overwrite<char[]>(content.size() + 1U);
        std::memcpy(allocation.get(), content.data(), content.size());
        allocation[content.size()] = '\0';
        return allocation;
    }
};

class blake2_kat_json_loader final
    : public Catch::Generators::IGenerator<b2_known_answer_dto>
{
    std::vector<b2_known_answer_dto> mKnownAnswers;
    std::vector<b2_known_answer_dto>::const_iterator mCurrent;

public:
    explicit blake2_kat_json_loader(std::filesystem::path const &filename,
                                    std::string_view const algorithm)
    try
        : mKnownAnswers()
        , mCurrent()
    {
        std::unique_ptr<FILE, int (*)(FILE *)> file{
                std::fopen(filename.string().c_str(), "rb"), &std::fclose};
        if (file == nullptr)
        {
            throw json_kat_loader_exception(std::strerror(errno));
        }
        if (std::fseek(file.get(), 0, SEEK_END) != 0)
        {
            throw json_kat_loader_exception(std::strerror(errno));
        }
        long fileSize = std::ftell(file.get());
        if (fileSize < 0)
        {
            throw json_kat_loader_exception(std::strerror(errno));
        }
        if (!std::in_range<std::size_t>(fileSize))
        {
            throw json_kat_loader_exception(
                    "json kat is too large to fit into memory");
        }
        if (std::fseek(file.get(), 0, SEEK_SET) != 0)
        {
            throw json_kat_loader_exception(std::strerror(errno));
        }
        std::string content;
        content.resize_and_overwrite(
                static_cast<std::size_t>(fileSize),
                [hf = file.get()](char *buf, std::size_t bufSize) {
                    return std::fread(buf, 1, bufSize, hf);
                });
        if (std::ferror(file.get()) != 0)
        {
            throw json_kat_loader_exception(std::strerror(errno));
        }
        file.reset();

        mKnownAnswers = nlohmann::json::parse(content)
                                .get<std::vector<b2_known_answer_dto>>();

        std::erase_if(mKnownAnswers,
                      [&algorithm](b2_known_answer_dto const &ka) {
                          return ka.variant != algorithm;
                      });

        mCurrent = mKnownAnswers.cbegin();
    }
    catch (json_kat_loader_exception const &)
    {
        // pass through
        throw;
    }
    catch (std::exception const &exc)
    {
        // wrap in order to make generator failure known to Catch framework
        throw json_kat_loader_exception(
                std::format("kat_json_loader failed to load [{}]:\n{}",
                            filename.generic_string(), exc.what()));
    }

    [[nodiscard]] auto get() const -> b2_known_answer_dto const & override
    {
        return *mCurrent;
    }
    [[nodiscard]] auto next() -> bool override
    {
        mCurrent += 1;
        return mCurrent != mKnownAnswers.end();
    }
};

inline auto load_kat_from_json(std::filesystem::path const &filename,
                               std::string_view const algorithm)
        -> Catch::Generators::GeneratorWrapper<b2_known_answer_dto>
{
    return {new blake2_kat_json_loader(filename, algorithm)};
}

} // namespace blake2_tests
