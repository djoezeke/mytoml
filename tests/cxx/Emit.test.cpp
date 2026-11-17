#include "TestSupport.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <sstream>
#include <string>

namespace
{
    mytoml::toml::table_t make_server_table()
    {
        mytoml::toml::table_t server;
        server["host"] = mytoml::toml(std::string("localhost"));
        server["port"] = mytoml::toml(std::int64_t{8080});

        mytoml::toml::table_t tls;
        tls["enabled"] = mytoml::toml(true);
        server["tls"] = mytoml::toml(tls);

        return server;
    }
}

TEST_CASE("Emitting scalar values", "[emitting]")
{
    CHECK(mytoml::toml(std::int64_t{42}).dump() == "42");
    CHECK(mytoml::toml(std::int64_t{-7}).dump() == "-7");
    CHECK(mytoml::toml(2.5).dump() == "2.5");
    CHECK(mytoml::toml(true).dump() == "true");
    CHECK(mytoml::toml(false).dump() == "false");
    CHECK(mytoml::toml(std::string("hello")).dump() == "\"hello\"");
    CHECK(mytoml::toml("hello").dump() == "\"hello\"");
}

TEST_CASE("Emitting a default-constructed value", "[emitting][edge]")
{
    const mytoml::toml value;
    CHECK(value.type() == mytoml::node_t::unknown);
    CHECK(value.dump() == "null");
}

TEST_CASE("Emitting arrays", "[emitting]")
{
    SECTION("empty array")
    {
        const mytoml::toml value{mytoml::toml::array_t{}};
        CHECK(value.dump() == "[]");
    }

    SECTION("scalar array")
    {
        mytoml::toml::array_t values;
        values.emplace_back(std::int64_t{1});
        values.emplace_back(std::string("two"));
        values.emplace_back(true);
        values.emplace_back(3.5);

        const mytoml::toml value{std::move(values)};
        CHECK(value.dump() == "[1, \"two\", true, 3.5]");
    }

    SECTION("nested array")
    {
        mytoml::toml::array_t inner;
        inner.emplace_back(std::int64_t{1});
        inner.emplace_back(std::int64_t{2});

        mytoml::toml::array_t outer;
        outer.emplace_back(std::move(inner));
        outer.emplace_back(std::int64_t{3});

        const mytoml::toml value{std::move(outer)};
        CHECK(value.dump() == "[[1, 2], 3]");
    }

    SECTION("array of inline tables")
    {
        mytoml::toml::table_t first;
        first["name"] = mytoml::toml(std::string("a"));
        mytoml::toml::table_t second;
        second["name"] = mytoml::toml(std::string("b"));

        mytoml::toml::array_t values;
        values.emplace_back(std::move(first));
        values.emplace_back(std::move(second));

        const mytoml::toml value{std::move(values)};
        CHECK(value.dump() == "[{ name = \"a\" }, { name = \"b\" }]");
    }
}

TEST_CASE("Emitting inline tables", "[emitting]")
{
    SECTION("empty inline table nested in an array")
    {
        mytoml::toml::array_t values;
        values.emplace_back(mytoml::toml::table_t{});
        const mytoml::toml value{std::move(values)};
        CHECK(value.dump() == "[{  }]");
    }

    SECTION("inline table with sorted keys nested in an array")
    {
        mytoml::toml::table_t table;
        table["z"] = mytoml::toml(std::int64_t{1});
        table["a"] = mytoml::toml(std::int64_t{2});

        mytoml::toml::array_t values;
        values.emplace_back(std::move(table));

        const mytoml::toml value{std::move(values)};
        CHECK(value.dump() == "[{ a = 2, z = 1 }]");
    }

    SECTION("a table dumped on its own is emitted as a document")
    {
        mytoml::toml::table_t table;
        table["a"] = mytoml::toml(std::int64_t{2});
        table["z"] = mytoml::toml(std::int64_t{1});

        const mytoml::toml value{std::move(table)};
        CHECK(value.dump() == "a = 2\nz = 1\n");
    }
}

TEST_CASE("Emitting root tables", "[emitting]")
{
    SECTION("empty root table")
    {
        const mytoml::toml document{mytoml::toml::table_t{}};
        CHECK(document.dump().empty());
    }

    SECTION("scalars are sorted by key")
    {
        mytoml::toml::table_t table;
        table["title"] = mytoml::toml(std::string("TOML Example"));
        table["count"] = mytoml::toml(std::int64_t{3});
        table["ratio"] = mytoml::toml(1.5);
        table["enabled"] = mytoml::toml(true);

        const mytoml::toml document{std::move(table)};
        CHECK(document.dump() ==
              "count = 3\n"
              "enabled = true\n"
              "ratio = 1.5\n"
              "title = \"TOML Example\"\n");
    }

    SECTION("nested tables are emitted after root scalars")
    {
        mytoml::toml::table_t table;
        table["name"] = mytoml::toml(std::string("example"));
        table["server"] = mytoml::toml(make_server_table());

        const mytoml::toml document{std::move(table)};
        CHECK(document.dump() ==
              "name = \"example\"\n"
              "[server]\n"
              "host = \"localhost\"\n"
              "port = 8080\n"
              "[server.tls]\n"
              "enabled = true\n");
    }

    SECTION("empty nested tables still get a header")
    {
        mytoml::toml::table_t table;
        table["empty"] = mytoml::toml(mytoml::toml::table_t{});

        const mytoml::toml document{std::move(table)};
        CHECK(document.dump() == "[empty]\n");
    }

    SECTION("arrays of tables are emitted inline")
    {
        mytoml::toml::table_t first;
        first["name"] = mytoml::toml(std::string("a"));
        mytoml::toml::table_t second;
        second["name"] = mytoml::toml(std::string("b"));

        mytoml::toml::array_t values;
        values.emplace_back(std::move(first));
        values.emplace_back(std::move(second));

        mytoml::toml::table_t table;
        table["items"] = mytoml::toml(std::move(values));

        const mytoml::toml document{std::move(table)};
        CHECK(document.dump() == "items = [{ name = \"a\" }, { name = \"b\" }]\n");
    }
}

TEST_CASE("Emitting accepts an indent argument", "[emitting][edge]")
{
    // NOTE: indentation is currently ignored by the emitter. The test only
    // documents that the argument is accepted and does not change output.
    const auto document = mytoml::toml::parse("a = 1\n[table]\nb = 2\n");
    CHECK(document.dump(0) == document.dump());
    CHECK(document.dump(4) == document.dump());
}

TEST_CASE("Stream insertion emits a document", "[emitting]")
{
    const auto document = mytoml::toml::parse("a = 1\nb = \"two\"\n");

    std::ostringstream stream;
    stream << document;

    CHECK(stream.str() == "a = 1\nb = \"two\"\n");
}

TEST_CASE("Parsing and emitting round-trips supported documents", "[emitting][roundtrip]")
{
    const std::string input =
        "title = \"Example\"\n"
        "count = 3\n"
        "ratio = 2.5\n"
        "enabled = true\n"
        "tags = [\"a\", \"b\"]\n"
        "[server]\n"
        "host = \"localhost\"\n"
        "port = 8080\n";

    const auto original = mytoml::toml::parse(input);
    const auto emitted = original.dump();
    const auto reparsed = mytoml::toml::parse(emitted);

    CHECK(testsupport::at(reparsed, "title").as_string() == "Example");
    CHECK(testsupport::at(reparsed, "count").as_integer() == 3);
    CHECK(testsupport::at(reparsed, "ratio").as_floating() == 2.5);
    CHECK(testsupport::at(reparsed, "enabled").as_boolean());
    REQUIRE(testsupport::at(reparsed, "tags").is_array());
    CHECK(testsupport::at(reparsed, "tags").as_array().size() == 2);
    CHECK(testsupport::at(reparsed, {"server", "host"}).as_string() == "localhost");
    CHECK(testsupport::at(reparsed, {"server", "port"}).as_integer() == 8080);
}

TEST_CASE("Parsing and emitting round-trips inline tables and arrays", "[emitting][roundtrip]")
{
    const std::string input =
        "point = { x = 1, y = 2 }\n"
        "matrix = [[1, 2], [3, 4]]\n"
        "items = [{ name = \"a\" }, { name = \"b\" }]\n";

    const auto original = mytoml::toml::parse(input);
    const auto reparsed = mytoml::toml::parse(original.dump());

    CHECK(testsupport::at(reparsed, {"point", "x"}).as_integer() == 1);
    CHECK(testsupport::at(reparsed, {"point", "y"}).as_integer() == 2);
    REQUIRE(testsupport::at(reparsed, "matrix").is_array());
    CHECK(testsupport::at(reparsed, "matrix").as_array().size() == 2);
    REQUIRE(testsupport::at(reparsed, "items").is_array());
    CHECK(testsupport::at(reparsed, "items").as_array().size() == 2);
    CHECK(testsupport::at(reparsed, "items").as_array()[0].as_table().at("name").as_string() == "a");
}
