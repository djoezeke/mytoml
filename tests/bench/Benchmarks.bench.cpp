/**
 * @file Benchmarks.bench.cpp
 * @brief Catch2 micro-benchmarks for the mytoml parser and emitter.
 *
 * These benchmarks are intentionally kept in a separate executable so that
 * they do not slow down the regular unit-test run. Build them with
 * MYTOML_BUILD_BENCHMARKS=ON (the default when MYTOML_BUILD_TESTS=ON) and run
 * the resulting `mytoml-cpp-benchmarks` binary.
 */

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <mytoml/mytoml.hpp>

#include <cstddef>
#include <cstdint>
#include <string>

namespace
{
    std::string make_small_config()
    {
        return R"(
title = "Mytoml benchmark"
version = "0.1.0"
enabled = true
retries = 3
ratio = 0.75

[server]
host = "localhost"
port = 8080
tags = ["alpha", "beta", "gamma"]

[server.tls]
enabled = true
protocols = ["tls1.2", "tls1.3"]

[client]
timeout = 30
)";
    }

    std::string make_flat_config(std::size_t entries)
    {
        std::string text;
        text.reserve(entries * 20);
        for (std::size_t i = 0; i < entries; ++i)
        {
            text += "key";
            text += std::to_string(i);
            text += " = ";
            text += std::to_string(i);
            text += "\n";
        }
        return text;
    }

    std::string make_nested_config(std::size_t sections, std::size_t values_per_section)
    {
        std::string text;
        text.reserve(sections * values_per_section * 24);
        for (std::size_t section = 0; section < sections; ++section)
        {
            text += "[section";
            text += std::to_string(section);
            text += "]\n";
            for (std::size_t value = 0; value < values_per_section; ++value)
            {
                text += "key";
                text += std::to_string(value);
                text += " = ";
                text += std::to_string(value);
                text += "\n";
            }
        }
        return text;
    }

    std::string make_array_config(std::size_t entries)
    {
        std::string text = "values = [";
        text.reserve(entries * 8);
        for (std::size_t i = 0; i < entries; ++i)
        {
            if (i != 0)
            {
                text += ", ";
            }
            text += std::to_string(i);
        }
        text += "]\n";
        return text;
    }

    unsigned int decode_utf8(const char *data, std::size_t size)
    {
        unsigned int codepoint = 0;
        if (mytoml::detail::utf8::decode(data, size, codepoint) < 0)
        {
            return 0;
        }
        return codepoint;
    }
}

TEST_CASE("Benchmark: parsing", "[benchmark]")
{
    const std::string small = make_small_config();
    const std::string flat = make_flat_config(2000);
    const std::string nested = make_nested_config(200, 10);
    const std::string arrays = make_array_config(1000);
    const std::string empty;

    BENCHMARK("parse small configuration")
    {
        return mytoml::toml::parse(small);
    };

    BENCHMARK("parse flat configuration (2000 keys)")
    {
        return mytoml::toml::parse(flat);
    };

    BENCHMARK("parse nested configuration (200 tables, 2000 values)")
    {
        return mytoml::toml::parse(nested);
    };

    BENCHMARK("parse array value (1000 elements)")
    {
        return mytoml::toml::parse(arrays);
    };

    BENCHMARK("parse empty document")
    {
        return mytoml::toml::parse(empty);
    };
}

TEST_CASE("Benchmark: emitting", "[benchmark]")
{
    const mytoml::toml small = mytoml::toml::parse(make_small_config());
    const mytoml::toml flat = mytoml::toml::parse(make_flat_config(2000));
    const mytoml::toml nested = mytoml::toml::parse(make_nested_config(200, 10));
    const mytoml::toml arrays = mytoml::toml::parse(make_array_config(1000));

    BENCHMARK("emit small configuration")
    {
        return small.dump();
    };

    BENCHMARK("emit flat configuration (2000 keys)")
    {
        return flat.dump();
    };

    BENCHMARK("emit nested configuration (200 tables, 2000 values)")
    {
        return nested.dump();
    };

    BENCHMARK("emit array value (1000 elements)")
    {
        return arrays.dump();
    };
}

TEST_CASE("Benchmark: round-trip and access", "[benchmark]")
{
    const std::string small = make_small_config();
    const mytoml::toml document = mytoml::toml::parse(small);

    BENCHMARK("parse + emit small configuration")
    {
        return mytoml::toml::parse(small).dump();
    };

    BENCHMARK("lookup nested string value")
    {
        const auto &table = document.as_table();
        const auto section = table.find("server");
        if (section == table.end())
        {
            return std::string{};
        }
        const auto host = section->second.as_table().find("host");
        if (host == section->second.as_table().end())
        {
            return std::string{};
        }
        return host->second.as_string();
    };

    BENCHMARK("construct a small table")
    {
        mytoml::toml::table_t table;
        table["title"] = mytoml::toml(std::string("benchmark"));
        table["count"] = mytoml::toml(std::int64_t{42});
        table["ratio"] = mytoml::toml(0.5);
        table["enabled"] = mytoml::toml(true);
        return mytoml::toml(std::move(table));
    };
}

TEST_CASE("Benchmark: encoding helpers", "[benchmark]")
{
    const char ascii[] = {'A'};
    const char two_byte[] = {'\xC2', '\xA9'};
    const char three_byte[] = {'\xE2', '\x82', '\xAC'};
    const char four_byte[] = {'\xF0', '\x9F', '\x98', '\x80'};
    char output[4] = {};

    BENCHMARK("decode UTF-8 ASCII")
    {
        return decode_utf8(ascii, sizeof(ascii));
    };

    BENCHMARK("decode UTF-8 two-byte code point")
    {
        return decode_utf8(two_byte, sizeof(two_byte));
    };

    BENCHMARK("decode UTF-8 three-byte code point")
    {
        return decode_utf8(three_byte, sizeof(three_byte));
    };

    BENCHMARK("decode UTF-8 four-byte code point")
    {
        return decode_utf8(four_byte, sizeof(four_byte));
    };

    BENCHMARK("encode UTF-8 four-byte code point")
    {
        return mytoml::detail::utf8::encode(0x1F600, output, sizeof(output));
    };
}
