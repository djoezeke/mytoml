#ifndef MYTOML_TESTS_TEST_SUPPORT_HPP
#define MYTOML_TESTS_TEST_SUPPORT_HPP

#include <catch2/catch_test_macros.hpp>
#include <mytoml/mytoml.hpp>

#include <initializer_list>
#include <string>
#include <string_view>

namespace testsupport
{
    inline const mytoml::toml &at(const mytoml::toml &node, std::string_view key)
    {
        REQUIRE(node.is_table());
        const auto &table = node.as_table();
        const auto it = table.find(std::string(key));
        REQUIRE(it != table.end());
        return it->second;
    }

    inline const mytoml::toml &at(const mytoml::toml &node,
                                  std::initializer_list<std::string_view> path)
    {
        const mytoml::toml *current = &node;
        for (const auto key : path)
        {
            current = &at(*current, key);
        }
        return *current;
    }

    inline std::string parse_error_what(const std::string &text)
    {
        try
        {
            (void)mytoml::toml::parse(text);
        }
        catch (const mytoml::parse_error &error)
        {
            return error.what();
        }
        return {};
    }

    inline mytoml::toml make_table(std::initializer_list<std::pair<const std::string, mytoml::toml>> values)
    {
        mytoml::toml::table_t table;
        for (const auto &value : values)
        {
            table.insert(value);
        }
        return mytoml::toml{std::move(table)};
    }
}

#endif // MYTOML_TESTS_TEST_SUPPORT_HPP
