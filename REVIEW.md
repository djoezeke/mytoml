# Review: Limitations and Known Issues

**Reviewed version:** 0.1.0
**Review date:** 2026-11-17
**Scope:** C++ parser/emitter, public C/C++ headers, encoding helpers, build
integration, tests, and benchmarks.

**Verification environment:** Windows, GCC 16.2 (MSYS2 UCRT64), Release build,
Catch2 3.15.1 (vendored). The C++ suite passes 55 test cases / 584 assertions.
Benchmark figures quoted below were measured with `--benchmark-samples 10` and
are indicative only.

> **Bottom line:** Mytoml is a useful prototype with a small, readable API, but
> it is not yet a TOML 1.0 compliant parser. It accepts a line-oriented subset
> and silently misreads several valid TOML constructs. It should not be used
> for untrusted input or for configuration files whose values matter until the
> correctness issues in this document are addressed.

---

## 1. Feature support matrix

| TOML 1.0 feature | Status | Notes |
| --- | --- | --- |
| Bare keys / key-value pairs | Partial | Bare keys may contain spaces; quoted keys are not decoded |
| Dotted keys | Partial | Works for simple `a.b = 1`; empty segments are rejected |
| Standard tables `[a.b]` | Partial | Works, but redefinitions are merged instead of rejected |
| Arrays of tables `[[a]]` | **No** | Parsed as a table whose name is literally `[a]` |
| Basic strings `"..."` | Partial | Only `\n`, `\t`, `\"`, `\\` escapes; no `\u`/`\U` |
| Literal strings `'...'` | **No** | Stored as a bare string including the quotes |
| Multiline strings `"""..."""` / `'''...'''` | **No** | Not supported by the line parser |
| Integers | Partial | Decimal prefix parsing; underscores, hex, octal, binary are misread |
| Floating-point numbers | Partial | Prefix parsing via `std::stod`; special values become strings |
| Booleans | Yes | Lowercase `true` / `false` only |
| Offset/local date-time | **No** | Misread as the leading integer (e.g. year) |
| Arrays | Partial | Single-line only; nested arrays and inline tables work |
| Inline tables | Partial | Single-line only; no dotted keys; duplicate keys overwrite |
| Comments | Partial | Full-line comments only; no trailing comments |
| Duplicate key rejection | **No** | Last assignment wins silently |
| Table redefinition rejection | **No** | Repeated headers merge or replace values |
| BOM / UTF-16 / UTF-32 input | **No** in `parse` | Helpers exist, but the parser bypasses them |
| Round-trip preservation | **No** | Formatting/comments lost; float and string values can change |
| C API | **No** | Header types only; no parse/emit functions |

---

## 2. Parser

`mytoml::toml::parse()` (`src/mytoml.cpp`) is a hand-written, line-oriented
parser built on `std::getline`, `std::istringstream`, string trimming, and
`std::map`. It does **not** use the lexer or input adapters declared in the
header.

### 2.1 Numeric parsing silently changes values

Number detection only looks for `.`, `e`, or `E`. If found, `std::stod` is
used; otherwise `std::stoll` is used. Both functions accept a valid prefix and
ignore trailing characters, so malformed and unsupported TOML values are
silently converted to a different value instead of being rejected. If the
conversion throws, the raw text becomes a bare string.

Observed behavior:

| Input | Parsed result | Expected TOML 1.0 behavior |
| --- | --- | --- |
| `x = 0x10` | integer `0` | integer `16` |
| `x = 1_000` | integer `1` | integer `1000` |
| `x = 0123` | integer `123` | invalid (leading zero) |
| `x = 1979-05-27` | integer `1979` | local date |
| `x = 1979-05-27T07:32:00Z` | integer `1979` | offset date-time |
| `x = 12abc` | integer `12` | invalid |
| `x = 1.2.3` | floating `1.2` | invalid |
| `x = 1 # comment` | floating `1.0` | integer `1` plus a comment |
| `x = nan` | string `"nan"` | floating-point NaN |
| `x = inf` | string `"inf"` | floating-point infinity |
| `x = 9223372036854775808` | string | invalid (out of `int64_t` range) |

The `1 # comment` case is particularly surprising: the `e` in the comment
triggers floating-point detection, and `std::stod` parses the leading `1`.

This is a correctness and security issue: a configuration value can change
meaning without any diagnostic. Numeric conversion must consume the entire
token and reject unsupported forms.

### 2.2 Strings

`parse_string()` requires the value to start and end with `"` and processes
only four escapes:

| Escape | Result |
| --- | --- |
| `\n` | newline |
| `\t` | tab |
| `\"` | double quote |
| `\\` | backslash |
| any other `\x` | `x` (backslash is dropped) |

Consequences:

- `"\u0041"` becomes `"u0041"`, not `"A"`.
- `"\r"`, `"\b"`, and `"\f"` become the letters `r`, `b`, and `f`.
- `\U0001F600` becomes `U0001F600`.
- Literal strings (`'text'`) are treated as bare strings and retain their
  single quotes.
- Multiline strings are not supported.
- Control characters and newlines can be introduced through escapes, but the
  emitter does not escape them again (see §3).

A trailing comment after a string value throws `parse_error`, because the
trimmed line no longer ends with `"`:

```toml
name = "demo" # comment   # throws "invalid string literal"
```

### 2.3 Keys and tables

- Quoted keys are not decoded. `"a b" = 1` is stored under the key `"a b"`
  including the quote characters; access requires `table.at("\"a b\"")`.
- Bare keys with embedded spaces are accepted (`a b = 1`), although TOML only
  permits `A-Za-z0-9_-`.
- A quoted key containing `=` is split at the first `=`, producing a corrupted
  key/value pair.
- Empty keys, empty table names, and trailing dotted-key segments are rejected.
  This was hardened during the current test work and prevents crashes that were
  previously reachable (`= 1`, `[]`, `a. = 1`).
- Duplicate keys overwrite earlier values silently; TOML requires an error.
- Repeated table headers merge silently; TOML forbids redefining a table.
- Assigning a scalar and then opening a table with the same name replaces the
  scalar with a table.
- Inline tables can be extended later with dotted keys, which TOML forbids.
- `[[fruit]]` is parsed as a table named `[fruit]`. Emitting it produces an
  `[[fruit]]` header, but it remains a single table, not an array of tables.
- A trailing comment after a table header throws `expected '=' in assignment`.

### 2.4 Arrays and inline tables

- Arrays and inline tables must fit on one line. A newline inside an array
  throws `unterminated array`.
- Commas inside quoted strings and braces/brackets in nested values are handled
  by `split_top_level`.
- Trailing commas are accepted for both arrays and inline tables. Trailing
  commas in inline tables are not TOML 1.0 compliant (they are a TOML 1.1
  proposal).
- Comments inside arrays are not recognized and become part of element values.
- Inline-table keys are stored verbatim; `{ a.b = 1 }` creates a key named
  `a.b` rather than a nested table.
- Duplicate inline-table keys overwrite silently.
- Empty entries are skipped rather than reported.

### 2.5 Comments and whitespace

Only lines whose first non-whitespace character is `#` are ignored. Inline
comments are not supported. Depending on the value type, an inline comment is:

- included in a bare-string fallback,
- swallowed by numeric prefix parsing and may change the node type,
- or the cause of a `parse_error` (for strings and table headers).

Comments are not stored, so they cannot be preserved when re-emitting.

### 2.6 Encoding and BOM handling

- `toml::parse()` operates on raw bytes and performs no encoding detection.
- A UTF-8 BOM is not stripped; it becomes part of the first key.
- UTF-16/UTF-32 documents are not decoded by `parse()`.
- `detail::determine_encoding()` recognizes BOMs only. It does not implement
  the “best-effort inspection of short sequences” described in its comment.
- The lexer can detect and convert BOM-marked UTF-16/32 input, but the parser
  does not use the lexer, so that support is unreachable through the public
  parse path.

### 2.7 Error reporting

- `parse_error` messages include `(line: 0, column: 0, index: 0)` for parser
  failures because `parse()` throws message-only errors; the position-aware
  constructor exists but is not used.
- Unsupported syntax is frequently accepted as a bare string rather than
  rejected. Examples: `x = hello`, `x = nan`, `x = True`, `x = null`.
- Several failure modes are silent: duplicate keys, table redefinition, and
  numeric prefix truncation.
- Accessor misuse throws `std::bad_variant_access` and map lookups throw
  `std::out_of_range`; these do not derive from `mytoml::exception`.
- `detail::error_t`, `detail::event_t`, and `detail::value_t` are effectively
  unused.

---

## 3. Emitter (`dump`)

`toml::dump()` serializes tables as documents. It does not preserve comments,
formatting, key order (tables are `std::map`), or the original numeric spelling.

### 3.1 Strings and keys are not escaped

- String values are written verbatim between quotes:
  - `a"b` is emitted as `a = "a"b"`, which is invalid TOML.
  - A newline in a value is emitted as a literal line break inside the quotes.
  - Backslashes are not escaped.
- Keys are written verbatim:
  - A key containing a space produces an invalid bare key.
  - A key containing `.` changes the document structure when re-parsed.
  - Quoted keys retained by the parser are emitted with their quotes, which is
    often valid but represents the quote characters as part of the key.
- Control characters are never escaped.

### 3.2 Numeric output

- Floating-point values use the default `std::ostream` formatting (6
  significant digits by default): `1.2345678901234567` dumps as `1.23457`.
- Integral floating-point values lose their type on round-trip: `1.0` dumps as
  `1`, which parses back as an integer. `-0.0` dumps as `-0`.
- Constructed `inf`/`nan` doubles dump as `inf`/`nan`; re-parsing produces
  strings because those tokens are not recognized by `parse_value`.
- Output is locale-sensitive because `std::ostringstream` uses the global
  locale; a comma decimal separator would produce invalid TOML.

### 3.3 Structure

- `indent` is accepted but ignored.
- A table always dumps as a document. Inline-table output (`{ key = value }`)
  is only reachable for a table nested inside an array or another value.
- Arrays are emitted on one line with `, ` separators.
- Arrays of tables cannot be represented; each table is emitted inline.
- A default-constructed/null node dumps as `null`, which TOML does not define.
- Empty root tables produce an empty string; nested empty tables produce a
  header only.
- There is no public formatting option beyond the ignored `indent` argument.

---

## 4. Public C++ API

- `toml` is a thin wrapper around
  `std::variant<std::nullptr_t, table_t, array_t, std::string, int64_t, double, bool>`.
- There is no `is_null()` query; the null alternative reports
  `node_t::unknown`.
- `as_table()`, `as_array()`, `as_string()`, `as_integer()`, and
  `as_boolean()` throw `std::bad_variant_access` on a type mismatch.
  `as_floating()` is inconsistent: it converts integers to `double`.
- There are no checked accessors returning `std::optional`, no visitor, and no
  iterators directly on `toml`; callers use the exposed containers.
- `parse()` accepts only `std::string`; there is no `std::string_view`, file,
  or adapter overload. A stream overload exists via `operator>>`.
- `operator>>(std::istream &, const toml &)` takes a const reference and
  modifies the object through `const_cast`. The signature should be changed to
  `toml &`.
- There is no equality/comparison operator for `toml`.
- Wide literal operators (`char16_t`, `char32_t`) truncate each code unit with
  `static_cast<char>` instead of performing a real UTF conversion.
- The `toml` class comment still calls it a “Lightweight JSON value holder” and
  a placeholder; documentation and API intent need to be reconciled.

---

## 5. C API

- `include/mytoml/mytoml.h` declares `mtEncoding`, `mtVersion`, and the
  `MYTOMLC_VERSION_*` macros, but **no functions**.
- `src/mytoml.c` contains no executable code beyond an empty `extern "C"`
  block.
- The C test target therefore only verifies compilation, enum values, and
  struct layout.
- The CMake C target links against the C++ library but exposes no parsing or
  emission capability.
- Users needing C support must currently use the C++ API from C++ (or write
  their own bridge).

---

## 6. Configuration macros

| Macro | Status |
| --- | --- |
| `MYTOML_DISABLE_READER` | Documented, but has no effect |
| `MYTOML_DISABLE_WRITER` | Documented, but has no effect |
| `MYTOML_NO_STL` | Not supported; implementation uses STL unconditionally |
| `MYTOML_NO_EXCEPTIONS` | Not supported; parser and accessors throw directly |
| `MYTOML_SKIP_VERSION_CHECK` | Works, but the version check target is stale (see §10) |
| `MYTOML_BUILD_CXX11` / `CXX14` | Options exist, but the implementation requires C++17 |
| `MYTOML_BUILD_C11` / `C17` / `C23` | C target compiles but exposes no functionality |

The public header also includes `<iostream>`, `<optional>`, and `<variant>`
unconditionally, so the “no STL” configuration is not a realistic build mode
today.

---

## 7. Encoding helpers

The UTF helpers are usable for simple valid input but do not perform strict
Unicode validation.

### 7.1 Validation gaps

- `utf8::decode()`:
  - does not verify continuation-byte prefixes,
  - accepts overlong encodings,
  - accepts UTF-8 encoded surrogates,
  - does not reject code points above U+10FFFF when encoded in 4 bytes.
- `utf8::encode()` accepts surrogate code points (U+D800–U+DFFF) and emits
  invalid UTF-8 for them; it does reject values above U+10FFFF.
- `utf16::decode()` treats an unpaired low surrogate as a normal code point.
- `utf16::encode()` encodes surrogate code points below U+10000 as single
  units and returns the number of code units, not bytes.
- `utf32::decode()` accepts any 32-bit value, including surrogates and values
  above U+10FFFF.
- `utf32::encode()` accepts any 32-bit value; it does not reject invalid code
  points.
- `to_utf16()` / `to_utf32()` rely on `MYTOML_ASSERT` for error handling. With
  `NDEBUG` (typical Release builds), asserts disappear. An invalid UTF-8
  sequence after the first character can wrap the index (`idx += size_t(-1)`)
  and loop or misconvert instead of reporting an error.
- The `endian` fallback enum uses `native = little` for non-GCC compilers when
  `std::endian` is unavailable, which is wrong on big-endian targets.

### 7.2 Size and return-value documentation mismatches

- `utf8::encode()` returns the number of bytes written (1–4).
- `utf16::encode()` returns the number of UTF-16 code units (1 or 2), while
  the header comment says it returns bytes (2 or 4).
- `utf16::encode()` and `utf32::encode()` interpret `size` as bytes, while the
  `utf32::encode()` comment describes it as a number of `char32_t` entries.
- `utf32::encode()` returns 1 unit but the comment says “1 == 4 bytes”.

Callers should treat this low-level API as unstable.

---

## 8. Lexer, adapters, and internal completeness

- `detail::lexer` is JSON-oriented: it emits `name_separator` for `:`, has a
  `null_literal`, and uses `object_start` / `object_end`. It is not used by the
  TOML parser.
- `detail::lexer::advance()` and `reverse()` use a
  `do { ... } while (times != amount)` loop whose counter is never
  incremented. They work only for the default `amount == 1`; any other value
  loops forever. Index/column underflow is also possible in `reverse()`.
- `emitter`, `serializer`, `oadapter`, and related types are declared but the
  public serialization path bypasses them; `dump()` is implemented separately.
- The token/event/value `string()` helpers for `mark`, `event`, and `token`
  return empty strings.
- Duplicate implementations (lexer/parser, emitter/dump) increase maintenance
  cost and have diverged from TOML semantics.

---

## 9. Robustness and security

- **Silent misinterpretation** of valid TOML numeric/date forms is the most
  serious issue. Values can change without an error.
- **No input size or nesting limits.** Nested arrays, inline tables, and
  emitted tables recurse without a depth guard; malicious input can exhaust the
  stack.
- **No duplicate/redefinition checks.** Conflicting keys overwrite silently.
- **No full-token validation** for numbers, dates, or bare strings.
- **No fuzzing** has been performed, and no sanitizer/Valgrind run is part of
  the current verification.
- **Emitter does not escape output**, so constructing a document from untrusted
  strings can produce invalid TOML or inject new keys.
- **Locale sensitivity** in `std::stod`/`std::stoll` and stream formatting can
  change parsing/emission behavior.
- Parsing itself uses only local state and appears reentrant, but no formal
  thread-safety guarantees are documented.

---

## 10. Performance observations

Measured on the verification machine (Release, `--benchmark-samples 10`):

| Benchmark | Mean time |
| --- | --- |
| Parse small configuration | ~9.6 µs |
| Parse flat configuration (2000 keys) | ~1.43 ms |
| Parse nested configuration (200 tables, 2000 values) | ~1.37 ms |
| Parse 1000-element array | ~137 µs |
| Emit small configuration | ~2.2 µs |
| Emit flat configuration (2000 keys) | ~148 µs |
| Emit nested configuration (200 tables, 2000 values) | ~144 µs |
| Emit 1000-element array | ~37 µs |
| Lookup nested string value | ~43 ns |
| Construct a small table | ~399 ns |
| UTF-8 decode/encode per code point | ~2.5–4.2 ns |

Observations:

- Parsing is roughly an order of magnitude slower than emitting for large
  documents because it builds an `std::istringstream`, copies/trims every line,
  and inserts into `std::map`.
- No `reserve`, `std::string_view`, or `std::from_chars` optimization is used.
- Benchmark coverage omits error paths, very long strings, deep nesting,
  UTF-16/32 documents, concurrent use, and the C API.
- Benchmarks are not run in CI and there is no regression threshold or stored
  baseline. Results vary by machine and compiler.

---

## 11. Tests, build, and documentation

### Tests

- 55 C++ test cases / 584 assertions pass in the verification environment.
- Coverage includes parsing, emitting, literals, streams, errors, values,
  version/enum stringification, encoding helpers, and regression cases for
  empty/trailing dotted key segments.
- Unsupported TOML 1.0 syntax is intentionally not asserted as working; it is
  documented here instead.
- The C test target checks only header-level declarations.
- There is no TOML compliance suite (e.g. `toml-test`), no fuzzing, no coverage
  gate, and no sanitizer job in this review environment.
- CI runs CMake/CTest, Meson, and Bazel tests, but not the benchmarks.

### Build system

- The C target source reference was corrected from `src/mytomlc.cpp` to
  `src/mytoml.c` in CMake, Meson, and Bazel.
- Meson now lists all C++ test sources; Bazel now lists them explicitly and
  exposes a benchmark binary.
- The Meson option `mytoml_cpp_std` is declared but never applied to the
  build (`add_project_arguments` is absent).
- `MYTOML_BUILD_CXX11` / `CXX14` advertise standards that cannot compile the
  current implementation.
- The top-level `Makefile` is stale: it references `examples/sample.c` and
  `tests/*.c` files that do not exist, and uses `-std=c23` for examples.
- The CMake option `MYTOML_BUILD_BENCHMARKS` is new and works; benchmark
  targets are deliberately not registered with CTest.

### Documentation

- The README previously described features and build steps that did not match
  the tree; it now documents the supported subset and links here.
- The MkDocs page was a single heading and now contains usage examples.
- API documentation comments often describe intended behavior rather than
  implemented behavior (encoding detection, inline tables, exceptions).
- There is no changelog and no documented compatibility/ABI policy.

### Versioning

- The header defines version `0.1.0` (`MAJOR=0`, `MINOR=1`, `PATCH=0`); the C
  header defines the same values through `MYTOMLC_VERSION_*`.
- CMake defines `MYTOML_VERSION_PATCH 1` but sets
  `MYTOML_VERSION_STRING`/project version to `0.1.0`.
- The C++ header’s version-mismatch check compares against `1.0.0`, so it warns
  for any other version pair, including the current one. The C header’s check
  correctly compares against `0.1.0`.
- `mytoml::version` is a generic semver value type; it is not populated from
  the library version and there is no `version()` function returning the
  compiled library version.

---

## 12. Issues fixed during the current test/documentation work

These were found while adding the test suite and are already resolved:

- Empty keys, empty table names, and trailing dotted-key segments no longer
  dereference invalid memory; they throw `parse_error` instead.
- The literal operator declarations no longer use `MYTOML_INLINE`
  (`always_inline`) for out-of-line definitions; they use `MYTOML_API`.
- `src/mytoml.c` no longer includes the C++-only header, so the C target
  compiles as C.
- The C++ literal declarations and definitions link correctly across
  translation units.
- Test and benchmark targets are wired into CMake, Meson, and Bazel, and the
  example is implemented and builds.

---

## 13. Recommended priority

1. **Correctness:** replace prefix-based numeric parsing with full-token
   parsing; reject unsupported syntax; implement dates/times; reject duplicate
   keys and table redefinitions.
2. **Emitter safety:** escape strings and keys; preserve float type/precision;
   reject or represent null explicitly.
3. **API correctness:** change `operator>>` to take `toml &`; add checked
   accessors; stop leaking `std::bad_variant_access`/`std::out_of_range`.
4. **C API:** implement the advertised surface or explicitly mark it
   experimental/unsupported.
5. **Robustness:** add depth/size limits, strict UTF validation, fuzzing, and
   sanitizer coverage.
6. **Performance:** remove the line/stream/map hot paths and add regression
   benchmarks.
7. **Packaging:** unify the version source, apply the Meson C++ standard
   option, and repair or remove the stale Makefile.

See [IMPROVE.md](IMPROVE.md) for a concrete roadmap.
