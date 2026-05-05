#include <catch2/catch_all.hpp>

#include <mytoml/mytoml.hpp>
#include <string>

TEST_CASE("Parse primitive values", "[parser]")
{
    SECTION("integer")
    {
        const auto doc = mytoml::toml::parse("answer = 42");
        INFO(doc.serialize());
        REQUIRE(doc["answer"].as_int() == 42);
    }

    SECTION("floating point")
    {
        const auto doc = mytoml::toml::parse("pi = 3.14159");
        INFO(doc.serialize());
        REQUIRE(doc["pi"].as_float() == Catch::Approx(3.14159));
    }

    SECTION("boolean")
    {
        const auto doc = mytoml::toml::parse("enabled = true");
        INFO(doc.serialize());
        REQUIRE(doc["enabled"].as_bool());
    }

    SECTION("string")
    {
        const auto doc = mytoml::toml::parse(R"(name = "hello")");
        INFO(doc.serialize());
        REQUIRE(doc["name"].as_string() == "hello");
    }
}

TEST_CASE("Parse arrays and tables", "[parser]")
{
    const auto doc = mytoml::toml::parse(R"(
[database]
ports = [8001, 8002, 8003]
[owner]
name = "Tom"
)");

    INFO(doc.serialize());

    REQUIRE(doc["database"]["ports"].as_array().size() == 3);
    REQUIRE(doc["database"]["ports"][static_cast<size_t>(0)].as_int() == 8001);
    REQUIRE(doc["owner"]["name"].as_string() == "Tom");
}

TEST_CASE("Serialize round trip", "[serializer]")
{
    mytoml::toml doc = mytoml::toml::table({
        {"name", mytoml::toml("example")},
        {"count", mytoml::toml(7)},
        {"enabled", mytoml::toml(true)},
    });

    const auto text = doc.serialize();
    REQUIRE(text.find("name") != std::string::npos);
    REQUIRE(text.find("count") != std::string::npos);
    REQUIRE(text.find("enabled") != std::string::npos);
}
