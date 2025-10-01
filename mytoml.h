/**
 * @file mytoml.h
 * @brief TOML parser library header for C/C++.
 * @details This header provides all public API, types, macros, and configuration for the mytoml TOML parser library. It is compliant with TOML v1.0.0 and supports both C and C++ usage.
 * @author Sackey Ezekiel Etrue
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
 *  [SECTION] Imports/Exports
 *  [SECTION] Data Structures
 *  [SECTION] C++ Only Classes
 *  [SECTION] C Only Functions
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
 * @brief Major version number of the library.
 */
#define MYTOML_VERSION_MAJOR 0

/**
 * @brief Minor version number of the library.
 */
#define MYTOML_VERSION_MINOR 1

/**
 * @brief Patch version number of the library.
 */
#define MYTOML_VERSION_PATCH 0

/**
 * @brief Library version string in the format "X.Y.Z".
 */
#define MYTOML_VERSION "0.1.0"

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Mytoml Header mess
//-----------------------------------------------------------------------------

#include <time.h>    //
#include <math.h>    //
#include <stdio.h>   // for printf
#include <stdarg.h>  //
#include <stdlib.h>  // for realloc
#include <string.h>  // for strdup strlen
#include <stdbool.h> //

#include "khash.h"

//-----------------------------------------------------------------------------
// [SECTION] Configurable macros
//-----------------------------------------------------------------------------

/**
 * @def MYTOML_MAX_DATE_FORMAT
 * @brief Maximum length for date format strings.
 * @note Default is 64.
 */
#define MYTOML_MAX_DATE_FORMAT 64

/**
 * @def MYTOML_MAX_ID_LENGTH
 * @brief Maximum length for TOML key identifiers.
 * @note Default is 256.
 */
#define MYTOML_MAX_ID_LENGTH 256

/**
 * @def MYTOML_MAX_STRING_LENGTH
 * @brief Maximum length for TOML string values.
 * @note Default is 4096.
 */
#define MYTOML_MAX_STRING_LENGTH 4096

/**
 * @def MYTOML_MAX_FILE_SIZE
 * @brief Maximum TOML file size in bytes.
 * @note Default is 1073741824 (1GB).
 */
#define MYTOML_MAX_FILE_SIZE 1073741824

/**
 * @def MYTOML_MAX_NUM_LINES
 * @brief Maximum number of lines in a TOML file.
 * @note Default is 16777216.
 */
#define MYTOML_MAX_NUM_LINES 16777216

/**
 * @def MYTOML_MAX_SUBKEYS
 * @brief Maximum number of subkeys per TOML key.
 * @note Default is 131072.
 */
#define MYTOML_MAX_SUBKEYS 131072

/**
 * @def MYTOML_MAX_ARRAY_LENGTH
 * @brief Maximum length of TOML arrays.
 * @note Default is 131072.
 */
#define MYTOML_MAX_ARRAY_LENGTH 131072

#ifdef __cplusplus

/** C++ Exclusive headers. */
#include <exception>
#include <iostream>

#endif //__cplusplus

#ifdef MYTOML_TESTS
#endif // MYTOML_TESTS

//-----------------------------------------------------------------------------
// [SECTION] Mytoml Import/Export
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
// [SECTION] Mytoml Data Structures
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
typedef enum TomlErrorType_t
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
    int idx;                       /**< Index for array tables. */
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
    TomlKeyType type;    /**< Type of error. */
    const char *message; /**< Error message string. */
};

/** @} */

/** @} */

#ifdef __cplusplus

//-----------------------------------------------------------------------------
// [SECTION] MyToml C++ Only
//-----------------------------------------------------------------------------

#endif //__cplusplus

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

    //-----------------------------------------------------------------------------
    // [SECTION] MyToml C Only
    //-----------------------------------------------------------------------------

    /**
     * @name Mytoml
     * C Functions
     * @{
     */

    /**
     * @brief Load and parse a TOML file or stdin.
     * @param[in] file Path to TOML file or NULL for stdin.
     * @return Pointer to root TomlKey object, or NULL on failure.
     * @note Frees memory with tomlFree().
     * @see tomlFree
     * @code
     * TomlKey *toml = tomlLoad("basic.toml");
     * if (toml == NULL) return 1;
     * tomlFree(toml);
     * @endcode
     */
    MYTOML_API TomlKey *tomlLoad(char *file);

    /**
     * @brief Load and parse a TOML file from a filename.
     * @param[in] file Path to TOML file.
     * @return Pointer to root TomlKey object, or NULL on failure.
     */
    MYTOML_API TomlKey *tomlLoadFile(char *file);

    /**
     * @brief Load and parse a TOML file from a FILE pointer.
     * @param[in] file FILE pointer to TOML file.
     * @return Pointer to root TomlKey object, or NULL on failure.
     */
    MYTOML_API TomlKey *tomlLoadFILE(FILE *file);

    /**
     * @brief Parse TOML from a string.
     * @param[in] toml TOML string to parse.
     * @return Pointer to root TomlKey object, or NULL on failure.
     * @note Frees memory with tomlFree().
     * @see tomlFree
     */
    // MYTOML_API TomlKey *tomlLoads(const char *toml);

    // TODO : Implement method and  Generic call.
    /**
     * @brief Load a toml string or filename.
     *
     * @code
     *  #include <mytoml.h>
     *
     *  int main(int argc, char *argv[])
     *  {
     *      TomlKey *toml = tomlLoad("basic.toml");
     *      if (toml == NULL)
     *          return 1;
     *
     *      tomlDump(toml,"simple.toml")
     *      tomlFree(toml);
     *      return 0;
     *  }
     * @endcode
     *
     * @param[in,out]  toml  A toml string or filename.
     * @returns the `TomlKey` object or @c NULL if failed to from `string` or file.
     */
    // tomlDump(key_or_value, file_or_FILE);

    /**
     * @brief Dump TOML key to a FILE stream.
     * @param[in] object TOML key to dump.
     * @param[in] file Output FILE stream.
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
     */
    MYTOML_API const char *tomlKeyDumps(TomlKey *k);

    /**
     * @brief Serialize TOML value to a string.
     * @param[in] v TOML value to serialize.
     * @return Pointer to string buffer (must be freed by caller).
     */
    MYTOML_API const char *tomlValueDumps(TomlValue *v);

    /**
     * @brief Dump TOML key to a buffer.
     * @param[in] k TOML key to dump.
     * @param[out] buffer Pointer to output buffer.
     * @param[out] size Size of output buffer.
     */
    MYTOML_API void tomlKeyDumpBuffer(TomlKey *k, char **buffer, size_t *size);

    /**
     * @brief Dump TOML value to a buffer.
     * @param[in] v TOML value to dump.
     * @param[out] buffer Pointer to output buffer.
     * @param[out] size Size of output buffer.
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

#endif // DJOEZEKE_MYTOML_H
