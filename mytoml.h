/**
 * @file mytoml.h
 * @brief TOML parser library header for C/C++.
 * @details This header provides all public API, types, macros, and configuration for the mytoml TOML parser library.
 *  It is compliant with [TOML v1.0.0](https://toml.io/en/v1.0.0) and supports both C and C++ usage.
 * @author Sackey Ezekiel Etrue (djoezeke)
 * @date Mon 29 09:06:15 Sept GMT 2025
 * @version 0.1.0
 * @see https://www.github.com/djoezeke/mytoml
 * @copyright Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Usage:
 * @code
 *  #include <mytoml.h>
 * @endcode
 *
 * Index of this file:
 *  [SECTION] Header mess
 *  [SECTION] Configurable macros
 *  [SECTION] Function Macros
 *  [SECTION] Imports/Exports
 *  [SECTION] Data Structures
 *  [SECTION] C Only Functions
 *  [SECTION] C++ Only Classes
 *
 *
 * Resources:
 * - Homepage ................... https://github.com/djoezeke/mytoml
 * - Releases & changelog ....... https://github.com/djoezeke/mytoml/releases
 * - Issues & support ........... https://github.com/djoezeke/mytoml/issues
 *
 */

#ifndef DJOEZEKE_MYTOML_H
#define DJOEZEKE_MYTOML_H

/**
 * @defgroup version Version Information
 * @brief Macros for library versioning.
 * @{
 */

/**
 * @def MYTOML_VERSION_MAJOR
 * @brief Major version number of the library.
 */
#define MYTOML_VERSION_MAJOR 0

/**
 * @def MYTOML_VERSION_MINOR
 * @brief Minor version number of the library.
 */
#define MYTOML_VERSION_MINOR 1

/**
 * @def MYTOML_VERSION_PATCH
 * @brief Patch version number of the library.
 */
#define MYTOML_VERSION_PATCH 0

/**
 * @def MYTOML_VERSION
 * @brief Library version string in the format @c "X.Y.Z",
 * where @c X is the major version number, @c Y is a minor version
 * number, and @c Z is the patch version number.
 */
#define MYTOML_VERSION "0.1.0"

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

#include <time.h>    //
#include <math.h>    //
#include <stdio.h>   // for printf
#include <stdarg.h>  //
#include <stdlib.h>  // for realloc
#include <string.h>  // for strdup strlen
#include <stdbool.h> //

#include "khash.h"

#ifdef MYTOML_TESTS
#endif // MYTOML_TESTS

//-----------------------------------------------------------------------------
// [SECTION] Configurable Macros
//-----------------------------------------------------------------------------

/**
 * @def MYTOML_MAX_DATE_FORMAT
 * @brief Maximum length for date format strings.
 * @note Default is 64 [`2^6`].
 */
#define MYTOML_MAX_DATE_FORMAT 64

/**
 * @def MYTOML_MAX_ID_LENGTH
 * @brief Maximum length for TOML key identifiers.
 * @note Default is 256 [`2^8`].
 */
#define MYTOML_MAX_ID_LENGTH 256

/**
 * @def MYTOML_MAX_STRING_LENGTH
 * @brief Maximum length for TOML string values.
 * @note Default is 4096 [`2^12`].
 */
#define MYTOML_MAX_STRING_LENGTH 4096

/**
 * @def MYTOML_MAX_FILE_SIZE
 * @brief Maximum TOML file size in bytes.
 * @note Default is 1073741824 [`2^30`](1GB).
 */
#define MYTOML_MAX_FILE_SIZE 1073741824

/**
 * @def MYTOML_MAX_NUM_LINES
 * @brief Maximum number of lines in a TOML file.
 * @note Default is 16777216 [`2^24`].
 */
#define MYTOML_MAX_NUM_LINES 16777216

/**
 * @def MYTOML_MAX_SUBKEYS
 * @brief Maximum number of subkeys per TOML key.
 * @note Default is 131072 [`2^17`].
 */
#define MYTOML_MAX_SUBKEYS 131072

/**
 * @def MYTOML_MAX_ARRAY_LENGTH
 * @brief Maximum length of TOML arrays.
 * @note Default is 131072 [`2^17`].
 */
#define MYTOML_MAX_ARRAY_LENGTH 131072

//-----------------------------------------------------------------------------
// [SECTION] Function Macros
//-----------------------------------------------------------------------------

/**
 * @brief Load and parse a TOML file or stdin.
 * @param[in] file Path to TOML file or NULL for stdin.
 * @return Pointer to root TomlKey object, or NULL on failure.
 * @note Frees memory with tomlFree().
 * @see tomlFree
 * Example usage:
 * @code
 * TomlKey *toml = tomlLoad("basic.toml");
 * if (toml == NULL) return 1;
 * tomlFree(toml);
 * @endcode
 * @note This macro uses C11 _Generic to select the appropriate function based on the type of `file`.
 * If `file` is a `char *`, it calls `tomlLoadFile()`. If `file` is a `FILE *`, it calls `tomlLoadFILE()`.
 * This allows for flexible loading of TOML data from either a file path or an open file stream using a single macro.
 * @warning The user must ensure that the types passed to the macro match the expected types to avoid compilation errors.
 * Improper usage may lead to undefined behavior.
 * @warning The file must exist and be readable. If using a `FILE *`, the caller is responsible for managing the lifetime of the FILE stream.
 * Failure to do so may result in resource leaks or crashes.
 *
 */
#define tomlLoad(file) _Generic((file), \
    char *: tomlLoadFile,               \
    const char *: tomlLoadFile,         \
    FILE *: tomlLoadFILE)(file)

/**
 * @brief Dump TOML value or key to a file or stream.
 * @param[in] object TOML key or value to dump.
 * @param[in] file Output filename or FILE stream.
 * @see tomlKeyDumpFile, tomlKeyDumpFILE, tomlValueDumpFile, tomlValueDumpFILE
 * Example usage:
 * @code
 * TomlKey *toml = tomlLoad("config.toml");
 * if (toml == NULL) {
 *   fprintf(stderr, "Failed to load TOML file\n");
 *   return 1;
 * }
 * // Dump to stdout
 * tomlValueDump(toml, stdout);
 * // Dump to file
 * tomlValueDump(toml, "output.toml");
 * tomlFree(toml);
 * @endcode
 * @note This macro uses C11 _Generic to select the appropriate function based on the types of `object` and `file`.
 * For `TomlKey *` objects, it calls either `tomlKeyDumpFile()` or `tomlKeyDumpFILE()` depending on whether `file` is a `char *` filename or a `FILE *` stream.
 * For `TomlValue *` objects, it calls either `tomlValueDumpFile()` or `tomlValueDumpFILE()` similarly.
 * This allows for flexible dumping of both keys and values to either files or streams using a single macro.
 * @warning The user must ensure that the types passed to the macro match the expected types to avoid compilation errors.
 * Improper usage may lead to undefined behavior.
 * @warning The file or stream must be valid and writable. The caller is responsible for managing the lifetime of the FILE stream if used.
 * Failure to do so may result in resource leaks or crashes.
 */
#define tomlValueDump(object, file) _Generic((object), \
    TomlKey *: _Generic((file),                        \
            char *: tomlKeyDumpFile,                   \
            const char *: tomlKeyDumpFile,             \
            FILE *: tomlKeyDumpFILE),                  \
    TomlValue *: _Generic((file),                      \
            char *: tomlValueDumpFile,                 \
            const char *: tomlValueDumpFile,           \
            FILE *: tomlValueDumpFILE, ))(object, file)

/**
 * @brief Serialize TOML value or key to a string.
 * @param[in] object TOML key or value to serialize.
 * @return Pointer to string buffer (must be freed by caller).
 * @see tomlKeyDumps, tomlValueDumps
 * Example usage:
 * @code
 * TomlKey *toml = tomlLoad("basic.toml");
 * if (toml == NULL) return 1;
 * const char *json = tomlDumps(toml);
 * printf("%s\n", json);
 * free((void *)json);
 * tomlFree(toml);
 * @endcode
 * @note This macro uses C11 _Generic to select the appropriate function based on the type of `object`.
 * For `TomlKey *`, it calls `tomlKeyDumps()`, and for `TomlValue *`, it calls `tomlValueDumps()`.
 * @warning The returned string must be freed by the caller to avoid memory leaks.
 *
 */
#define tomlDumps(object) _Generic((object), \
    TomlKey *: tomlKeyDumps,                 \
    TomlValue *: tomlValueDumps)(object)

/**
 * @brief Dump TOML value or key to a buffer.
 * @param[in] object TOML key or value to dump.
 * @param[out] buffer Pointer to output buffer.
 * @param[out] size Size of output buffer.
 * @see tomlKeyDumpBuffer, tomlValueDumpBuffer
 * Example usage:
 * @code
 * TomlKey *toml = tomlLoad("basic.toml");
 * if (toml == NULL) return 1;
 * char *buffer = "";
 * size_t size = 0;
 * tomlDumpBuffer(toml, &buffer, &size);
 * printf("%s\n", buffer);
 * free(buffer);
 * tomlFree(toml);
 * @endcode
 * @note This macro uses C11 _Generic to select the appropriate function based on the type of `object`.
 * For `TomlKey *`, it calls `tomlKeyDumpBuffer()`, and for `TomlValue *`, it calls `tomlValueDumpBuffer()`.
 * @warning The user must ensure that the types passed to the macro match the expected types to avoid compilation errors.
 * Improper usage may lead to undefined behavior.
 * @warning The buffer must be managed by the caller. The caller is responsible for freeing the
 * buffer to avoid memory leaks.
 *
 */
#define tomlDumpBuffer(object, buffer, size) _Generic((object), \
    TomlKey *: tomlKeyDumpBuffer,                               \
    TomlValue *: tomlValueDumpBuffer)(object, buffer, size)

#ifdef __cplusplus

/** C++ Exclusive headers. */
#include <exception>
#include <iostream>

#endif //__cplusplus

//-----------------------------------------------------------------------------
// [SECTION] Import/Export
//-----------------------------------------------------------------------------

/**
 * @defgroup export Export Definitions
 * @{
 */

#if defined(_WIN32)
#define MYTOML_NO_EXPORT_ATTR
#define MYTOML_API_EXPORT __declspec(dllexport)
#define MYTOML_API_IMPORT __declspec(dllimport)
#define MYTOML_DEPRECATED_ATTR __declspec(deprecated)
#else // _WIN32
#define MYTOML_API_EXPORT __attribute__((visibility("default")))
#define MYTOML_API_IMPORT __attribute__((visibility("default")))
#define MYTOML_NO_EXPORT_ATTR __attribute__((visibility("hidden")))
#define MYTOML_DEPRECATED_ATTR __attribute__((__deprecated__))
#endif // _WIN32

/**
 * @def MYTOML_API
 * @brief Macro for public API symbol export/import.
 * @details Use this macro to annotate all public API functions for correct symbol visibility on all platforms.
 */

#if defined(MYTOML_BUILD_STATIC)
#define MYTOML_API
#elif defined(MYTOML_BUILD_SHARED)
/* We are building this library */
#define MYTOML_API MYTOML_API_EXPORT
#elif defined(MYTOML_IMPORT)
/* We are using this library */
#define MYTOML_API MYTOML_API_IMPORT
#else // MYTOML_BUILD_STATIC
#define MYTOML_API
#endif // MYTOML_BUILD_STATIC

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Data Structures
//-----------------------------------------------------------------------------

/**
 * @defgroup basic Basic Types
 * @brief Core types and data structures for TOML parsing.
 * @{
 */

/**
 * @enum TomlValueType
 * @brief Enumerates all TOML value types supported by the parser.
 * @details Used to distinguish between integer, boolean, float, array, string, datetime, and table types as defined in the TOML specification.
 */
typedef enum TomlValueType_t
{
    TOML_INT,          /**< Integer value type. */
    TOML_BOOL,         /**< Boolean value type. */
    TOML_FLOAT,        /**< Floating-point value type. */
    TOML_ARRAY,        /**< Array value type. */
    TOML_STRING,       /**< String value type. */
    TOML_DATETIME,     /**< Datetime value type (RFC 3339). */
    TOML_DATELOCAL,    /**< Local date value type. */
    TOML_TIMELOCAL,    /**< Local time value type. */
    TOML_INLINETABLE,  /**< Inline table value type. */
    TOML_DATETIMELOCAL /**< Local datetime value type. */
} TomlValueType;

/**
 * @enum TomlKeyType
 * @brief Enumerates all TOML key types recognized by the parser.
 * @details Used to distinguish between root keys, tables, leaf keys, and array tables for parsing and validation.
 */
typedef enum TomlKeyType_t
{
    TOML_KEY,       /**< Key in a key-value pair (e.g., j in j.k = v). */
    TOML_TABLE,     /**< Table key (e.g., a in [a.b]). */
    TOML_KEYLEAF,   /**< Leaf key in a key-value pair (e.g., k in j.k = v). */
    TOML_TABLELEAF, /**< Leaf table key (e.g., b in [a.b]). */
    TOML_ARRAYTABLE /**< Array table key (e.g., t in [[t]]). */
} TomlKeyType;

/**
 * @enum TomlErrorType
 * @brief Enumerates error types for TOML parsing.
 * @details Used to classify errors encountered during parsing or validation.
 */
typedef enum TomlErrorType
{
    TOML_UNKNOWN /**< Unknown error type. */
} TomlErrorType;

/**
 * @name TomlValue data type
 * @{
 */

/**
 * @struct TomlValue
 * @brief Represents a TOML value and its associated metadata.
 * @details Used to store any TOML value type, including arrays, numbers, strings, and datetimes.
 */
typedef struct TomlValue_t TomlValue;
struct TomlValue_t
{
    TomlValueType type;                  /**< Type of TOML value. */
    TomlValue **arr;                     /**< Array of TOML values (for TOML_ARRAY type). */
    int len;                             /**< Length of array or value. */
    void *data;                          /**< Pointer to value data (non-array types). */
    int precision;                       /**< Numeric precision for floating-point values. */
    bool scientific;                     /**< Whether to print numbers in scientific notation. */
    char format[MYTOML_MAX_DATE_FORMAT]; /**< Format string for datetime values. */
};

/** @} */

/**
 * @name TomlKey data type
 * @{
 */

/**
 * @struct TomlKey
 * @brief Represents a TOML key node in the parsed AST.
 * @details Each TOML key or table is represented as a TomlKey, with subkeys and associated value.
 */
typedef struct TomlKey_t TomlKey;
KHASH_MAP_INIT_STR(str, TomlKey *)
struct TomlKey_t
{
    TomlKeyType type;              /**< Type of TOML key. */
    char id[MYTOML_MAX_ID_LENGTH]; /**< Key identifier string. */
    khash_t(str) * subkeys;        /**< Hash map of subkeys. */
    TomlValue *value;              /**< Value associated with this key. */
    size_t idx;                    /**< Index for array tables. */
};

/** @} */

/**
 * @name TomlError data type
 * @{
 */

/**
 * @struct TomlError
 * @brief Represents an error encountered during TOML parsing.
 * @details Contains error type and message for diagnostics.
 */
typedef struct TomlError_t TomlError;
struct TomlError_t
{
    TomlErrorType type;  /**< Type of error. */
    const char *message; /**< Error message string. */
};

/** @} */

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] C Only Functions
//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

    /**
     * @name Mytoml
     * C Functions
     * @{
     */

    /**
     * @brief Load and parse a TOML file from a filename.
     * @param[in] file Path to TOML file.
     * @return Pointer to root TomlKey object, or NULL on failure.
     * @note Frees memory with tomlFree().
     * @see tomlFree
     */
    MYTOML_API TomlKey *tomlLoadFile(char *file);

    /**
     * @brief Load and parse a TOML file from a FILE pointer.
     * @param[in] file FILE pointer to TOML file.
     * @return Pointer to root TomlKey object, or NULL on failure.
     * @note Frees memory with tomlFree().
     * @see tomlFree
     */
    MYTOML_API TomlKey *tomlLoadFILE(FILE *file);

    /**
     * @brief Parse TOML from a string.
     * @param[in] toml TOML string to parse.
     * @return Pointer to root TomlKey object, or NULL on failure.
     * @note Frees memory with tomlFree().
     * @see tomlFree
     */
    MYTOML_API TomlKey *tomlLoads(const char *toml);

    /**
     * @brief Dump TOML key to a FILE stream.
     * @param[in] object TOML key to dump.
     * @param[in] file Output FILE stream.
     * @warning The file must be valid and writable.
     * The caller is responsible for managing the lifetime of the FILE stream if used.
     * Failure to do so may result in resource leaks or crashes.
     */
    MYTOML_API void tomlKeyDumpFILE(TomlKey *object, FILE *file);

    /**
     * @brief Dump TOML key to a file by filename.
     * @param[in] object TOML key to dump.
     * @param[in] file Output filename.
     */
    MYTOML_API void tomlKeyDumpFile(TomlKey *object, const char *file);

    /**
     * @brief Dump TOML value to a FILE stream.
     * @param[in] object TOML value to dump.
     * @param[in] file Output FILE stream.
     * @warning The file must be valid and writable.
     * The caller is responsible for managing the lifetime of the FILE stream if used.
     * Failure to do so may result in resource leaks or crashes.
     */
    MYTOML_API void tomlValueDumpFILE(TomlValue *object, FILE *file);

    /**
     * @brief Dump TOML value to a file by filename.
     * @param[in] object TOML value to dump.
     * @param[in] file Output filename.
     */
    MYTOML_API void tomlValueDumpFile(TomlValue *object, const char *file);

    /**
     * @brief Serialize TOML key to a string.
     * @param[in] k TOML key to serialize.
     * @return Pointer to string buffer (must be freed by caller).
     * @warning The returned string must be freed by the caller to avoid memory leaks.
     */
    MYTOML_API const char *tomlKeyDumps(TomlKey *k);

    /**
     * @brief Serialize TOML value to a string.
     * @param[in] v TOML value to serialize.
     * @return Pointer to string buffer (must be freed by caller).
     * @warning The returned string must be freed by the caller to avoid memory leaks.
     */
    MYTOML_API const char *tomlValueDumps(TomlValue *v);

    /**
     * @brief Dump TOML key to a buffer.
     * @param[in] k TOML key to dump.
     * @param[out] buffer Pointer to output buffer.
     * @param[out] size Size of output buffer.
     * @warning The buffer must be managed by the caller. The caller is responsible for freeing the
     * buffer to avoid memory leaks.
     */
    MYTOML_API void tomlKeyDumpBuffer(TomlKey *k, char **buffer, size_t *size);

    /**
     * @brief Dump TOML value to a buffer.
     * @param[in] v TOML value to dump.
     * @param[out] buffer Pointer to output buffer.
     * @param[out] size Size of output buffer.
     * @warning The buffer must be managed by the caller. The caller is responsible for freeing the
     * buffer to avoid memory leaks.     *
     */
    MYTOML_API void tomlValueDumpBuffer(TomlValue *v, char **buffer, size_t *size);

    /**
     * @brief Dump TOML key as JSON to stdout.
     * @param[in] root Root TOML key to dump as JSON.
     */
    MYTOML_API void toml_json_dump(TomlKey *root);

    /**
     * @brief Free memory allocated for a TomlKey object and all its children.
     * @param[in] toml Pointer to TomlKey object to free.
     */
    MYTOML_API void tomlFree(TomlKey *toml);

    /**
     * @brief Get integer value from TOML key.
     * @param[in] key TOML key to query.
     * @return Pointer to integer value, or NULL if not an integer.
     */
    MYTOML_API int *tomlGetInt(TomlKey *key);

    /**
     * @brief Get boolean value from TOML key.
     * @param[in] key TOML key to query.
     * @return Pointer to boolean value, or NULL if not a boolean.
     */
    MYTOML_API bool *tomlGetBool(TomlKey *key);

    /**
     * @brief Get string value from TOML key.
     * @param[in] key TOML key to query.
     * @return Pointer to string value, or NULL if not a string.
     */
    MYTOML_API char *tomlGetString(TomlKey *key);

    /**
     * @brief Get floating-point value from TOML key.
     * @param[in] key TOML key to query.
     * @return Pointer to double value, or NULL if not a float.
     */
    MYTOML_API double *tomlGetFloat(TomlKey *key);

    /**
     * @brief Get array value from TOML key.
     * @param[in] key TOML key to query.
     * @return Pointer to TomlValue array, or NULL if not an array.
     */
    MYTOML_API TomlValue *tomlGetArray(TomlKey *key);

    /**
     * @brief Get datetime value from TOML key.
     * @param[in] key TOML key to query.
     * @return Pointer to struct tm value, or NULL if not a datetime.
     */
    MYTOML_API struct tm *tomlGetDatetime(TomlKey *key);

    /**
     * @brief Find a subkey by identifier.
     * @param[in] key TOML key to search.
     * @param[in] id Identifier string to match.
     * @return Pointer to matching TomlKey, or NULL if not found.
     */
    MYTOML_API TomlKey *tomlGetKey(TomlKey *key, const char *id);

    /** @} */

#ifdef __cplusplus
}
#endif //__cplusplus

//-----------------------------------------------------------------------------
// [SECTION] C++ Only Classes
//-----------------------------------------------------------------------------

#ifdef __cplusplus

#endif //__cplusplus

#endif // DJOEZEKE_MYTOML_H
