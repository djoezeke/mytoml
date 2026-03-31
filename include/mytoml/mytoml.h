/**
 * @file mytoml.h
 * @brief Parser/Emitter .
 * @details This header provides all public API, types, macros, and
 * configuration for the library.
 * @author Sackey Ezekiel Etrue (djoezeke)
 * @version 0.1.0
 * @see https://www.github.com/djoezeke/mytoml
 * @copyright Copyright (c) 2025 Sackey Ezekiel Etrue
 *
 * Developed by Sackey Ezekiel Etrue and every direct or indirect contributors to the GitHub.
 * See LICENSE for copyright and licensing details (standard MIT License).
 *
 * MYTOML: What this Is
 *
 * DOCUMENTATION:
 *
 * FEATURES:
 *
 * MISSION:
 *
 * ISSUES:
 *
 * NOTES:
 *
 * USAGE:
 *
 * FAQS:
 *
 * TODO:
 *
 * HELP:
 *    - See links below.
 *    - Read top of mytomlc.cpp for more details and comments.
 *
 *  Has only had a few tests run, may have issues.
 *
 *  If having issues compiling/linking/running raise an issue (https://github.com/djoezeke/mytoml/issues).
 *  Please post in https://github.com/djoezeke/mytoml/discussions if you cannot find a solution in resources above.
 *
 * RESOURCES:
 * - Homepage ................... https://github.com/djoezeke/mytoml
 * - Releases & changelog ....... https://github.com/djoezeke/mytoml/releases
 * - Issues & support ........... https://github.com/djoezeke/mytoml/issues
 *
 */

#ifndef DJOEZEKE_MYTOMLC_H

/**
 * [SECTIONS] Index of this file
 *
 *  SECTION: Include Mess
 *  SECTION: Configurations
 *  SECTION: Function Macros
 *  SECTION: API Imports/Exports
 *  SECTION: Forward Declarations
 *  SECTION: Flags & Enumerations
 *  SECTION: Data Structures
 *  SECTION: Functions Declarations
 */

#define DJOEZEKE_MYTOMLC_H

// NOLINTBEGIN

// clang-format off

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef MYTOMLC_SKIP_VERSION_CHECK
    #if defined(MYTOMLC_VERSION_MAJOR) && defined(MYTOMLC_VERSION_MINOR) && defined(MYTOMLC_VERSION_PATCH)
        #if MYTOMLC_VERSION_MAJOR != 0 || MYTOMLC_VERSION_MINOR != 1 || MYTOMLC_VERSION_PATCH != 0
            #warning "Already included a different version of the library!"
        #endif
    #endif
#endif  // MYTOMLC_SKIP_VERSION_CHECK

/**
 * @defgroup version Version Information
 * @brief Macros for library versioning.
 * @{
 */

/**
 * @def MYTOMLC_VERSION_MAJOR
 * @brief Major version number of the library.
 * @note If this were version 1.2.3, this value would be 1.
 * @since This macro is available since 0.1.0 .
 */
#define MYTOMLC_VERSION_MAJOR 0

/**
 * @def MYTOMLC_VERSION_MINOR
 * @brief Minor version number of the library.
 * @note If this were version 1.2.3, this value would be 2.
 * @since This macro is available since 0.1.0 .
 */
#define MYTOMLC_VERSION_MINOR 1

/**
 * @def MYTOMLC_VERSION_PATCH
 * @brief Patch version number of the library.
 * @note If this were version 1.2.3, this value would be 3.
 * @since This macro is available since 0.1.0 .
 */
#define MYTOMLC_VERSION_PATCH 0

/**
 * @def MYTOMLC_VERSION
 * @brief Library version string in the format @c "X.Y.Z",
 * where @c X is the major version number, @c Y is a minor version
 * number, and @c Z is the patch version number.
 */
#define MYTOMLC_VERSION "0.1.0"

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Include Mess
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// [SECTION] Configurations
//-----------------------------------------------------------------------------

/**
 * @defgroup configure Library Configurations
 * @{
 */

/**
 * @def MYTOMLC_DISABLE_READER
 * @brief Exclude Reading/Deserialization methods.
 *
 * Define to 1 to remove reader/parsing code at compile time when parsing
 * is not required. This reduces the compiled binary size.
 *
 * Example:
 * @code
 * #define MYTOMLC_DISABLE_READER 1
 * #include <mytoml/mytoml.h>
 * @endcode
 *
 * @warning This will disable all parsing functions.
 * 
 * @note This will reduce the binary size by about 60%.
 */
#ifndef MYTOMLC_DISABLE_READER
#endif

/**
 * @def MYTOMLC_DISABLE_WRITER
 * @brief Exclude Writing/Serialization methods.
 *
 * Define to 1 to remove writer/emitting code at compile time when emitting
 * is not required. This reduces the compiled binary size.
 *
 * Example:
 * @code
 * #define MYTOMLC_DISABLE_WRITER 1
 * #include <mytoml/mytoml.h>
 * @endcode
 *  
 * @warning This will disable writting & serialization.
 *
 * @note This will reduce the binary size by about 30%.
 */
#ifndef MYTOMLC_DISABLE_WRITER
#endif

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Function Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] API Import/Export
//-----------------------------------------------------------------------------

/**
 * @defgroup export Export Definitions
 * @{
 */

#if defined(_WIN32)
    #define MYTOMLC_API_EXPORT __declspec(dllexport)
    #define MYTOMLC_API_IMPORT __declspec(dllimport)
    #define MYTOMLC_NO_EXPORT
#else  // _WIN32
    #define MYTOMLC_API_EXPORT __attribute__((visibility("default")))
    #define MYTOMLC_API_IMPORT __attribute__((visibility("default")))
    #define MYTOMLC_NO_EXPORT __attribute__((visibility("hidden")))
#endif  // _WIN32

/**
 * @def MYTOMLC_API
 * @brief Macro for public API symbol export/import.
 * @details Use this macro to annotate all public API functions for correct
 * symbol visibility on all platforms.
 */

#if defined(MYTOMLC_BUILD_STATIC)
    #define MYTOMLC_API
#elif defined(MYTOMLC_BUILD_SHARED) || defined(MYTOMLC_EXPORTS)
    /* We are building this library */
    #define MYTOMLC_API MYTOMLC_API_EXPORT
#elif defined(MYTOMLC_LOAD_SHARED) || defined(MYTOMLC_IMPORTS)
    /* We are using this library */
    #define MYTOMLC_API MYTOMLC_API_IMPORT
#else  // MYTOMLC_BUILD_STATIC
    #define MYTOMLC_API
#endif  // MYTOMLC_BUILD_STATIC

/** @} */

// clang-format on

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

    //-----------------------------------------------------------------------------
    // [SECTION] Forward Declarations
    //-----------------------------------------------------------------------------

    /** Enumerations */

    /** Structures */

    struct mtVersion;

    //-----------------------------------------------------------------------------
    // [SECTION] Flags & Enumerations
    //-----------------------------------------------------------------------------

    /**
     * @defgroup enum Flags & Enumerations
     * @brief Core enum types and flags.
     * @{
     */

    typedef enum mtEncoding
    {
        mtUnspecifiedEncoding, /** Let the parser choose the encoding. */
        mtUTF8Encoding,        /** The default UTF-8 encoding. */
        mtUTF16Encoding,       /** The UTF-16-LE encoding with native endianness. */
        mtUTF16LEEncoding,     /** The UTF-16-LE encoding with BOM. */
        mtUTF16BEEncoding,     /** The UTF-16-BE encoding with BOM. */
        mtUTF32Encoding,       /** The UTF-32 encoding with native endianness. */
        mtUTF32LEEncoding,     /** The UTF-32-LE encoding with BOM. */
        mtUTF32BEEncoding,     /** The UTF-32-BE encoding with BOM. */
    } mtEncoding;

    /** @} */

    //-----------------------------------------------------------------------------
    // [SECTION] Data Structures
    //-----------------------------------------------------------------------------

    /**
     * @defgroup struct Data Structures
     * @brief Core types and data structures.
     * @{
     */

    typedef struct mtVersion
    {
        unsigned int major;
        unsigned int minor;
        unsigned int patch;
    } mtVersion;

    /** @} */

    //-----------------------------------------------------------------------------
    // [SECTION] Function Declarations
    //-----------------------------------------------------------------------------

#ifdef __cplusplus
};
#endif //__cplusplus

//-----------------------------------------------------------------------------

// clang-format off

#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

// clang-format on

#endif // DJOEZEKE_MYTOMLC_H

// NOLINTEND

/**
 * HISTORY: Version History
 */

/**
 * LICENSE: MIT License
 *
 * Copyright (c) 2025 Sackey Ezekiel Etrue
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
 */