#include "TestSupport.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstring>
#include <sstream>
#include <string>

TEST_CASE("TOML literal operator parses documents", "[api][literals]")
{
    using namespace mytoml::literals;

    SECTION("char literal")
    {
        const auto document = "a = 1\nb = \"two\""_toml;
        REQUIRE(document.is_table());
        CHECK(document.as_table().at("a").as_integer() == 1);
        CHECK(document.as_table().at("b").as_string() == "two");
    }

    SECTION("char16_t literal")
    {
        const auto document = u"a = 1"_toml;
        REQUIRE(document.is_table());
        CHECK(document.as_table().at("a").as_integer() == 1);
    }

    SECTION("char32_t literal")
    {
        const auto document = U"a = 1"_toml;
        REQUIRE(document.is_table());
        CHECK(document.as_table().at("a").as_integer() == 1);
    }

    SECTION("explicit operator call")
    {
        const char text[] = "a = 1";
        const auto document = mytoml::literals::operator""_toml(text, sizeof(text) - 1);
        CHECK(document.as_table().at("a").as_integer() == 1);
    }
}

TEST_CASE("Stream extraction parses documents", "[api]")
{
    std::istringstream input("a = 1\n[table]\nb = \"two\"\n");
    mytoml::toml document;

    input >> document;

    CHECK(document.is_table());
    CHECK(testsupport::at(document, "a").as_integer() == 1);
    CHECK(testsupport::at(document, {"table", "b"}).as_string() == "two");
}

TEST_CASE("Stream insertion for encodings and node types", "[api]")
{
    std::ostringstream stream;
    stream << mytoml::encoding::utf8 << ' ' << mytoml::node_t::integer;
    CHECK(stream.str() == "utf8 integer");
}

TEST_CASE("Memory input adapter reads bounded chunks", "[api][adapter]")
{
    char data[] = "abcdef";
    mytoml::detail::memory_iadapter adapter(data, 6);

    char output[8] = {};
    CHECK(adapter.read(output, 4) == 4);
    CHECK(std::string(output, 4) == "abcd");

    CHECK(adapter.read(output, 4) == 2);
    CHECK(std::string(output, 2) == "ef");
    CHECK(adapter.read(output, 4) == 0);

    CHECK(adapter.read(nullptr, 4) == 0);
    CHECK(adapter.read(output, 0) == 0);
}

TEST_CASE("Memory input adapter rejects empty input", "[api][adapter]")
{
    mytoml::detail::memory_iadapter adapter(nullptr, 0);
    char output[1] = {};
    CHECK(adapter.read(output, 1) == 0);
}

TEST_CASE("Stream input adapter reads from an istream", "[api][adapter]")
{
    std::istringstream input("hello");
    mytoml::detail::stream_iadapter adapter(input);

    char output[5] = {};
    CHECK(adapter.read(output, 2) == 2);
    CHECK(std::string(output, 2) == "he");
    CHECK(adapter.read(output, 2) == 2);
    CHECK(std::string(output, 2) == "ll");
    CHECK(adapter.read(output, 2) == 1);
    CHECK(std::string(output, 1) == "o");
    CHECK(adapter.read(output, 2) == 0);
}

TEST_CASE("Stream output adapter writes to an ostream", "[api][adapter]")
{
    std::ostringstream stream;
    mytoml::detail::stream_oadapter adapter(stream);

    CHECK(adapter.write("abc", 3) == 3);
    CHECK(adapter.write("de", 2) == 2);
    CHECK(adapter.write("", 0) == 0);
    CHECK(adapter.write(nullptr, 1) == 0);
    CHECK(stream.str() == "abcde");
}

TEST_CASE("Memory output adapter respects its capacity", "[api][adapter]")
{
    char buffer[4] = {};
    mytoml::detail::memory_oadapter adapter(buffer, sizeof(buffer));

    CHECK(adapter.write("abc", 3) == 3);
    CHECK(adapter.write("de", 2) == 1);
    CHECK(adapter.write("f", 1) == 0);
    CHECK(adapter.write(nullptr, 1) == 0);
    CHECK(adapter.write("x", 0) == 0);

    CHECK(std::string(buffer, 4) == "abcd");

    mytoml::detail::memory_oadapter empty(nullptr, 0);
    CHECK(empty.write("x", 1) == 0);
}

TEST_CASE("Lexer produces tokens for primitive constructs", "[api][lexer]")
{
    char data[] = "{}[],:\"text\" 42 -1.5 true false null";
    mytoml::detail::memory_iadapter adapter(data, sizeof(data) - 1);
    mytoml::detail::lexer lexer(&adapter);

    CHECK(lexer.get_type() == mytoml::detail::token_t::unknown);

    auto token = lexer.next_token();
    CHECK(token.type == mytoml::detail::token_t::object_start);
    CHECK(lexer.next_token().type == mytoml::detail::token_t::object_end);
    CHECK(lexer.next_token().type == mytoml::detail::token_t::array_start);
    CHECK(lexer.next_token().type == mytoml::detail::token_t::array_end);
    CHECK(lexer.next_token().type == mytoml::detail::token_t::value_separator);
    CHECK(lexer.next_token().type == mytoml::detail::token_t::name_separator);

    token = lexer.next_token();
    CHECK(token.type == mytoml::detail::token_t::string_value);
    CHECK(token.text == "text");

    token = lexer.next_token();
    CHECK(token.type == mytoml::detail::token_t::number_value);
    CHECK(token.text == "42");

    token = lexer.next_token();
    CHECK(token.type == mytoml::detail::token_t::number_value);
    CHECK(token.text == "-1.5");

    CHECK(lexer.next_token().type == mytoml::detail::token_t::true_literal);
    CHECK(lexer.next_token().type == mytoml::detail::token_t::false_literal);
    CHECK(lexer.next_token().type == mytoml::detail::token_t::null_literal);
    CHECK(lexer.next_token().type == mytoml::detail::token_t::end_of_input);

    CHECK(lexer.get_token().type == mytoml::detail::token_t::end_of_input);
    CHECK(lexer.get_type() == mytoml::detail::token_t::end_of_input);
    CHECK(lexer.next_token().type == mytoml::detail::token_t::end_of_input);
}

TEST_CASE("Lexer handles strings and unknown characters", "[api][lexer]")
{
    SECTION("escaped string")
    {
        char data[] = R"("a\nb")";
        mytoml::detail::memory_iadapter adapter(data, sizeof(data) - 1);
        mytoml::detail::lexer lexer(&adapter);

        const auto token = lexer.next_token();
        CHECK(token.type == mytoml::detail::token_t::string_value);
        CHECK(token.text == "a\\nb");
    }

    SECTION("unterminated string")
    {
        char data[] = "\"abc";
        mytoml::detail::memory_iadapter adapter(data, sizeof(data) - 1);
        mytoml::detail::lexer lexer(&adapter);

        CHECK(lexer.next_token().type == mytoml::detail::token_t::string_value);
    }

    SECTION("unknown character")
    {
        char data[] = "=";
        mytoml::detail::memory_iadapter adapter(data, sizeof(data) - 1);
        mytoml::detail::lexer lexer(&adapter);

        CHECK(lexer.next_token().type == mytoml::detail::token_t::unknown);
    }

    SECTION("empty input")
    {
        char data[] = "";
        mytoml::detail::memory_iadapter adapter(data, sizeof(data) - 1);
        mytoml::detail::lexer lexer(&adapter);

        CHECK(lexer.next_token().type == mytoml::detail::token_t::end_of_input);
    }
}
