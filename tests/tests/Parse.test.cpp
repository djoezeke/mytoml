#include "TestSupport.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <string>

TEST_CASE("Parsing empty documents yields an empty table", "[parsing]")
{
    SECTION("empty string")
    {
        const auto document = mytoml::toml::parse("");
        REQUIRE(document.is_table());
        CHECK(document.as_table().empty());
    }

    SECTION("whitespace only")
    {
        const auto document = mytoml::toml::parse("   \n\t\r\n  \t");
        REQUIRE(document.is_table());
        CHECK(document.as_table().empty());
    }

    SECTION("comments only")
    {
        const auto document = mytoml::toml::parse("# a comment\n   # an indented comment\n\n");
        REQUIRE(document.is_table());
        CHECK(document.as_table().empty());
    }
}

TEST_CASE("Parsing scalar values", "[parsing]")
{
    const auto document = mytoml::toml::parse(R"(
title = "TOML Example"
owner = "Tom Preston-Werner"
count = 42
negative = -17
positive = +7
zero = 0
maximum = 9223372036854775807
ratio = 3.5
leading_dot = .5
exponent = 1e3
negative_exponent = -1.25e2
enabled = true
disabled = false
)");

    CHECK(testsupport::at(document, "title").is_string());
    CHECK(testsupport::at(document, "title").as_string() == "TOML Example");
    CHECK(testsupport::at(document, "owner").as_string() == "Tom Preston-Werner");

    CHECK(testsupport::at(document, "count").is_integer());
    CHECK(testsupport::at(document, "count").as_integer() == 42);
    CHECK(testsupport::at(document, "negative").as_integer() == -17);
    CHECK(testsupport::at(document, "positive").as_integer() == 7);
    CHECK(testsupport::at(document, "zero").as_integer() == 0);
    CHECK(testsupport::at(document, "maximum").as_integer() == INT64_MAX);

    CHECK(testsupport::at(document, "ratio").is_floating());
    CHECK(testsupport::at(document, "ratio").as_floating() == Catch::Approx(3.5));
    CHECK(testsupport::at(document, "leading_dot").as_floating() == Catch::Approx(0.5));
    CHECK(testsupport::at(document, "exponent").as_floating() == Catch::Approx(1000.0));
    CHECK(testsupport::at(document, "negative_exponent").as_floating() == Catch::Approx(-125.0));

    CHECK(testsupport::at(document, "enabled").is_boolean());
    CHECK(testsupport::at(document, "enabled").as_boolean());
    CHECK_FALSE(testsupport::at(document, "disabled").as_boolean());
}

TEST_CASE("Parsing strings", "[parsing]")
{
    SECTION("empty and plain strings")
    {
        const auto document = mytoml::toml::parse(R"(
empty = ""
plain = "hello"
spaced = " hello world "
with_equals = "a = b"
with_comment_marker = "# not a comment"
unicode = "café"
)");

        CHECK(testsupport::at(document, "empty").as_string().empty());
        CHECK(testsupport::at(document, "plain").as_string() == "hello");
        CHECK(testsupport::at(document, "spaced").as_string() == " hello world ");
        CHECK(testsupport::at(document, "with_equals").as_string() == "a = b");
        CHECK(testsupport::at(document, "with_comment_marker").as_string() == "# not a comment");
        CHECK(testsupport::at(document, "unicode").as_string() == "café");
    }

    SECTION("basic escape sequences")
    {
        const auto document = mytoml::toml::parse(
            R"(escaped = "line\nbreak\ttab\"quote\\slash")");

        CHECK(testsupport::at(document, "escaped").as_string() == "line\nbreak\ttab\"quote\\slash");
    }

    SECTION("unknown escape sequences are preserved without the backslash")
    {
        // NOTE: This is current behaviour, not TOML 1.0 behaviour. Full TOML
        // escape handling (including \uXXXX and \UXXXXXXXX) is not implemented.
        const auto document = mytoml::toml::parse(R"(escaped = "a\qb")");
        CHECK(testsupport::at(document, "escaped").as_string() == "aqb");
    }
}

TEST_CASE("Parsing arrays", "[parsing]")
{
    const auto document = mytoml::toml::parse(R"(
numbers = [1, 2, 3]
mixed = [1, "two", true, 3.5]
nested = [[1, 2], [3, 4]]
empty = []
strings = ["a,b", "c[d]", "e]"]
trailing = [1, 2, ]
objects = [{ name = "a" }, { name = "b" }]
)");

    SECTION("homogeneous arrays")
    {
        const auto &numbers = testsupport::at(document, "numbers");
        REQUIRE(numbers.is_array());
        REQUIRE(numbers.as_array().size() == 3);
        CHECK(numbers.as_array()[0].as_integer() == 1);
        CHECK(numbers.as_array()[1].as_integer() == 2);
        CHECK(numbers.as_array()[2].as_integer() == 3);
    }

    SECTION("mixed arrays")
    {
        const auto &mixed = testsupport::at(document, "mixed");
        REQUIRE(mixed.is_array());
        REQUIRE(mixed.as_array().size() == 4);
        CHECK(mixed.as_array()[0].as_integer() == 1);
        CHECK(mixed.as_array()[1].as_string() == "two");
        CHECK(mixed.as_array()[2].as_boolean());
        CHECK(mixed.as_array()[3].as_floating() == Catch::Approx(3.5));
    }

    SECTION("nested arrays")
    {
        const auto &nested = testsupport::at(document, "nested");
        REQUIRE(nested.is_array());
        REQUIRE(nested.as_array().size() == 2);
        CHECK(nested.as_array()[0].as_array()[0].as_integer() == 1);
        CHECK(nested.as_array()[0].as_array()[1].as_integer() == 2);
        CHECK(nested.as_array()[1].as_array()[0].as_integer() == 3);
        CHECK(nested.as_array()[1].as_array()[1].as_integer() == 4);
    }

    SECTION("empty arrays")
    {
        const auto &empty = testsupport::at(document, "empty");
        REQUIRE(empty.is_array());
        CHECK(empty.as_array().empty());
    }

    SECTION("delimiters inside strings are ignored")
    {
        const auto &strings = testsupport::at(document, "strings");
        REQUIRE(strings.as_array().size() == 3);
        CHECK(strings.as_array()[0].as_string() == "a,b");
        CHECK(strings.as_array()[1].as_string() == "c[d]");
        CHECK(strings.as_array()[2].as_string() == "e]");
    }

    SECTION("trailing commas are accepted")
    {
        const auto &trailing = testsupport::at(document, "trailing");
        REQUIRE(trailing.as_array().size() == 2);
    }

    SECTION("arrays of inline tables")
    {
        const auto &objects = testsupport::at(document, "objects");
        REQUIRE(objects.is_array());
        REQUIRE(objects.as_array().size() == 2);
        CHECK(objects.as_array()[0].as_table().at("name").as_string() == "a");
        CHECK(objects.as_array()[1].as_table().at("name").as_string() == "b");
    }
}

TEST_CASE("Parsing inline tables", "[parsing]")
{
    const auto document = mytoml::toml::parse(R"(
point = { x = 1, y = 2, label = "origin" }
nested = { inner = { value = 1 }, list = [1, 2] }
empty = {}
trailing = { x = 1, }
)");

    SECTION("simple inline table")
    {
        const auto &point = testsupport::at(document, "point");
        REQUIRE(point.is_table());
        CHECK(point.as_table().size() == 3);
        CHECK(point.as_table().at("x").as_integer() == 1);
        CHECK(point.as_table().at("y").as_integer() == 2);
        CHECK(point.as_table().at("label").as_string() == "origin");
    }

    SECTION("nested inline table and array")
    {
        const auto &nested = testsupport::at(document, "nested");
        REQUIRE(nested.is_table());
        CHECK(nested.as_table().at("inner").as_table().at("value").as_integer() == 1);
        const auto &list = nested.as_table().at("list");
        REQUIRE(list.is_array());
        CHECK(list.as_array().size() == 2);
    }

    SECTION("empty and trailing-comma inline tables")
    {
        CHECK(testsupport::at(document, "empty").as_table().empty());
        CHECK(testsupport::at(document, "trailing").as_table().size() == 1);
    }
}

TEST_CASE("Parsing dotted keys", "[parsing]")
{
    SECTION("dotted keys create nested tables")
    {
        const auto document = mytoml::toml::parse("a.b.c = 1\na.b.d = 2\n");
        CHECK(testsupport::at(document, {"a", "b", "c"}).as_integer() == 1);
        CHECK(testsupport::at(document, {"a", "b", "d"}).as_integer() == 2);
    }

    SECTION("dotted keys are relative to the current table")
    {
        const auto document = mytoml::toml::parse("[server]\nnetwork.host = \"localhost\"\nnetwork.port = 80\n");
        CHECK(testsupport::at(document, {"server", "network", "host"}).as_string() == "localhost");
        CHECK(testsupport::at(document, {"server", "network", "port"}).as_integer() == 80);
    }
}

TEST_CASE("Parsing tables", "[parsing]")
{
    const auto document = mytoml::toml::parse(R"(
[server]
host = "localhost"
port = 8080

[server.tls]
enabled = true
protocols = ["tls1.2", "tls1.3"]

[client]
retries = 3

[empty]
)");

    SECTION("top-level table")
    {
        const auto &server = testsupport::at(document, "server");
        REQUIRE(server.is_table());
        CHECK(server.as_table().at("host").as_string() == "localhost");
        CHECK(server.as_table().at("port").as_integer() == 8080);
    }

    SECTION("nested table")
    {
        const auto &tls = testsupport::at(document, {"server", "tls"});
        REQUIRE(tls.is_table());
        CHECK(tls.as_table().at("enabled").as_boolean());
        REQUIRE(tls.as_table().at("protocols").is_array());
        CHECK(tls.as_table().at("protocols").as_array().size() == 2);
    }

    SECTION("sibling tables")
    {
        CHECK(testsupport::at(document, {"client", "retries"}).as_integer() == 3);
    }

    SECTION("empty table")
    {
        const auto &empty = testsupport::at(document, "empty");
        REQUIRE(empty.is_table());
        CHECK(empty.as_table().empty());
    }
}

TEST_CASE("Parsing creates implicit parent tables", "[parsing]")
{
    const auto document = mytoml::toml::parse("[a.b.c]\nvalue = 1\n");
    CHECK(testsupport::at(document, {"a", "b", "c", "value"}).as_integer() == 1);
}

TEST_CASE("Repeated table headers merge their contents", "[parsing]")
{
    const auto document = mytoml::toml::parse(R"(
[section]
a = 1

[section]
b = 2
)");

    const auto &section = testsupport::at(document, "section");
    REQUIRE(section.is_table());
    CHECK(section.as_table().size() == 2);
    CHECK(section.as_table().at("a").as_integer() == 1);
    CHECK(section.as_table().at("b").as_integer() == 2);
}

TEST_CASE("Parsing handles comments and blank lines", "[parsing]")
{
    const auto document = mytoml::toml::parse(R"(
# leading comment

name = "value"
# between entries

[section]
# inside a table
flag = true
)");

    CHECK(testsupport::at(document, "name").is_string());
    CHECK(testsupport::at(document, "name").as_string() == "value");
    CHECK(testsupport::at(document, {"section", "flag"}).as_boolean());
}

TEST_CASE("Parsing accepts CRLF line endings", "[parsing]")
{
    const auto document = mytoml::toml::parse("a = 1\r\nb = \"two\"\r\n");
    CHECK(testsupport::at(document, "a").as_integer() == 1);
    CHECK(testsupport::at(document, "b").as_string() == "two");
}

TEST_CASE("Parsing preserves insertion-independent map ordering", "[parsing]")
{
    const auto document = mytoml::toml::parse("z = 1\na = 2\nm = 3\n");
    const auto &table = document.as_table();
    REQUIRE(table.size() == 3);

    auto it = table.begin();
    CHECK(it->first == "a");
    ++it;
    CHECK(it->first == "m");
    ++it;
    CHECK(it->first == "z");
}
