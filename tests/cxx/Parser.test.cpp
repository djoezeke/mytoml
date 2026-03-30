#include <catch2/catch_all.hpp>

#include <mytoml/mytoml.hpp>

using mytoml::node_t;
using mytoml::toml;

TEST_CASE("Parse scalars", "[toml]")
{
    auto t = toml::parse("number = 42\npi = 3.14\nflag = true\ntext = \"hi\"\n");
    const auto &tbl = t.as_table();
    REQUIRE(tbl.at("number").is_integer());
    REQUIRE(tbl.at("number").as_integer() == 42);
    REQUIRE(tbl.at("pi").is_floating());
    // REQUIRE(tbl.at("pi").as_floating() == std::Approx(3.14));
    REQUIRE(tbl.at("flag").is_boolean());
    REQUIRE(tbl.at("flag").as_boolean());
    REQUIRE(tbl.at("text").is_string());
    REQUIRE(tbl.at("text").as_string() == "hi");
}

TEST_CASE("Parse arrays", "[toml]")
{
    auto t = toml::parse("nums = [1, 2, 3]\n");
    const auto &arr = t.as_table().at("nums").as_array();
    REQUIRE(arr.size() == 3);
    REQUIRE(arr[0].as_integer() == 1);
    REQUIRE(arr[1].as_integer() == 2);
    REQUIRE(arr[2].as_integer() == 3);
}

TEST_CASE("Parse inline table", "[toml]")
{
    auto t = toml::parse("cfg = { host = \"localhost\", port = 8080 }\n");
    const auto &cfg = t.as_table().at("cfg").as_table();
    REQUIRE(cfg.at("host").as_string() == "localhost");
    REQUIRE(cfg.at("port").as_integer() == 8080);
}

TEST_CASE("Parse dotted keys", "[toml]")
{
    auto t = toml::parse("a.b.c = 1\na.d = 2\n");
    const auto &a = t.as_table().at("a").as_table();
    REQUIRE(a.at("b").as_table().at("c").as_integer() == 1);
    REQUIRE(a.at("d").as_integer() == 2);
}

TEST_CASE("Parse table header", "[toml]")
{
    auto t = toml::parse("[server]\nport = 8080\n[server.nested]\nflag = false\n");
    const auto &root = t.as_table();
    const auto &server = root.at("server").as_table();
    REQUIRE(server.at("port").as_integer() == 8080);
    REQUIRE(server.at("nested").as_table().at("flag").as_boolean() == false);
}

TEST_CASE("Dump round trip", "[toml]")
{
    auto t = toml::parse("[db]\nuser = \"alice\"\nport = 5432\n");
    auto dumped = t.dump();
    auto t2 = toml::parse(dumped);
    REQUIRE(t2.as_table().at("db").as_table().at("user").as_string() == "alice");
    REQUIRE(t2.as_table().at("db").as_table().at("port").as_integer() == 5432);
}
