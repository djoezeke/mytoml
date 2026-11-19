#include "TestSupport.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <string>
#include <utility>

TEST_CASE("Default constructed values are null-like", "[values]")
{
    const mytoml::toml value;
    CHECK(value.type() == mytoml::node_t::unknown);
    CHECK_FALSE(value.is_table());
    CHECK_FALSE(value.is_array());
    CHECK_FALSE(value.is_string());
    CHECK_FALSE(value.is_integer());
    CHECK_FALSE(value.is_floating());
    CHECK_FALSE(value.is_boolean());
}

TEST_CASE("Values report their type", "[values]")
{
    const mytoml::toml table{mytoml::toml::table_t{}};
    const mytoml::toml array{mytoml::toml::array_t{}};
    const mytoml::toml string{mytoml::toml(std::string("text"))};
    const mytoml::toml integer{mytoml::toml(std::int64_t{7})};
    const mytoml::toml floating{mytoml::toml(1.25)};
    const mytoml::toml boolean{mytoml::toml(true)};

    CHECK(table.type() == mytoml::node_t::table);
    CHECK(array.type() == mytoml::node_t::array);
    CHECK(string.type() == mytoml::node_t::string);
    CHECK(integer.type() == mytoml::node_t::integer);
    CHECK(floating.type() == mytoml::node_t::floating);
    CHECK(boolean.type() == mytoml::node_t::boolean);

    CHECK(table.is_table());
    CHECK(array.is_array());
    CHECK(string.is_string());
    CHECK(integer.is_integer());
    CHECK(floating.is_floating());
    CHECK(boolean.is_boolean());
}

TEST_CASE("Values can be read through accessors", "[values]")
{
    SECTION("table")
    {
        mytoml::toml::table_t table;
        table["key"] = mytoml::toml(std::string("value"));
        auto value = mytoml::toml(std::move(table));

        REQUIRE(value.is_table());
        CHECK(value.as_table().at("key").as_string() == "value");
        value.as_table()["other"] = mytoml::toml(std::int64_t{1});
        CHECK(value.as_table().size() == 2);
    }

    SECTION("array")
    {
        mytoml::toml::array_t array;
        array.emplace_back(std::int64_t{1});
        auto value = mytoml::toml(std::move(array));

        REQUIRE(value.is_array());
        CHECK(value.as_array().size() == 1);
        value.as_array().emplace_back(std::int64_t{2});
        CHECK(value.as_array().size() == 2);
    }

    SECTION("string")
    {
        auto value = mytoml::toml(std::string("text"));
        REQUIRE(value.is_string());
        CHECK(value.as_string() == "text");
        value.as_string() = "changed";
        CHECK(value.as_string() == "changed");
    }

    SECTION("integer")
    {
        const auto value = mytoml::toml(std::int64_t{-42});
        REQUIRE(value.is_integer());
        CHECK(value.as_integer() == -42);
    }

    SECTION("floating")
    {
        const auto value = mytoml::toml(2.5);
        REQUIRE(value.is_floating());
        CHECK(value.as_floating() == 2.5);
    }

    SECTION("boolean")
    {
        const auto value = mytoml::toml(true);
        REQUIRE(value.is_boolean());
        CHECK(value.as_boolean());
    }
}

TEST_CASE("Floating accessor converts integer values", "[values]")
{
    const mytoml::toml integer{std::int64_t{3}};
    CHECK(integer.as_floating() == 3.0);

    const mytoml::toml floating{4.5};
    CHECK(floating.as_floating() == 4.5);
}

TEST_CASE("Values are copyable and movable", "[values]")
{
    const auto original = mytoml::toml::parse("a = 1\n[table]\nb = \"two\"\n");

    auto copy = original;
    CHECK(copy.as_table().at("a").as_integer() == 1);
    CHECK(copy.as_table().at("table").as_table().at("b").as_string() == "two");

    auto moved = std::move(copy);
    CHECK(moved.as_table().at("a").as_integer() == 1);
    CHECK(moved.as_table().at("table").as_table().at("b").as_string() == "two");
}

TEST_CASE("Node and encoding names are exposed", "[values][api]")
{
    CHECK(std::string(mytoml::string(mytoml::node_t::unknown)) == "unknown");
    CHECK(std::string(mytoml::string(mytoml::node_t::table)) == "table");
    CHECK(std::string(mytoml::string(mytoml::node_t::array)) == "array");
    CHECK(std::string(mytoml::string(mytoml::node_t::string)) == "string");
    CHECK(std::string(mytoml::string(mytoml::node_t::integer)) == "integer");
    CHECK(std::string(mytoml::string(mytoml::node_t::floating)) == "floating");
    CHECK(std::string(mytoml::string(mytoml::node_t::boolean)) == "boolean");

    CHECK(std::string(mytoml::string(mytoml::encoding::unspecified)) == "unspecified");
    CHECK(std::string(mytoml::string(mytoml::encoding::utf8)) == "utf8");
    CHECK(std::string(mytoml::string(mytoml::encoding::utf16)) == "utf16");
    CHECK(std::string(mytoml::string(mytoml::encoding::utf16le)) == "utf16le");
    CHECK(std::string(mytoml::string(mytoml::encoding::utf16be)) == "utf16be");
    CHECK(std::string(mytoml::string(mytoml::encoding::utf32)) == "utf32");
    CHECK(std::string(mytoml::string(mytoml::encoding::utf32le)) == "utf32le");
    CHECK(std::string(mytoml::string(mytoml::encoding::utf32be)) == "utf32be");
}
