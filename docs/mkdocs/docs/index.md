# Mytoml

Mytoml is a lightweight TOML parsing and serialization library for C++17. It
provides a small, type-safe API built on `std::variant`, `std::map`, and
`std::vector`.

!!! warning "TOML 1.0 subset"
    Mytoml currently implements a line-oriented subset of TOML 1.0. Dates and
    times, multiline strings, quoted keys, arrays of tables, and inline
    comments are not supported. See the repository's
    [REVIEW.md](https://github.com/djoezeke/mytoml/blob/main/REVIEW.md) for the
    complete list of limitations.

## Quick start

```cpp
#include <mytoml/mytoml.hpp>

#include <iostream>
#include <string>

int main()
{
    const std::string text = R"(
title = "Mytoml"
enabled = true
retries = 3
ratio = 0.75
tags = ["toml", "cpp", "config"]

[server]
host = "127.0.0.1"
port = 8080

[server.limits]
timeout = 30
)";

    mytoml::toml config = mytoml::toml::parse(text);

    std::cout << config.as_table().at("title").as_string() << '\n';
    std::cout << config.as_table().at("server").as_table().at("port").as_integer() << '\n';
    std::cout << config.as_table().at("tags").as_array().size() << '\n';
}
```

Compile it directly against the library sources:

```sh
g++ main.cpp src/mytoml.cpp -std=c++17 -Iinclude -o main
```

Or build the bundled example with CMake:

```sh
cmake -S . -B build -DMYTOML_BUILD_EXAMPLES=ON
cmake --build build --target Basic
./build/Basic
```

`CMAKE_RUNTIME_OUTPUT_DIRECTORY` is set to the top of the build tree, so
example executables are placed directly in `build/`.

## Parsing

`mytoml::toml::parse()` takes a complete document as a `std::string` and
returns a `mytoml::toml` value. A TOML document is a table at the root:

```cpp
mytoml::toml document = mytoml::toml::parse(R"(
name = "demo"
values = [1, 2, 3]

[owner]
name = "Sackey"
)");

const mytoml::toml::table_t &root = document.as_table();
```

Supported syntax includes:

- bare and dotted keys, such as `server.port = 8080`
- table headers, including dotted headers such as `[server.limits]`
- basic double-quoted strings
- integers, floating-point numbers, and booleans
- arrays, including nested arrays and inline tables
- inline tables, such as `meta = { name = "mytoml", stars = 5 }`
- full-line comments beginning with `#`

Unsupported constructs are described in
[REVIEW.md](https://github.com/djoezeke/mytoml/blob/main/REVIEW.md). Some
unsupported values are silently stored as strings, so validate untrusted
documents rather than assuming that parsing implies TOML compliance.

## Reading values

Each node exposes a `type()` query and a set of `is_*()` helpers:

```cpp
mytoml::toml value = /* ... */;

switch (value.type())
{
case mytoml::node_t::table:
    // ...
    break;
case mytoml::node_t::array:
    // ...
    break;
case mytoml::node_t::string:
    std::cout << value.as_string() << '\n';
    break;
case mytoml::node_t::integer:
    std::cout << value.as_integer() << '\n';
    break;
case mytoml::node_t::floating:
    std::cout << value.as_floating() << '\n';
    break;
case mytoml::node_t::boolean:
    std::cout << std::boolalpha << value.as_boolean() << '\n';
    break;
default:
    break;
}
```

Use `is_*()` when the document is not trusted:

```cpp
const auto &root = document.as_table();
const auto it = root.find("retries");

if (it != root.end() && it->second.is_integer())
{
    const int64_t retries = it->second.as_integer();
}
```

The accessors do not perform general conversions. `as_floating()` is the one
exception: it returns an integer node converted to `double`. Calling an
accessor for the wrong node type throws `std::bad_variant_access`, not a
`mytoml::exception`.

## Navigating tables and arrays

Tables and arrays use standard containers, so the usual C++ iteration patterns
apply:

```cpp
const mytoml::toml::table_t &root = document.as_table();

for (const auto &entry : root)
{
    std::cout << entry.first << " = " << entry.second.type() << '\n';
}

const mytoml::toml::array_t &tags = root.at("tags").as_array();
for (const mytoml::toml &tag : tags)
{
    std::cout << tag.as_string() << '\n';
}
```

`table_t` is a `std::map<std::string, toml>`, so keys are sorted in the
emitted document rather than preserved in input order.

## Modifying a document

Values can be inserted or replaced directly:

```cpp
document.as_table()["enabled"] = false;
document.as_table()["retries"] = int64_t{5};
document.as_table()["release"] = "0.1.0";
```

Nested tables are built with `toml::table_t`:

```cpp
mytoml::toml::table_t database;
database["url"] = "sqlite://demo.db";
database["pool_size"] = int64_t{4};

document.as_table()["database"] = std::move(database);
```

Arrays are built with `toml::array_t`:

```cpp
mytoml::toml::array_t features;
features.emplace_back("parse");
features.emplace_back("emit");
document.as_table()["features"] = std::move(features);
```

## Emitting

`dump()` serializes a table as a TOML document. `operator<<` calls `dump()`:

```cpp
std::cout << document.dump();
std::cout << document; // equivalent
```

Top-level scalars are emitted before nested tables, and nested tables are
emitted with `[table]` headers. An inline table is emitted with `{ key = value }`
syntax when it appears inside an array or another value:

```cpp
mytoml::toml::array_t people;
mytoml::toml::table_t alice;
alice["name"] = "Alice";
alice["age"] = int64_t{30};
people.emplace_back(std::move(alice));

mytoml::toml output{mytoml::toml::table_t{{"people", mytoml::toml{std::move(people)}}}};
std::cout << output.dump();
```

Emitted strings are written verbatim; the current emitter does not escape
quotes, backslashes, or control characters. See
[REVIEW.md](https://github.com/djoezeke/mytoml/blob/main/REVIEW.md).

## Literal operator

The `_toml` literal parses a string at run time:

```cpp
using namespace mytoml::literals;

const mytoml::toml config = R"(
name = "from literal"
count = 2
)"_toml;
```

Literal operators are available for `char`, `char8_t` (when the compiler
provides it), `char16_t`, and `char32_t` strings. The wide literal overloads
currently truncate each code unit to a single byte and do not perform a full
UTF conversion.

## Streams

`operator>>` parses from an input stream into an existing `toml` object:

```cpp
std::istringstream input("answer = 42\n");
mytoml::toml config;
input >> config;

std::cout << config.as_table().at("answer").as_integer() << '\n';
```

The extraction operator takes `const toml &` for historical reasons and
modifies the object through `const_cast`; prefer `parse()` in new code.

## Error handling

Errors raised by the library derive from `mytoml::exception`:

- `mytoml::parse_error` for invalid documents
- `mytoml::encoding_error` for encoding failures
- `mytoml::exception` as the common base class

```cpp
try
{
    mytoml::toml config = mytoml::toml::parse("key = \"unterminated");
    (void)config;
}
catch (const mytoml::parse_error &error)
{
    std::cerr << "parse error: " << error.what() << '\n';
}
catch (const mytoml::exception &error)
{
    std::cerr << "mytoml error: " << error.what() << '\n';
}
```

Many malformed values are currently accepted instead of rejected. For
example, a value that is not a recognized number falls back to a bare string,
and numeric conversion accepts a valid prefix such as `01abc`. Validate input
against your application's expectations and see
[REVIEW.md](https://github.com/djoezeke/mytoml/blob/main/REVIEW.md).

## Encoding helpers

Low-level UTF-8, UTF-16, and UTF-32 helpers live in `mytoml::detail`:

```cpp
const char check_mark[] = "\xE2\x9C\x93"; // U+2713

unsigned int codepoint = 0;
int consumed = mytoml::detail::utf8::decode(check_mark, sizeof(check_mark) - 1, codepoint);
if (consumed > 0)
{
    mytoml::detail::utf8::char_t buffer[4] = {};
    int written = mytoml::detail::utf8::encode(codepoint, buffer, sizeof(buffer));
    std::cout.write(reinterpret_cast<const char *>(buffer), written);
}
```

`mytoml::detail::determine_encoding()` recognizes byte-order marks for UTF-8,
UTF-16, and UTF-32. The parser itself does not strip BOMs or decode UTF-16/32
input; encoding conversion is currently available only through the helpers and
the lexer.

## C API

`include/mytoml/mytoml.h` currently provides the `MYTOMLC_VERSION_*` macros,
the `mtEncoding` enum, and the `mtVersion` structure. It does not yet expose
parsing or emission functions. Use the C++ API for now; the planned C API is
described in [IMPROVE.md](https://github.com/djoezeke/mytoml/blob/main/IMPROVE.md).

## Testing and benchmarks

```sh
# Unit tests
cmake -S . -B build -DMYTOML_BUILD_TESTS=ON
cmake --build build --target mytoml-cpp-tests mytoml-c-tests
ctest --test-dir build --output-on-failure

# Micro-benchmarks
cmake -S . -B build -DMYTOML_BUILD_TESTS=ON -DMYTOML_BUILD_BENCHMARKS=ON
cmake --build build --target mytoml-cpp-benchmarks
./build/mytoml-cpp-benchmarks --benchmark-samples 10
```

The test suite uses Catch2, which is vendored under `tests/vendor/Catch2`.
Benchmark targets are built separately and are not registered with CTest.
