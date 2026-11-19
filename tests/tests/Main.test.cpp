#include <catch2/catch_test_macros.hpp>
#include <mytoml/mytoml.hpp>

TEST_CASE("Library headers are usable", "[cxx][smoke]")
{
    const mytoml::toml document = mytoml::toml::parse("answer = 42\n");
    REQUIRE(document.is_table());
    CHECK(document.as_table().at("answer").as_integer() == 42);
}
