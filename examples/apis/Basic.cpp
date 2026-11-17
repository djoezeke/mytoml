/**
 * @file Basic.cpp
 * @brief End-to-end usage example for the mytoml C++ API.
 *
 * This example demonstrates:
 *  - parsing a TOML document from a string
 *  - checking node types before reading values
 *  - navigating tables and arrays
 *  - modifying an existing document
 *  - building and emitting a document from scratch
 *  - the `_toml` literal operator
 *  - stream input/output
 *  - UTF-8 decode/encode helpers
 *  - handling mytoml::parse_error
 *
 * Build (from the repository root):
 *   g++ -std=c++17 -Iinclude examples/apis/Basic.cpp src/mytoml.cpp -o basic
 */

#include <mytoml/mytoml.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

namespace
{
    void print_header(const char *title)
    {
        std::cout << "\n== " << title << " ==\n";
    }
}

int main()
{
    // ---------------------------------------------------------------------
    // 1. Parse a document.
    // ---------------------------------------------------------------------
    const std::string source = R"(
# A small configuration document.
title = "Mytoml example"
enabled = true
retries = 3
ratio = 0.75
tags = ["toml", "cpp", "config"]

[server]
host = "127.0.0.1"
port = 8080

[server.limits]
timeout = 30

[owner]
name = "Sackey"
)";

    mytoml::toml config = mytoml::toml::parse(source);

    print_header("Parsed values");
    std::cout << "title   = " << config.as_table().at("title").as_string() << '\n';
    std::cout << "enabled = " << std::boolalpha << config.as_table().at("enabled").as_boolean() << '\n';
    std::cout << "retries = " << config.as_table().at("retries").as_integer() << '\n';
    std::cout << "ratio   = " << config.as_table().at("ratio").as_floating() << '\n';

    // ---------------------------------------------------------------------
    // 2. Check node types before accessing values.
    // ---------------------------------------------------------------------
    print_header("Type checks");
    const auto &root = config.as_table();
    for (const auto &entry : root)
    {
        std::cout << entry.first << " is " << entry.second.type() << '\n';
    }

    // ---------------------------------------------------------------------
    // 3. Navigate nested tables and arrays.
    // ---------------------------------------------------------------------
    print_header("Nested access");
    const mytoml::toml::table_t &server = root.at("server").as_table();
    std::cout << "server.host = " << server.at("host").as_string() << '\n';
    std::cout << "server.port = " << server.at("port").as_integer() << '\n';
    std::cout << "server.limits.timeout = "
              << server.at("limits").as_table().at("timeout").as_integer() << '\n';

    const mytoml::toml::array_t &tags = root.at("tags").as_array();
    std::cout << "tags:";
    for (const auto &tag : tags)
        std::cout << ' ' << tag.as_string();
    std::cout << '\n';

    // ---------------------------------------------------------------------
    // 4. Modify the parsed document.
    // ---------------------------------------------------------------------
    print_header("Modified document");
    config.as_table()["enabled"] = false;
    config.as_table()["retries"] = int64_t{5};
    config.as_table()["release"] = "0.1.0";
    std::cout << config.dump();

    // ---------------------------------------------------------------------
    // 5. Build a document from scratch and emit it.
    // ---------------------------------------------------------------------
    print_header("Built from scratch");
    mytoml::toml::table_t application;
    application["name"] = std::string("demo");
    application["version"] = int64_t{1};
    application["debug"] = true;

    mytoml::toml::array_t features;
    features.emplace_back("parse");
    features.emplace_back("emit");
    application["features"] = std::move(features);

    mytoml::toml::table_t database;
    database["url"] = "sqlite://demo.db";
    database["pool_size"] = int64_t{4};
    application["database"] = std::move(database);

    const mytoml::toml built{std::move(application)};
    std::cout << built.dump();

    // ---------------------------------------------------------------------
    // 6. The `_toml` literal operator.
    // ---------------------------------------------------------------------
    print_header("Literal operator");
    using namespace mytoml::literals;
    const mytoml::toml literal = R"(
name = "from literal"
count = 2
)"_toml;
    std::cout << literal.dump();

    // ---------------------------------------------------------------------
    // 7. Stream input and output.
    // ---------------------------------------------------------------------
    print_header("Streams");
    std::istringstream input("answer = 42\n");
    mytoml::toml from_stream;
    input >> from_stream;

    std::ostringstream output;
    output << from_stream;
    std::cout << "answer = " << from_stream.as_table().at("answer").as_integer() << '\n';
    std::cout << "stream dump:\n" << output.str();

    // ---------------------------------------------------------------------
    // 8. UTF-8 helpers (U+2713 CHECK MARK).
    // ---------------------------------------------------------------------
    print_header("UTF-8 helpers");
    const char check_mark[] = "\xE2\x9C\x93";
    unsigned int codepoint = 0;
    const int decoded = mytoml::detail::utf8::decode(check_mark, sizeof(check_mark) - 1, codepoint);
    if (decoded > 0)
    {
        mytoml::detail::utf8::char_t encoded[4] = {};
        const int encoded_size = mytoml::detail::utf8::encode(codepoint, encoded, sizeof(encoded));
        std::cout << "decoded U+" << std::hex << codepoint << std::dec
                  << " (" << decoded << " bytes), re-encoded to " << encoded_size << " bytes: ";
        std::cout.write(reinterpret_cast<const char *>(encoded), encoded_size);
        std::cout << '\n';
    }

    // ---------------------------------------------------------------------
    // 9. Error handling.
    // ---------------------------------------------------------------------
    print_header("Error handling");
    try
    {
        const mytoml::toml invalid = mytoml::toml::parse("key = \"unterminated");
        (void)invalid;
    }
    catch (const mytoml::parse_error &error)
    {
        std::cout << "parse_error: " << error.what() << '\n';
    }
    catch (const mytoml::exception &error)
    {
        std::cout << "mytoml error: " << error.what() << '\n';
    }

    return 0;
}
