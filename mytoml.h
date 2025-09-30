/** -*- C++ -*- compatibility header. */

/**
 * @file mytoml.h
 *
 * @brief
 *  TOML parser written in C. Written to be compliant with [TOML v1.0.0](https://toml.io/en/v1.0.0).
 *  This is a  C/C++ Toml Parser Library Header  @c mytoml.h.
 *
 * @note
 *  You can @c \#include this file in your programs, for all compilers C or C++.
 *
 * @author      Sackey Ezekiel Etrue.
 * @date        Mon 29 09:06:15 Sept GMT 2025
 * @version     0.1.0
 * @see         <https://www.github.com/djoezeke/mytoml>.
 * @copyright   Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
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

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/**
 * @defgroup version Version Information
 * @{
 */

/**
 * @returns Major version number.
 */
#define MYTOML_VERSION_MAJOR 0

/**
 * @returns Minor version number.
 */
#define MYTOML_VERSION_MINOR 1

/**
 * @returns Patch version number.
 */
#define MYTOML_VERSION_PATCH 0

/**
 * Get the library version as a string.
 *
 * @returns The function returns the string of the form
 * @c "X.Y.Z", where @c X is the major version number, @c Y is a minor version
 * number, and @c Z is the patch version number.
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

#define TOML_MAX_DATE_FORMAT 64
#define TOML_MAX_ID_LENGTH 256
#define TOML_MAX_STRING_LENGTH 4096

#define TOML_MAX_SUBKEYS 131072      // 2^17
#define TOML_MAX_ARRAY_LENGTH 131072 // 2^17

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
#define MYTOML_API_EXPORT __declspec(dllexport)
#define MYTOML_API_IMPORT __declspec(dllimport)
#else // _WIN32
#define MYTOML_API_EXPORT __attribute__((visibility("default")))
#define MYTOML_API_IMPORT __attribute__((visibility("default")))
#endif // _WIN32

    /** The public API declaration. */

#if defined(MYTOML_BUILD_STATIC)
#define MYTOML_API
#elif defined(MYTOML_BUILD_SHARED)
#define MYTOML_API MYTOML_API_EXPORT
#elif defined(MYTOML_IMPORT)
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
     * @{
     */

    /*
        Enum `TomlValueType` represents the set of value
        types accepted by this TOML parser. This corresponds
        to the various types defined in the TOML Spec.
    */
    typedef enum TomlValueType_t
    {

        /**
         * @name Toml value types
         * @{
         */

        TOML_INT,           /** A number value type (integer). */
        TOML_BOOL,          /** A boolean value type. */
        TOML_FLOAT,         /** A number value type (floating-point).*/
        TOML_ARRAY,         /** A array value type. */
        TOML_STRING,        /** A string value type. */
        TOML_DATETIME,      /** A datetime value type. */
        TOML_DATELOCAL,     /** A datelocal value type. */
        TOML_TIMELOCAL,     /** A timelocal value type. */
        TOML_INLINETABLE,   /** A table value type. */
        TOML_DATETIMELOCAL, /** A datetime local value type. */

        /** @} */

    } TomlValueType;

    /*
        Enum `TomlKeyType` represents the various types of
        keys that this TOML parser is aware of. It is used
        to make re-defining validity decisions.
    */
    typedef enum TomlKeyType_t
    {
        /**
         * @name Toml key types
         * @{
         */

        TOML_KEY,        /** `j.k = v` -> j */
        TOML_TABLE,      /** `[a.b]` -> a */
        TOML_KEYLEAF,    /** `j.k = v` -> k */
        TOML_TABLELEAF,  /** `[a.b]` -> b */
        TOML_ARRAYTABLE, /** `[[t]]` -> t */

        /** @} */

    } TomlKeyType;

    typedef enum TomlErrorType_t
    {

        /**
         * @name Toml error types
         * @{
         */

        TOML_UNKNOWN, /** An unknown error type. */

        /** @} */

    } TomlErrorType;

    /**
     * @name TomlValue data type
     * @{
     */

    /*
        Struct `TomlValue` holds the various attributes
        associated with a TOML value.
    */
    typedef struct TomlValue_t TomlValue;
    struct TomlValue_t
    {
        TomlValueType type;                /** the type of TOML value */
        TomlValue **arr;                   /** used for storing `ARRAY` type values */
        int len;                           /**  */
        void *data;                        /** used for storing non-`ARRAY` type values */
        int precision;                     /** used for printing numeric values */
        bool scientific;                   /** used for printing numeric values */
        char format[TOML_MAX_DATE_FORMAT]; /** used for printing datetime values */
    };

    /** @} */

    /**
     * @name TomlKey data type
     * @{
     */

    /*
        Struct `TomlKey` defines the TOML keys. Each node in
        the parsed AST is a `key`, irrespective of the fact
        if they were defined as TOML keys or tables.
    */
    typedef struct TomlKey_t TomlKey;
    KHASH_MAP_INIT_STR(str, TomlKey *)
    struct TomlKey_t
    {
        TomlKeyType type;            /** key type as described above */
        char id[TOML_MAX_ID_LENGTH]; /** identifier */
        khash_t(str) * subkeys;      /** map of subkeys */
        TomlValue *value;            /** value associated with this key */
        int idx;                     /** used for indexing ARRAYTABLES */
    };

    /** @} */

    /**
     * @name TomlError data type
     * @{
     */

    typedef struct TomlError_t TomlError;
    struct TomlError_t
    {
        TomlKeyType type;    /** type of error*/
        const char *message; /** */
    };

    /** @} */

    /** @} */

#ifdef __cplusplus

    //-----------------------------------------------------------------------------
    // [SECTION] MyToml C++ Only
    //-----------------------------------------------------------------------------

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
     * @brief Load a toml file or stdin.
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
     *      tomlFree(toml);
     *      return 0;
     *  }
     * @endcode
     *
     * @param[in,out]   file  A file pointer.
     * @returns the `TomlKey` object or @c NULL if failed to from load file or `stdin`.
     */
    MYTOML_API TomlKey *tomlLoad(char *file);
    MYTOML_API TomlKey *tomlLoadFile(char *file);
    MYTOML_API TomlKey *tomlLoadFILE(FILE *file);

    /**
     * @brief Load a toml string or filename.
     *
     * @code
     *  #include <mytoml.h>
     *
     *  int main(int argc, char *argv[])
     *  {
     *      char* object = ""
     *      TomlKey *toml = tomlLoads(object);
     *      if (toml == NULL)
     *          return 1;
     *
     *      tomlFree(toml);
     *      return 0;
     *  }
     * @endcode
     *
     * @param[in,out]  toml  A toml string or filename.
     * @returns the `TomlKey` object or @c NULL if failed to from `string` or file.
     */
    MYTOML_API TomlKey *tomlLoads(const char *toml);

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
    MYTOML_API void tomlKeyDumpFILE(TomlKey *object, FILE *file);
    MYTOML_API void tomlKeyDumpFile(TomlKey *object, const char *file);
    MYTOML_API void tomlValueDumpFILE(TomlValue *object, FILE *file);
    MYTOML_API void tomlValueDumpFile(TomlValue *object, const char *file);

    MYTOML_API const char *tomlKeyDumps(TomlKey *k);
    MYTOML_API const char *tomlValueDumps(TomlValue *v);

    MYTOML_API void tomlKeyDumpBuffer(TomlKey *k, char **buffer, size_t *size);
    MYTOML_API void tomlValueDumpBuffer(TomlValue *v, char **buffer, size_t *size);

    MYTOML_API void toml_json_dump(TomlKey *root);

    /**
     * @brief Free any memory allocated for a `TomlKey` object.
     *
     * @param[in,out] toml A TomlKey object.
     */
    MYTOML_API void tomlFree(TomlKey *toml);

    MYTOML_API int *tomlGetInt(TomlKey *key);
    MYTOML_API bool *tomlGetBool(TomlKey *key);

    /**
     * @brief Gets the name of the error type.
     * @note If the existing `key->id` is not a match, it iterates through the list of `children`.
     * @param[in]  key The key to get it string value.
     * @return The a pointer to the first match.
     * @retval string String value of `TomlKey`.
     * @retval otherwise it returns NULL.
     */
    MYTOML_API char *tomlGetString(TomlKey *key);
    MYTOML_API double *tomlGetFloat(TomlKey *key);
    MYTOML_API TomlValue *tomlGetArray(TomlKey *key);
    MYTOML_API struct tm *tomlGetDatetime(TomlKey *key);

    /**
     * @brief Tries to return a key based on the argument `id`.
     * @note If the existing `key->id` is not a match, it iterates through the list of `children`.
     * @param[in] key The key to get it string value.
     * @param[in] key The key to get it string value.
     * @retval pointer first match of `id`.
     * @retval otherwise it returns NULL.
     */
    MYTOML_API TomlKey *tomlGetKey(TomlKey *key, const char *id);

    /** @} */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // DJOEZEKE_MYTOML_H
