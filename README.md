<p align="center"><h1 align="center">🚀 MYTOML : Modern C/C++ TOML Library</h1></p>

<p align="center">
    <em><code>Lightweight, modern, and easy-to-use C++ TOML serializer & deserializer.</code></em>
</p>
<p align="center">
    <img src="https://img.shields.io/github/license/djoezeke/mytoml?style=flat-square&logo=opensourceinitiative&logoColor=white&color=0080ff" alt="license">
    <img src="https://img.shields.io/github/last-commit/djoezeke/mytoml?style=flat-square&logo=git&logoColor=white&color=0080ff" alt="last-commit">
    <img src="https://img.shields.io/github/languages/top/djoezeke/mytoml?style=flat-square&color=0080ff" alt="repo-top-language">
    <img src="https://img.shields.io/github/languages/count/djoezeke/mytoml?style=flat-square&color=0080ff" alt="repo-language-count">
</p>

<!-- ## Table of contents -->

<details><summary>Table of Contents</summary>

- [📍 Overview](#-overview)
- [👾 Design Goals](#-design-goals)
- [👾 Features](#-features)
- [📁 Project Structure](#-project-structure)
- [🚀 Getting Started](#-getting-started)
  - [☑️ Prerequisites](#-prerequisites)
  - [🛠️ Integration](#-integration)
    <!-- - [git](#git-submodule) -->
    <!-- - [CMake](#cmake-fetchcontent) -->
  - [🤖 Usage](#-usage)
  <!-- - [🤖 Example](#-example) -->
  - [🧪 Testing](#-testing)
  - [⚡ Benchmarking](#-benchmarking)
- [📋 Known Limitations](REVIEW.md)
- [🚧 Improvement Roadmap](IMPROVE.md)
- [Support](#support)
- [🔰 Contributing](#-contributing)
  <!-- - [Contributors](#contributors) -->
- [🙌 Acknowledgments](#-acknowledgments)
  <!-- - [Used third party tools](#used-third-party-tools) -->
- [📄 License](#-license)

</details>

## 📍 Overview

<!-- ## About Mytoml -->

**Mytoml** is a modern, lightweight TOML parsing and serialization library for C++. It provides an easy-to-use interface for working with TOML data, including parsing TOML strings, creating TOML objects, and serializing TOML objects to strings.It provides a simple, type-safe API for working with TOML data in C++ projects.

<img align="right" src=".github/readme/TOML_logo.svg" width="200" alt="TOML Logo">

### [Toml](https://toml.io/en/) [Tom's Obvious Minimal Language]

#### A config file format for humans.

TOML aims to be a minimal configuration file format that's easy to read due to
obvious semantics. TOML is designed to map unambiguously to a hash table. TOML
should be easy to parse into data structures in a wide variety of languages.

More information about TOML can be obtained at [here](https://toml.io/en/)

<!--
#### Toml Example.

```toml
name = "Mytoml : Toml for Modern C/C++"
branches = [ "main", "dev" ]
author = "djoezeke"
published = true

[[tutorial]]
[tutorial.yaml]
name = "YAML Ain't Markup Language"
type = "awesome"
born = 2_001

[[tutorial]]
[tutorial.json]
name = "JavaScript Object Notation"
type = "great"
born = 2_001

[[tutorial]]
[tutorial.toml]
name = "Tom's Obvious Minimal Language"
type = "great"
born = 2_013

[[tutorial]]
[tutorial.xml]
name = "Extensible Markup Language"
type = "good"
born = 1_996
``` -->

<!-- ## Design Goals

Mytoml has been developed with these design goals:

- 💼 **Portable** The library depends only on C/C++ standards, and is carefully designed to work on many platforms so that Mytoml can be imported into existing C or C++ projects written in either C11, C++11 or later.
  Furthermore, the project supports [CMake](https://cmake.org/) and provides [the documentation exclusively for CMake integration](https://djoezeke.github.io/mytoml/tutorials/cmake_integration).
  Those characteristics allow existing C or C++ project using CMake for builds to quickly incorporate TOML support with just a little effort.

- 🪰 **Heavily Tested** Mytoml has been [unit-tested](https://github.com/djoezeke/mytoml/tree/main/tests/) and its test suite covers 100% of lines and conditions of the codebase. (You can see the actual coverage [here](#).)
  We check with [Valgrind](https://valgrind.org) and the [Clang Sanitizers](https://clang.llvm.org/docs/index.html) that there are no runtime issues such as memory leak.
  Furthermore, the quality of our codebase has been checked with [Clang-Tidy](https://releases.llvm.org/14.0.0/tools/clang/tools/extra/docs/clang-tidy/index.html), [CodeQL](https://codeql.github.com/docs/) and [Codacy](https://www.codacy.com/).
  [GitHub Actions](https://docs.github.com/en/actions) workflows run against every commit pushed on the main & develop branches to ensure that the fkYAML library can be successfully built/tested with a variety of compilers, operating systems and C/C++ standards.
  See the [prerequisites](#-prerequisites) section for more details.

- 📝 **Well Documented** [The documentation](https://djoezeke.github.io/mytoml/) provides plenty of information so that users can understand what Mytoml is and what they can achieve with Mytoml.
  For instance, [the tutorial pages](https://djoezeke.github.io/mytoml/tutorials/) show how you can read/write/customize using Mytoml to handle TOML documents while creating a simple C/C++ project using CMake.
  Also, [API reference pages](https://djoezeke.github.io/mytoml/api/) provide more detailed, exclusive descriptions with example usages for each Mytoml API.

## ✨ Features

- ⚡ **Fast**: Efficient parsing and serialization
- 🧩 **Type-safe**: Strong C++ type system for all TOML types
- 🛠️ **Easy API**: Intuitive interface for manipulating TOML objects and arrays
- 📝 **Modern C++**: Uses smart pointers, `std::variant`, and other C++17 features
- 🧪 **Tested**: Includes unit tests and examples

More detailed descriptions are to be found on [GitHub Pages](https://djoezeke.github.io/mytoml).

Here is the list you might want to know:

- [Tutorial](https://djoezeke.github.io/mytoml/tutorials) - getting started
- [API references](https://djoezeke.github.io/mytoml/api/) - all the details with examples

### 📓 Note
 -->

<details closed>

<summary> 📁 Project Structure</summary>

```sh
mytoml/
├── CMakeLists.txt
├── LICENSE
├── include/mytoml/
│   ├── mytoml.h
│   └── mytoml.hpp
├── src/
│   ├── mytoml.c
│   └── mytoml.cpp
├── tests/
│   ├── c/
│   ├── cxx/
│   └── bench/
└── examples/
    └── apis/
        └── Basic.cpp
```

</details>

## 🚀 Getting Started

### ☑️ Prerequisites

Please ensure your runtime environment meets the following requirements:

- **C++17** or newer compiler
- **CMake** (for building examples/tests)
- Optional C++20 modules build requires **CMake 3.28+** and a modules-capable compiler (MSVC 19.34+ or Clang 16+).

Mytoml uses following software, tools and services:

- [CMake](https://cmake.org/) as a general build tool
- (optional) [Catch2](https://github.com/catchorg/Catch2.git) for unit and performance testing
- (optional) [Mkdocs](http://) to build web documentation
- (optional) [Doxygen](http://www.doxygen.org) to build code documentation

### 🛠️ Integration

1. **Clone the repository:**

   ```sh
   git clone https://github.com/djoezeke/mytoml.git
   ```

2. **Add to your project:**

   - Copy `include/mytoml/mytoml.hpp` and `src/mytoml.cpp` into your project.
   - Or add as a submodule and include in your build.

3. **Include in your code:**

   ```cpp
   #include <mytoml/mytoml.hpp>
   ```

4. **Compile:**
   ```sh
   g++ my_source.cpp src/mytoml.cpp -std=c++17 -Iinclude -o main
   ```

To build the C++20 module target, configure CMake with `-DMYTOML_BUILD_MODULE=ON`.

Module example (requires module-enabled build):

```sh
cmake --preset modules
cmake --build build/modules --target ModuleExample
./build/modules/examples/apis/ModuleExample
```

<!-- #### Using [CMake](https://cmake.org/)

<details closed>
<summary>Configuring and Building</summary>

</details>

#### Using [Meson]()

<details closed>
<summary>Configuring and Building</summary>

</details>

#### Using [Makefile](https://www.gnu.org/software/make/)

<details closed>
<summary>Testing and Building</summary>

> [!NOTE]
> Makefile support is limited

</details> -->

## 🤖 Usage

A complete, compilable example is available in
[`examples/apis/Basic.cpp`](examples/apis/Basic.cpp). It covers parsing,
typed access, modification, emission, literals, streams, UTF helpers, and
error handling.

Build and run it with CMake:

```sh
cmake -S . -B build -DMYTOML_BUILD_EXAMPLES=ON
cmake --build build --target Basic
./build/Basic
```

`CMAKE_RUNTIME_OUTPUT_DIRECTORY` is set to the top of the build tree, so all
example executables land directly in `build/`.

### Parse a document

`mytoml::toml::parse()` accepts a complete TOML document as a `std::string`
and throws `mytoml::parse_error` when the input cannot be parsed.

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
tags = ["toml", "cpp"]

[server]
host = "127.0.0.1"
port = 8080
)";

    mytoml::toml config = mytoml::toml::parse(text);

    std::cout << config.as_table().at("title").as_string() << '\n';
    std::cout << config.as_table().at("server").as_table().at("port").as_integer() << '\n';
}
```

### Check types before reading values

Every node exposes `type()` plus `is_table()`, `is_array()`, `is_string()`,
`is_integer()`, `is_floating()`, and `is_boolean()`. Use these checks when a
document is untrusted:

```cpp
const auto &root = config.as_table();
const auto retries = root.find("retries");

if (retries != root.end() && retries->second.is_integer())
{
    std::cout << "retries = " << retries->second.as_integer() << '\n';
}
```

The accessors do not convert between types, except `as_floating()`, which also
accepts an integer node. Calling an accessor for the wrong type throws
`std::bad_variant_access`.

### Modify and emit

Tables are `std::map<std::string, toml>`, so new values can be inserted
directly. `dump()` serializes a table as a TOML document:

```cpp
config.as_table()["enabled"] = false;
config.as_table()["release"] = "0.1.0";

std::cout << config.dump();
```

New nested tables can be created with the `table_t` and `array_t` aliases:

```cpp
mytoml::toml::table_t database;
database["url"] = "sqlite://demo.db";
database["pool_size"] = int64_t{4};

config.as_table()["database"] = std::move(database);
```

### Arrays and inline tables

Arrays and inline tables are parsed recursively and can be nested:

```cpp
mytoml::toml doc = mytoml::toml::parse(R"(
tags = ["toml", "cpp", "config"]
meta = { name = "mytoml", stars = 5 }
)");

const auto &tags = doc.as_table().at("tags").as_array();
const auto &meta = doc.as_table().at("meta").as_table();

std::cout << tags.size() << ' ' << meta.at("name").as_string() << '\n';
```

Emission of an array of inline tables uses the TOML inline-table form:

```cpp
mytoml::toml::array_t people;
mytoml::toml::table_t alice;
alice["name"] = "Alice";
alice["age"] = int64_t{30};
people.emplace_back(std::move(alice));

mytoml::toml output{mytoml::toml::table_t{{"people", mytoml::toml{std::move(people)}}}};
std::cout << output.dump();
```

### Literal operator

The `_toml` literal parses a string at run time:

```cpp
using namespace mytoml::literals;

const mytoml::toml config = R"(
name = "from literal"
count = 2
)"_toml;
```

### Streams

`operator<<` dumps a node, and `operator>>` parses a stream into an existing
`toml` object:

```cpp
std::istringstream input("answer = 42\n");
mytoml::toml config;
input >> config;

std::cout << config.as_table().at("answer").as_integer() << '\n';
std::cout << config; // same output as config.dump()
```

### Error handling

```cpp
try
{
    const mytoml::toml doc = mytoml::toml::parse("key = \"unterminated");
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

> [!IMPORTANT]
> Mytoml currently implements a line-oriented subset of TOML 1.0. Features
> such as dates/times, multiline strings, quoted keys, arrays of tables, and
> inline comments are not supported. Unsupported values may also be accepted
> as bare strings instead of being rejected. See [REVIEW.md](REVIEW.md) for the
> full list of limitations before using the library with untrusted input.

### C API status

`include/mytoml/mytoml.h` currently exposes the `MYTOMLC_VERSION_*`
macros, the `mtEncoding` enum, and the `mtVersion` structure. There are no C
parsing or emission functions yet; use the C++ API for now. See
[IMPROVE.md](IMPROVE.md) for the planned C API work.

## 🧪 Testing

The test suite uses [Catch2](https://github.com/catchorg/Catch2) and lives in
`tests/`. Catch2 is vendored as a submodule under `tests/vendor/Catch2`.

```sh
# Configure with tests enabled and build the test executables.
cmake -S . -B build -DMYTOML_BUILD_TESTS=ON
cmake --build build --target mytoml-cpp-tests mytoml-c-tests

# Run through CTest.
ctest --test-dir build --output-on-failure

# Or run the C++ tests directly (all Catch2 options are available).
./build/mytoml-cpp-tests
./build/mytoml-cpp-tests "[parse]"
```

The C test target currently verifies the public C header's macros, enum, and
structure layout only.

## ⚡ Benchmarking

Catch2 micro-benchmarks are built when `MYTOML_BUILD_BENCHMARKS` is enabled.
The option defaults to the value of `MYTOML_BUILD_TESTS`, and benchmark
targets are not registered with CTest.

```sh
cmake -S . -B build -DMYTOML_BUILD_TESTS=ON -DMYTOML_BUILD_BENCHMARKS=ON
cmake --build build --target mytoml-cpp-benchmarks

./build/mytoml-cpp-benchmarks --benchmark-samples 10
./build/mytoml-cpp-benchmarks "parse*" --benchmark-samples 20
```

The benchmarks cover parsing and emitting small, flat, nested, and array-heavy
documents, table construction and lookups, and UTF-8 decode/encode helpers.

### Meson and Bazel

```sh
# Meson
meson setup builddir -Dmytoml_build_tests=true -Dmytoml_build_benchmarks=true
meson compile -C builddir
meson test -C builddir
meson test -C builddir --benchmark

# Bazel
bazel test //tests:mytoml_test //tests:mytomlc_test
bazel run //tests:mytoml_benchmarks
```

## Support

🤔 If you have a **question**, please check if it is already answered in the [**FAQ**](https://djoezeke.github.io/mytoml/faq/) or the [**Q&A**](https://github.com/djoezeke/mytoml/discussions/categories/q-a) section. If not, please [**ask a new question**](https://github.com/djoezeke/mytoml/discussions/new) there.

📓 If you want to **learn more** about how to use the library, check out the rest of the [**README**](#examples), have a look at [**code examples**](https://github.com/djoezeke/mytoml/tree/main/examples), or browse through the [**help pages**](https://djoezeke.github.io/mytoml/faq/).

🚧 If you want to understand the **API** better, check out the [**API Reference**](https://djoezeke.github.io/mytoml/api/).
🐞 If you found a **bug**, please check the [**FAQ**](https://djoezeke.github.io/mytoml/faq/) if it is a known issue or the result of a design decision. Please also have a look at the [**issue list**](https://github.com/djoezeke/mytoml/issues) before you [**create a new issue**](https://github.com/djoezeke/mytoml/issues/new/choose). Please provide as much information as possible to help us understand and reproduce your issue.

## 🤝 Contributing

- **💬 [Join the Discussions](https://github.com/djoezeke/mytoml/discussions)**: Share your insights, provide feedback, or ask questions.
- **🐛 [Report Issues](https://github.com/djoezeke/mytoml/issues)**: Submit bugs found or log feature requests for the `mytoml` project.
- **💡 [Submit Pull Requests](https://github.com/djoezeke/mytoml/blob/main/.github/CONTRIBUTING.md)**: Review open PRs, and submit your own PRs.

<details closed>
<summary>Contributing Guidelines</summary>

1. **Fork the Repository**: Start by forking the project repository to your github account.
2. **Clone Locally**: Clone the forked repository to your local machine using a git client.
   ```sh
   git clone https://github.com/djoezeke/mytoml
   ```
3. **Create a New Branch**: Always work on a new branch, giving it a descriptive name.
   ```sh
   git checkout -b new-feature-x
   ```
4. **Make Your Changes**: Develop and test your changes locally.
5. **Commit Your Changes**: Commit with a clear message describing your updates.
   ```sh
   git commit -m 'Implemented new feature x.'
   ```
6. **Push to github**: Push the changes to your forked repository.
   ```sh
   git push origin new-feature-x
   ```
7. **Submit a Pull Request**: Create a PR against the original project repository. Clearly describe the changes and their motivations.
8. **Review**: Once your PR is reviewed and approved, it will be merged into the main branch. Congratulations on your contribution!

See the [CONTRIBUTING.md](https://github.com/djoezeke/mytoml/blob/main/.github/CONTRIBUTING.md) file for detailed information.

</details>

<details closed>
<summary>Contributor Graph</summary>
<br>
<p align="left">
   <a href="https://github.com{/djoezeke/mytoml/}graphs/contributors">
      <img src="https://contrib.rocks/image?repo=djoezeke/mytoml">
   </a>
</p>
</details>

## 🙌 Acknowledgments

We would like to express our gratitude to the following projects and individuals whose work made this project possible:

- [**Catch2**](https://github.com/catchorg/Catch2) as a unit-test and micro-benchmark framework.
- [**Clang**](https://clang.llvm.org) for coding style checks, and static/runtime analysis.
- [**CMake**](https://cmake.org) for build automation.
- [**Material for MkDocs**](https://squidfunk.github.io/mkdocs-material/) for the style of the documentation site
- [**MkDocs**](https://www.mkdocs.org) for the documentation site.
- [**Valgrind**](https://valgrind.org) for memory leak check & correct memory management.
- [**GitHub Changelog Generator**](https://github.com/skywinder/github-changelog-generator) to generate the [ChangeLog]().

The open-source community for their invaluable libraries, tutorials, and support.

### With thanks to:

<!-- - **[](https://github.com/)** - Fixed a bug -->

> [!NOTE]
> If you feel your work should be acknowledged here, please open an issue or pull request.

### References

- [mytoml](https://github.com/djoezeke/myxml/) : A C/C++ XML Library.
- [myjson](https://github.com/djoezeke/myjson/) : A C/C++ JSON Library.
- [myyaml](https://github.com/djoezeke/myyaml/) : A C/C++ YAML Library.

### More Reading

## 📄 License

Mytoml is licensed under the [MIT](LICENSE) License. License, see [LICENSE](./LICENSE) for more information.

<!-- <picture>
  <source
    media="(prefers-color-scheme: dark)"
    srcset="
      https://api.star-history.com/svg?repos=mytoml/djoezeke&type=Date&theme=dark
    "
  />
  <source
    media="(prefers-color-scheme: light)"
    srcset="
      https://api.star-history.com/svg?repos=mytoml/djoezeke&type=Date
    "
  />
  <img
    alt="Star History Chart"
    src="https://api.star-history.com/svg?repos=mytoml/djoezeke&type=Date"
  />
</picture> -->
