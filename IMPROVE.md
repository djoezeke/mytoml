# Improvement Roadmap

This document proposes a path from the current 0.1.0 prototype to a dependable
TOML library. It is based on the limitations documented in
[REVIEW.md](REVIEW.md). Priorities favor correctness and safety before
features and performance.

## Guiding principles

1. **Never silently change a value.** Unsupported or malformed input must
   produce an error with a source position, not a fallback string or a
   truncated number.
2. **TOML 1.0 compatibility is the contract.** “Close enough” parsing is worse
   than an explicit, documented subset.
3. **The emitter must produce valid TOML for every value the API can hold.**
   Round-trip tests should enforce this.
4. **C and C++ APIs share one implementation and one test corpus.**
5. **Correctness, then diagnostics, then performance.** Optimize only after
   the parser and emitter are specified and covered by tests.
6. **No dead configuration.** Every advertised macro and build option either
   works or is removed.

---

## Milestone 0 — Stabilize the current API

**Goal:** remove misleading behavior and make the existing subset safe to use.

| Item | Details | Priority |
| --- | --- | --- |
| Full-token number parsing | Use `std::from_chars`/`std::to_chars` or equivalent; require the entire token to be consumed; reject `0x10`, `1_000`, `0123`, `12abc`, and dates until implemented | P0 |
| Strict fallback policy | Remove the “bare string” fallback; unknown values must raise `parse_error` | P0 |
| Emitter escaping | Escape `"`, `\`, control characters, and newlines; quote keys when they are not valid bare keys | P0 |
| Float round-trip | Emit shortest round-trip representation; keep `1.0` as a float (`1.0`), handle `-0.0`, `inf`, and `nan` | P0 |
| `operator>>` signature | Change to `std::istream &operator>>(std::istream &, toml &)`; remove `const_cast` | P0 |
| Accessor errors | Add checked accessors (`get_if`, `as_integer` returning `optional`/`expected`) or document and consistently use `std::bad_variant_access` | P1 |
| Null representation | Remove `std::nullptr_t` from the public value model, or add `node_t::null` and a `dump` policy that cannot emit invalid TOML | P1 |
| C API honesty | Either implement the C API or mark the header experimental and remove the empty library target | P1 |
| Feature macros | Implement `MYTOML_DISABLE_READER`/`WRITER` or delete them; remove unsupported `MYTOML_NO_STL`/`MYTOML_NO_EXCEPTIONS` claims | P1 |
| Version source | Generate one version header from CMake; fix the `0.1.0` vs patch `1` mismatch and the stale `1.0.0` check | P1 |
| Regression tests | Add tests for every behavior changed above, including invalid-input cases that must throw | P0 |
| Documentation | Update README/API comments to state the exact subset and error behavior | P1 |

**Acceptance criteria:** all existing tests pass; every malformed numeric value
throws; emitted strings/keys re-parse to the same values; no public API
pretends to support a feature that is absent.

---

## Milestone 1 — Implement a real TOML 1.0 parser

**Goal:** pass the official `toml-test` compliance suite for TOML 1.0.

### 1.1 Architecture

Replace the line-oriented parser with a proper pipeline:

```
bytes -> encoding layer -> lexer -> parser -> document builder
```

- **Encoding layer:** detect and strip UTF-8/UTF-16/UTF-32 BOMs, decode input
  to UTF-8, reject invalid byte sequences with position information.
- **Lexer:** produce tokens with source spans (line, column, byte offset):
  bare/quoted keys, strings, numbers, booleans, dates/times, punctuation,
  comments, and newlines.
- **Parser:** implement the TOML grammar with explicit context for current
  table, dotted keys, inline tables, arrays, and arrays of tables.
- **Document builder:** enforce duplicate-key and table-redefinition rules,
  manage implicit tables, and reject invalid transitions with precise errors.
- **Depth and size limits:** configurable recursion limits and maximum input
  size to avoid stack exhaustion and unbounded allocation.

The existing `detail::lexer`, adapters, and emitter declarations can be reused
only after they are rewritten to TOML semantics; otherwise remove them.

### 1.2 Syntax checklist

- [ ] Bare keys limited to `A-Za-z0-9_-`
- [ ] Quoted keys with full basic/literal string escaping
- [ ] Dotted keys and dotted table headers, including whitespace rules
- [ ] Standard tables, implicit tables, and redefinition errors
- [ ] Arrays of tables with correct nesting and ordering
- [ ] Basic strings, including `\b`, `\f`, `\r`, `\uXXXX`, `\UXXXXXXXX`,
      escaped line endings, and rejection of invalid escapes
- [ ] Multiline basic strings with trimming and line-ending backslash
- [ ] Literal and multiline literal strings
- [ ] Decimal integers with underscores, `+`/`-`, and int64 range checks
- [ ] Hexadecimal, octal, and binary integers
- [ ] Floats with underscores, exponents, `inf`, `nan`, and range rules
- [ ] Offset date-time, local date-time, local date, and local time
- [ ] Arrays: multiline, comments, trailing commas, nesting
- [ ] Inline tables: single-line rule, no trailing comma, no dotted extension
- [ ] Comments after values and table headers
- [ ] UTF-8 input validation and BOM handling
- [ ] CRLF and LF handling
- [ ] Duplicate key/table errors with source positions

### 1.3 Diagnostics

- Introduce an error type carrying a message, line, column, byte offset, and
  an optional source snippet.
- Use the position-aware `parse_error` constructor everywhere.
- Provide error codes (`error_t`) for programmatic handling and a
  human-readable message for users.
- Add a non-throwing `try_parse`/`expected<toml, parse_error>` API.

**Acceptance criteria:** `toml-test` passes for valid and invalid cases;
fuzzing finds no crashes or hangs; errors always include a useful location.

---

## Milestone 2 — Correct and complete emitter

| Item | Details | Priority |
| --- | --- | --- |
| String escaping | Escape quotes, backslashes, control characters, and line breaks; choose basic vs literal strings safely | P0 |
| Key emission | Emit bare keys where legal, otherwise quoted keys with escaping | P0 |
| Float formatting | Shortest round-trip representation; preserve float type and negative zero; emit `inf`/`nan` consistently | P0 |
| Date/time emission | RFC 3339 formatting with correct precision and offset | P1 |
| Arrays of tables | Emit `[[table]]` sections and preserve order | P1 |
| Inline tables | Emit inline or document form on request; validate values | P1 |
| Indentation | Honor `indent` or remove the parameter | P2 |
| Formatting options | Optional key sorting, indentation width, newline style, and inline thresholds | P2 |
| Comments | Preserve comments only if the parser stores them; otherwise document that they are lost | P2 |
| Round-trip mode | Guarantee `parse(dump(x)) == x` for every supported value, including type | P0 |

**Acceptance criteria:** every emitted document re-parses to an equal value;
emitted output passes `toml-test` valid cases; no public value can produce
invalid TOML.

---

## Milestone 3 — Value model and C++ API

### 3.1 Value model

- Add a first-class `datetime` value (offset date-time, local date-time, local
  date, local time) instead of parsing dates loosely.
- Decide the table representation:
  - keep `std::map` for deterministic sorted output, or
  - add an ordered table preserving insertion order (TOML order is
    semantically meaningful for arrays; table order is not, but users often
    expect it).
- Remove `std::nullptr_t` from the public variant unless a documented “null”
  extension is desired.
- Add equality (`operator==`) and a stable comparison strategy for testing and
  round-trip assertions.

### 3.2 API surface

```cpp
// Proposed additions
toml toml::parse(std::string_view text);
toml toml::parse(std::istream &stream);
toml toml::parse_file(const std::filesystem::path &path); // or std::string

std::optional<toml> try_parse(std::string_view text) noexcept;
// or: std::expected<toml, parse_error> try_parse(std::string_view text);

const toml *find(std::string_view dotted_key) const;
toml *find(std::string_view dotted_key);

// Checked access
std::optional<std::string_view> as_string_if() const;
std::optional<int64_t> as_integer_if() const;
std::optional<double> as_floating_if() const;
std::optional<bool> as_boolean_if() const;

// Traversal
class value_iterator; // optional, for ergonomic traversal
```

- Make `parse` accept `std::string_view` to avoid copies.
- Make `operator>>` non-const and integrate diagnostics.
- Keep the current accessors for compatibility but document their throwing
  behavior; add non-throwing alternatives.
- Replace the wide literal truncation with real UTF conversion.
- Add a `version()` function returning the compiled library version (and keep
  the `version` value type, or merge them deliberately).
- Provide `operator==` for `toml` and the container aliases.

**Acceptance criteria:** no `const_cast`; no STL exception leaks from documented
API paths; all new overloads are covered by tests and examples.

---

## Milestone 4 — C API

**Goal:** make `include/mytoml/mytoml.h` genuinely usable from C11/C17.

Design sketch (opaque handles, no exceptions across the boundary):

```c
typedef struct mtDocument mtDocument;
typedef struct mtError { int code; size_t line; size_t column; char message[256]; } mtError;

mtDocument *mt_parse(const char *data, size_t size, mtError *error);
void mt_free(mtDocument *document);

int mt_get_type(const mtDocument *document);
const mtDocument *mt_table_get(const mtDocument *table, const char *key);
const mtDocument *mt_array_get(const mtDocument *array, size_t index);

int mt_as_integer(const mtDocument *value, int64_t *out);
int mt_as_double(const mtDocument *value, double *out);
int mt_as_bool(const mtDocument *value, int *out);
const char *mt_as_string(const mtDocument *value, size_t *length);

char *mt_dump(const mtDocument *document, size_t *length);
void mt_string_free(char *string);
```

Implementation options:

1. Wrap the C++ value in an opaque `mtDocument` allocated in C++ and expose
   only C functions. This keeps one implementation.
2. Add a separate C implementation only if C-only builds are a hard
   requirement; duplicating the parser would be expensive to maintain.

Also provide:

- error codes matching the C++ `error_t`,
- version functions returning `mtVersion`,
- ownership documentation for every returned pointer,
- C and C++ examples built in CI,
- a C-only build of the C target that does not link the C++ runtime if
  feasible.

**Acceptance criteria:** C tests parse, inspect, modify, and emit documents;
the C target has no dependency on C++ exceptions; examples compile as C11.

---

## Milestone 5 — Robustness, security, and performance

### 5.1 Robustness

- Enforce configurable maximum input size, nesting depth, and token length.
- Validate UTF-8/16/32 strictly in the encoding layer; reject surrogates,
  overlong encodings, out-of-range code points, and unpaired surrogates.
- Replace assert-based helper error handling with return values or exceptions
  that survive `NDEBUG`.
- Make all parser and emitter operation locale-independent
  (`std::from_chars`/`std::to_chars`, no global-locale streams for numbers).
- Add fuzzing targets for parse, dump, and encoding helpers (libFuzzer or AFL).
- Run ASan, UBSan, and TSan in CI; run Valgrind on Linux.
- Add duplicate-key and table-redefinition tests to the invalid corpus.

### 5.2 Performance

Current measurements (see REVIEW.md) show parsing is roughly 10x slower than
emitting on large documents. Proposed work:

- Parse directly from a `std::string_view`/byte buffer; remove
  `std::istringstream` and `std::getline`.
- Avoid per-line and per-value string copies; use views into the input and
  allocate only for decoded strings.
- Reserve containers based on token counts or heuristics.
- Consider `std::unordered_map`/flat map for lookups, or keep `std::map` with
  an optional sorted/ordered mode.
- Use `std::from_chars` for numbers and `std::to_chars` for emission.
- Build a single-pass parser that does not rescan lines.
- Add benchmarks for: large flat documents, deep nesting, long strings, error
  paths, UTF-16/32 input, repeated parse/emit, and concurrent parsing.
- Store benchmark baselines and fail CI on significant regressions (with
  generous noise thresholds).
- Add memory-usage benchmarks, not only time.

Suggested relative targets after the rewrite:

- parse 2000-key document: at least 5x faster than the current ~1.43 ms,
- emit 2000-key document: at least 2x faster than the current ~148 µs,
- no allocation growth with nesting depth beyond the document size,
- constant or near-constant lookup behavior for typical table sizes.

**Acceptance criteria:** sanitizer/fuzz jobs are clean; performance targets are
tracked in CI; no unbounded recursion on adversarial input.

---

## Milestone 6 — Testing, CI, and release engineering

### 6.1 Test strategy

- **Compliance:** run [`toml-test`](https://github.com/toml-lang/toml-test) for
  valid and invalid cases in CI.
- **Unit tests:** keep the current Catch2 suites and extend them for every new
  syntax rule and every error path.
- **Property tests:** generate supported values, round-trip them, and compare;
  include strings with quotes/backslashes/newlines and extreme numbers.
- **Fuzzing:** continuous fuzz targets with a seed corpus and crash triage.
- **Compile tests:** verify C11/C17 and C++17/C++20 builds, header-only
  inclusion, shared/static, and the C API from a C compiler.
- **Negative tests:** malformed numbers, dates, escapes, duplicate keys,
  redefinitions, overlong input, invalid UTF, depth limits.
- **Coverage:** set thresholds for parser/emitter lines and branches; publish
  reports.
- **Benchmarks:** run on a stable machine, store results, and compare against a
  baseline.

### 6.2 CI matrix

- Linux, Windows, macOS.
- GCC, Clang, MSVC.
- C++17 and C++20 (and C++23 when supported).
- Static and shared libraries.
- CMake, Meson, and Bazel.
- ASan/UBSan, TSan, Valgrind.
- C API example compiled as C11.
- Documentation build and link check.

### 6.3 Packaging and documentation

- Generate a single version header; keep CMake, Meson, Bazel, and C macros in
  sync.
- Apply `mytoml_cpp_std` in Meson or remove the option.
- Remove or repair the stale top-level `Makefile`.
- Add pkg-config/CMake package tests that consume an installed package.
- Document the supported TOML version and every extension/limitation.
- Add a changelog and a compatibility/ABI policy.
- Provide one focused example per feature: parse, access, modify, emit,
  dates/times, arrays of tables, errors, streams, C API.
- Publish the compliance status and benchmark results.

**Acceptance criteria:** release artifacts build from a clean checkout on all
supported platforms; C and C++ consumers can build from installed packages;
compliance and sanitizer suites pass.

---

## Suggested issue breakdown

| Order | Issue | Depends on | Effort |
| --- | --- | --- | --- |
| 1 | Full-token numeric parsing and strict fallback | — | S |
| 2 | Emitter escaping, key quoting, float round-trip | — | S–M |
| 3 | Position-aware parse errors and `try_parse` | — | S |
| 4 | `operator>>` signature and checked accessors | — | S |
| 5 | New lexer with source spans | 3 | M |
| 6 | TOML 1.0 grammar and document builder | 1, 5 | L |
| 7 | Strings, multiline strings, escapes | 5, 6 | M |
| 8 | Numbers, dates/times, and value model | 1, 6 | M–L |
| 9 | Arrays of tables and table semantics | 6 | M |
| 10 | Correct emitter for all values | 2, 8, 9 | M |
| 11 | `toml-test` integration | 6–10 | S |
| 12 | Fuzzing and sanitizer CI | 6–10 | M |
| 13 | C API wrapper and C examples | 6, 10 | M |
| 14 | Performance rewrite and benchmarks | 6, 10 | M–L |
| 15 | Version/build cleanup and docs | — | S |

Effort: S = days, M = 1–2 weeks, L = several weeks for an experienced
contributor.

---

## Definition of done for a 1.0 release

- Passes the TOML 1.0 `toml-test` valid and invalid suites.
- Strict UTF-8/16/32 validation with BOM support.
- No silent fallback or prefix parsing.
- Emitter output always re-parses to an equal value.
- Duplicate keys and table redefinitions are rejected with source positions.
- C and C++ APIs are implemented, documented, and tested.
- ASan/UBSan/Valgrind and fuzz jobs are clean.
- Benchmarks are tracked in CI.
- CMake, Meson, Bazel, and installed-package consumers all build.
- README, API docs, examples, REVIEW.md, and this roadmap reflect the released
  behavior.

---

## Already completed in the current iteration

- Catch2 test suite added (55 cases / 584 assertions) covering the supported
  subset, errors, values, literals, streams, encoding helpers, and crash fixes.
- Catch2 micro-benchmarks added for parsing, emitting, construction, lookup,
  round-trip, and encoding.
- Benchmark build option and targets added to CMake; Meson/Bazel test sources
  kept in sync; a Bazel benchmark binary added.
- Crash fixes for empty keys/table names and trailing dotted-key segments.
- Literal operator linkage fixed; C target now compiles as C.
- `examples/apis/Basic.cpp` implemented and verified.
- README, MkDocs usage page, REVIEW.md, and this roadmap written.
