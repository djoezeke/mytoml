#include <catch2/catch_test_macros.hpp>
#include <mytoml/mytoml.hpp>

#include <sstream>
#include <string>

TEST_CASE("Version defaults to 0.0.0", "[version]")
{
    const mytoml::version version;
    CHECK(version.major == 0);
    CHECK(version.minor == 0);
    CHECK(version.patch == 0);
}

TEST_CASE("Version stores its components", "[version]")
{
    const mytoml::version version(1, 2, 3);
    CHECK(version.major == 1);
    CHECK(version.minor == 2);
    CHECK(version.patch == 3);
}

TEST_CASE("Version comparison operators", "[version]")
{
    const mytoml::version a(1, 2, 3);
    const mytoml::version b(1, 2, 4);
    const mytoml::version c(1, 3, 0);
    const mytoml::version d(2, 0, 0);
    const mytoml::version same(1, 2, 3);

    CHECK(a == same);
    CHECK_FALSE(a != same);
    CHECK(a != b);

    CHECK(a < b);
    CHECK(a < c);
    CHECK(a < d);
    CHECK_FALSE(b < a);

    CHECK(b > a);
    CHECK_FALSE(a > b);

    CHECK(a <= same);
    CHECK(a <= b);
    CHECK_FALSE(b <= a);

    CHECK(a >= same);
    CHECK(b >= a);
    CHECK_FALSE(a >= b);
}

TEST_CASE("Version streams in semantic form", "[version]")
{
    std::ostringstream stream;
    stream << mytoml::version(0, 1, 0);
    CHECK(stream.str() == "0.1.0");
}

TEST_CASE("Version macros describe the library version", "[version]")
{
    CHECK(MYTOML_VERSION_MAJOR == 0);
    CHECK(MYTOML_VERSION_MINOR == 1);
    CHECK(MYTOML_VERSION_PATCH == 0);
    CHECK(std::string(MYTOML_VERSION) == "0.1.0");
}

TEST_CASE("Break type names are exposed", "[version][api]")
{
    CHECK(std::string(mytoml::detail::string(mytoml::detail::break_t::any)) == "any");
    CHECK(std::string(mytoml::detail::string(mytoml::detail::break_t::cr)) == "cr");
    CHECK(std::string(mytoml::detail::string(mytoml::detail::break_t::ln)) == "ln");
    CHECK(std::string(mytoml::detail::string(mytoml::detail::break_t::crln)) == "crln");
}
