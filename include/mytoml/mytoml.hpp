/**
 * @file mytoml.hpp
 * @brief TOML For Modern C/C++.
 * @details All public API, types, macros, and configuration.
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
 * HELP:
 *    - See links below.
 *    - Read top of mytoml.cpp for more details and comments.
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

#ifndef DJOEZEKE_MYTOML_HPP

/**
 * [SECTIONS] Index of this file
 *
 *  SECTION: Include Mess
 *  SECTION: Configurations
 *  SECTION: Function Macros
 *  SECTION: Platform Defines
 *  SECTION: Standard Defines
 *  SECTION: Compiler Defines
 *  SECTION: Compiler Warnings
 *  SECTION: Compiler Attributes
 *  SECTION: API Imports/Exports
 *
 *  Forward: Forward Declarations
 *
 *  SECTION: Details Declarations
 *  SECTION: Mytoml Declarations
 *  SECTION: Literals Declarations
 *
 *  Details: Details API Namespace
 *
 *  SECTION: Flags & Enumerations
 *  SECTION: Data Structures
 *  SECTION: Function Declarations
 *
 *  Mytoml: Mytoml API Namespace
 *
 *  SECTION: Flags & Enumerations
 *  SECTION: Data Structures
 *  SECTION: Function Declarations
 *
 *  Literals: Literals API Namespace
 *
 *  SECTION: Function Declarations
 *
 */

#define DJOEZEKE_MYTOML_HPP

// NOLINTBEGIN

// clang-format off

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef MYTOML_SKIP_VERSION_CHECK
    #if defined(MYTOML_VERSION_MAJOR) && defined(MYTOML_VERSION_MINOR) && defined(MYTOML_VERSION_PATCH)
        #if MYTOML_VERSION_MAJOR != 1 || MYTOML_VERSION_MINOR != 0 || MYTOML_VERSION_PATCH != 0
            #warning "Already included a different version of the library!"
        #endif
    #endif
#endif  // MYTOML_SKIP_VERSION_CHECK

/**
 * @defgroup version version Information
 * @brief Macros for library versioning.
 * @{
 */

/**
 * @def MYTOML_VERSION_MAJOR
 * @brief Major version number of the library.
 * @note If this were version 1.2.3, this value would be 1.
 * @since This macro is available since 0.1.0 .
 */
#ifndef MYTOML_VERSION_MAJOR
    #define MYTOML_VERSION_MAJOR 0
#endif // MYTOML_VERSION_MAJOR

/**
 * @def MYTOML_VERSION_MINOR
 * @brief Minor version number of the library.
 * @note If this were version 1.2.3, this value would be 2.
 * @since This macro is available since 0.1.0 .
 */
#ifndef MYTOML_VERSION_MINOR
    #define MYTOML_VERSION_MINOR 1
#endif // MYTOML_VERSION_MINOR

/**
 * @def MYTOML_VERSION_PATCH
 * @brief Patch version number of the library.
 * @note If this were version 1.2.3, this value would be 3.
 * @since This macro is available since 0.1.0 .
 */
#ifndef MYTOML_VERSION_PATCH
    #define MYTOML_VERSION_PATCH 0
#endif // MYTOML_VERSION_PATCH

/**
 * @def MYTOML_VERSION
 * @brief Library version string in the format @c "X.Y.Z",
 * where @c X is the major version number, @c Y is a minor version
 * number, and @c Z is the patch version number.
 * @sa MyGetVersion
 */
#ifndef MYTOML_VERSION
    #define MYTOML_VERSION "0.1.0"
#endif // MYTOML_VERSION

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Include Mess
//-----------------------------------------------------------------------------

#include <exception>
#include <functional>
#include <memory>
#include <ostream>
#include <istream>
#include <string>
#include <vector>
#include <map>
#include <variant>

#include <stdio.h>
#include <stdint.h>
// #include <uchar.h> // char16_t, char32_t
#include <string.h>
#include <stdlib.h>

#include <iostream>

#include <optional>
    #include <iterator>

//-----------------------------------------------------------------------------
// [SECTION] Configurations
//-----------------------------------------------------------------------------

/**
 * @defgroup configure Library Configurations
 * @{
 */

/**
 * @def MYTOML_DISABLE_READER
 * @brief Exclude Reading/Deserialization APIs.
 *
 * Define to 1 to remove reader/parsing code at compile time when parsing
 * is not required. This reduces the compiled binary size.
 *
 * Example:
 * @code
 * #define MYTOML_DISABLE_READER 1
 * #include <mytoml/mytoml.hpp>
 * @endcode
 *
 * @note When disabled, all parsing classes and functions are not available.
 */
#ifndef MYTOML_DISABLE_READER
#endif

/**
 * @def MYTOML_DISABLE_WRITER
 * @brief Exclude Writing/Serialization methods.
 * Define as 1 to disable writer if serialization is not required.
 *
 * @warning This will disable these function at compile-time.
 *
 * @note This will reduce the binary size by about 30%.
 */
#ifndef MYTOML_DISABLE_WRITER
#endif

// Uncomment this to disable STL
// #define MYTOML_NO_STL

// Uncomment this to disable exceptions
// #define MYTOML_NO_EXCEPTIONS

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Function Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// [SECTION] Platform Defines
//-----------------------------------------------------------------------------

/**
 * @defgroup platform Platform Definitions
 * @{
 */

/**
 * @brief   Checks if the compiler is of given brand.
 * @param   name Platform, like `APPLE`.
 * @retval  true   It is.
 * @retval  false  It isn't.
 */
#define MYTOML_PLATFORM_IS(name) MYTOML_PLATFORM_IS_##name

/**
 * @brief  Returns the current platform name.
 * @return  platform name.
 */
#ifdef __APPLE__
	/**
	* A preprocessor macro that is only defined if compiling for MacOS.
	*/
	#define MYTOML_PLATFORM_IS_APPLE 1
	/**
	 * @brief  Returns the current platform name.
	 * @return  platform name.
	 */
  #define MYTOML_PLATFORM_NAME_IS "Apple"
#elif defined(linux) || defined(__linux) || defined(__linux__)
	/**
	* A preprocessor macro that is only defined if compiling for Linux.
	*/
	#define MYTOML_PLATFORM_IS_LINUX 1
  	/**
   	* @brief  Returns the current platform name.
   	* @return  platform name.
   	*/
  	#define MYTOML_PLATFORM_NAME_IS "Linux"
#elif defined(WIN32) || defined(__WIN32__) || defined(_WIN32) || defined(_MSC_VER) || defined(__MINGW32__)
  	/**
   	* A preprocessor macro that is only defined if compiling for Windows.
   	*/
  	#define MYTOML_PLATFORM_IS_WINDOWS 1
  	/**
   	* @brief  Returns the current platform name.
   	* @return  platform name.
   	*/
  	#define MYTOML_PLATFORM_NAME_IS "Windows"
#else
  	/**
   	* A preprocessor macro that is only defined if compiling for others.
   	*/
  	#define MYTOML_PLATFORM_IS_OTHERS 1
  	/**
   	* @brief  Returns the current platform name.
   	* @return  platform name.
   	*/
  	#define MYTOML_PLATFORM_NAME_IS "Others"
#endif

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Standard Defines
//-----------------------------------------------------------------------------

/**
 * @defgroup language Standard Checks
 * @{
 */

/** C version (STDC) */
#if defined(__STDC__) && (__STDC__ >= 1) && defined(__STDC_VERSION__)
	#define MYTOML_STDC __STDC_VERSION__
#else
	#define MYTOML_STDC 0
#endif

// With the MSVC compilers, the value of __cplusplus is by default always "199611L"(C++98).
// To avoid that, the library instead references _MSVC_LANG which is always set a correct value.
// See https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/ for more details.
#if defined(_MSVC_LANG) && !defined(__clang__)
    #define MYTOML_CPLUSPLUS _MSVC_LANG
#else
    #define MYTOML_CPLUSPLUS __cplusplus
#endif

// C++ language standard detection

#if !defined(MYTOML_HAS_CXX_26) && !defined(MYTOML_HAS_CXX_23) && !defined(MYTOML_HAS_CXX_20) && !defined(MYTOML_HAS_CXX_17) && !defined(MYTOML_HAS_CXX_14) && !defined(MYTOML_HAS_CXX_11)
    #if (defined(MYTOML_CPLUSPLUS) && MYTOML_CPLUSPLUS > 202302L)
        #define MYTOML_HAS_CXX_26
        #define MYTOML_HAS_CXX_23
        #define MYTOML_HAS_CXX_20
        #define MYTOML_HAS_CXX_17
        #define MYTOML_HAS_CXX_14
    #elif (defined(MYTOML_CPLUSPLUS) && MYTOML_CPLUSPLUS > 202002L)
        #define MYTOML_HAS_CXX_23
        #define MYTOML_HAS_CXX_20
        #define MYTOML_HAS_CXX_17
        #define MYTOML_HAS_CXX_14
    #elif (defined(MYTOML_CPLUSPLUS) && MYTOML_CPLUSPLUS > 201703L)
        #define MYTOML_HAS_CXX_20
        #define MYTOML_HAS_CXX_17
        #define MYTOML_HAS_CXX_14
    #elif (defined(MYTOML_CPLUSPLUS) && MYTOML_CPLUSPLUS > 201402L)
        #define MYTOML_HAS_CXX_17
        #define MYTOML_HAS_CXX_14
    #elif (defined(MYTOML_CPLUSPLUS) && MYTOML_CPLUSPLUS > 201103L)
        #define MYTOML_HAS_CXX_14
    #endif
    // Always specified because it is the minimal required version
    #define MYTOML_HAS_CXX_11
#endif

/** @} */


//-----------------------------------------------------------------------------
// [SECTION] Compiler Defines
//-----------------------------------------------------------------------------

/**
 * @defgroup compiler Compiler Definitions
 * @{
 */

/**
 * @brief   Checks if the compiler is of given brand.
 * @param   name  Compiler brand, like `MSVC`.
 * @retval  true   It is.
 * @retval  false  It isn't.
 */
#define MYTOML_COMPILER_IS(name) MYTOML_COMPILER_IS_##name

/// Compiler is apple
#if !defined(__clang__)
    #define MYTOML_COMPILER_IS_APPLE 0
#elif !defined(__apple_build_version__)
    #define MYTOML_COMPILER_IS_APPLE 0
#else
    #define MYTOML_COMPILER_IS_APPLE 1
    #define MYTOML_COMPILER_VERSION_MAJOR __clang_major__
    #define MYTOML_COMPILER_VERSION_MINOR __clang_minor__
    #define MYTOML_COMPILER_VERSION_PATCH __clang_patchlevel__
#endif

/// Compiler is clang
#if !defined(__clang__)
    #define MYTOML_COMPILER_IS_CLANG 0
#elif MYTOML_COMPILER_IS(APPLE)
    #define MYTOML_COMPILER_IS_CLANG 0
#else
    #define MYTOML_COMPILER_IS_CLANG 1
    #define MYTOML_COMPILER_VERSION_MAJOR __clang_major__
    #define MYTOML_COMPILER_VERSION_MINOR __clang_minor__
    #define MYTOML_COMPILER_VERSION_PATCH __clang_patchlevel__
#endif

/// Compiler is intel
#if !defined(__INTEL_COMPILER)
    #define MYTOML_COMPILER_IS_INTEL 0
#elif !defined(__INTEL_COMPILER_UPDATE)
    #define MYTOML_COMPILER_IS_INTEL 1
    /* __INTEL_COMPILER = XXYZ */
    #define MYTOML_COMPILER_VERSION_MAJOR (__INTEL_COMPILER / 100)
    #define MYTOML_COMPILER_VERSION_MINOR (__INTEL_COMPILER % 100 / 10)
    #define MYTOML_COMPILER_VERSION_PATCH (__INTEL_COMPILER % 10)
#else
    #define MYTOML_COMPILER_IS_INTEL 1
    /* __INTEL_COMPILER = XXYZ */
    #define MYTOML_COMPILER_VERSION_MAJOR (__INTEL_COMPILER / 100)
    #define MYTOML_COMPILER_VERSION_MINOR (__INTEL_COMPILER % 100 / 10)
    #define MYTOML_COMPILER_VERSION_PATCH __INTEL_COMPILER_UPDATE
#endif

/// Compiler is msc
#if !defined(_MSC_VER)
    #define MYTOML_COMPILER_IS_MSVC 0
#elif MYTOML_COMPILER_IS(CLANG)
    #define MYTOML_COMPILER_IS_MSVC 0
#elif MYTOML_COMPILER_IS(INTEL)
    #define MYTOML_COMPILER_IS_MSVC 0
#elif _MSC_VER >= 1400
    #define MYTOML_COMPILER_IS_MSVC 1
    /* _MSC_FULL_VER = XXYYZZZZZ */
    #define MYTOML_COMPILER_VERSION_MAJOR (_MSC_FULL_VER / 10000000)
    #define MYTOML_COMPILER_VERSION_MINOR (_MSC_FULL_VER % 10000000 / 100000)
    #define MYTOML_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 100000)
#elif defined(_MSC_FULL_VER)
    #define MYTOML_COMPILER_IS_MSVC 1
    /* _MSC_FULL_VER = XXYYZZZZ */
    #define MYTOML_COMPILER_VERSION_MAJOR (_MSC_FULL_VER / 1000000)
    #define MYTOML_COMPILER_VERSION_MINOR (_MSC_FULL_VER % 1000000 / 10000)
    #define MYTOML_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 10000)
#else
    #define MYTOML_COMPILER_IS_MSVC 1
    /* _MSC_VER = XXYY */
    #define MYTOML_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
    #define MYTOML_COMPILER_VERSION_MINOR (_MSC_VER % 100)
    #define MYTOML_COMPILER_VERSION_PATCH 0
#endif

/// Compiler is gcc
#if !defined(__GNUC__)
    #define MYTOML_COMPILER_IS_GCC 0
#elif MYTOML_COMPILER_IS(APPLE)
    #define MYTOML_COMPILER_IS_GCC 0
#elif MYTOML_COMPILER_IS(CLANG)
    #define MYTOML_COMPILER_IS_GCC 0
#elif MYTOML_COMPILER_IS(INTEL)
    #define MYTOML_COMPILER_IS_GCC 0
#else
    #define MYTOML_COMPILER_IS_GCC 1
    #define MYTOML_COMPILER_VERSION_MAJOR __GNUC__
    #define MYTOML_COMPILER_VERSION_MINOR __GNUC_MINOR__
    #define MYTOML_COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#endif

/**
 * @brief   Checks if the compiler is of given brand and is newer than or equal
 *          to the passed version.
 * @param   name     Compiler brand, like `MSVC`.
 * @param   x      Major version.
 * @param   y      Minor version.
 * @param   z      Patchlevel.
 * @retval  true   name >= x.y.z.
 * @retval  false  otherwise.
 */
#define MYTOML_COMPILER_SINCE(name, x, y, z)                                 \
  (MYTOML_COMPILER_IS(name) && ((MYTOML_COMPILER_VERSION_MAJOR > (x)) ||     \
                                ((MYTOML_COMPILER_VERSION_MAJOR == (x)) &&   \
                                 ((MYTOML_COMPILER_VERSION_MINOR > (y)) ||   \
                                  ((MYTOML_COMPILER_VERSION_MINOR == (y)) && \
                                   (MYTOML_COMPILER_VERSION_PATCH >= (z)))))))

/**
 * @brief   Checks if  the compiler  is of  given brand and  is older  than the
 *          passed version.
 * @param   name     Compiler brand, like `MSVC`.
 * @param   x      Major version.
 * @param   y      Minor version.
 * @param   z      Patchlevel.
 * @retval  true   name < x.y.z.
 * @retval  false  otherwise.
 */
#define MYTOML_COMPILER_BEFORE(name, x, y, z)                                \
  (MYTOML_COMPILER_IS(name) && ((MYTOML_COMPILER_VERSION_MAJOR < (x)) ||     \
                                ((MYTOML_COMPILER_VERSION_MAJOR == (x)) &&   \
                                 ((MYTOML_COMPILER_VERSION_MINOR < (y)) ||   \
                                  ((MYTOML_COMPILER_VERSION_MINOR == (y)) && \
                                   (MYTOML_COMPILER_VERSION_PATCH < (z)))))))


// exclude unsupported compilers
#ifndef MYTOML_SKIP_COMPILER_CHECK
    #if MYTOML_COMPILER_IS_CLANG
        #if (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) < 30400
            #error "Unsupported Clang Compiler version."
        #endif
    #elif MYTOML_COMPILER_IS_GCC
        #if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40800
            #error "Unsupported GCC Compiler version."
        #endif
    #endif
#endif

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Compiler Warnings
//-----------------------------------------------------------------------------

/**
 * @defgroup compiler Compiler Warnings
 * @{
 */

#if MYTOML_COMPILER_IS(CLANG)
    #define MYTOML_PRAGMA_TO_STR(x) _Pragma(#x)
    #define MYTOML_CLANG_SUPPRESS_WARNING_PUSH _Pragma("clang diagnostic push")
    #define MYTOML_CLANG_SUPPRESS_WARNING(w) \
    MYTOML_PRAGMA_TO_STR(clang diagnostic ignored w)
    #define MYTOML_CLANG_SUPPRESS_WARNING_POP _Pragma("clang diagnostic pop")
    #define MYTOML_CLANG_SUPPRESS_WARNING_WITH_PUSH(w) \
    MYTOML_CLANG_SUPPRESS_WARNING_PUSH MYTOML_CLANG_SUPPRESS_WARNING(w)
#else // MYTOML_CLANG
    #define MYTOML_CLANG_SUPPRESS_WARNING_PUSH
    #define MYTOML_CLANG_SUPPRESS_WARNING(w)
    #define MYTOML_CLANG_SUPPRESS_WARNING_POP
    #define MYTOML_CLANG_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // MYTOML_CLANG

#if MYTOML_COMPILER_IS(GCC)
    #define MYTOML_PRAGMA_TO_STR(x) _Pragma(#x)
    #define MYTOML_GCC_SUPPRESS_WARNING_PUSH _Pragma("GCC diagnostic push")
    #define MYTOML_GCC_SUPPRESS_WARNING(w) \
    MYTOML_PRAGMA_TO_STR(GCC diagnostic ignored w)
    #define MYTOML_GCC_SUPPRESS_WARNING_POP _Pragma("GCC diagnostic pop")
    #define MYTOML_GCC_SUPPRESS_WARNING_WITH_PUSH(w) \
    MYTOML_GCC_SUPPRESS_WARNING_PUSH MYTOML_GCC_SUPPRESS_WARNING(w)
#else // MYTOML_GCC
    #define MYTOML_GCC_SUPPRESS_WARNING_PUSH
    #define MYTOML_GCC_SUPPRESS_WARNING(w)
    #define MYTOML_GCC_SUPPRESS_WARNING_POP
    #define MYTOML_GCC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // MYTOML_GCC

#if MYTOML_COMPILER_IS(MSVC)
    #define MYTOML_MSVC_SUPPRESS_WARNING_PUSH __pragma(warning(push))
    #define MYTOML_MSVC_SUPPRESS_WARNING(w) __pragma(warning(disable : w))
    #define MYTOML_MSVC_SUPPRESS_WARNING_POP __pragma(warning(pop))
    #define MYTOML_MSVC_SUPPRESS_WARNING_WITH_PUSH(w) \
    MYTOML_MSVC_SUPPRESS_WARNING_PUSH MYTOML_MSVC_SUPPRESS_WARNING(w)
#else // MYTOML_MSVC
    #define MYTOML_MSVC_SUPPRESS_WARNING_PUSH
    #define MYTOML_MSVC_SUPPRESS_WARNING(w)
    #define MYTOML_MSVC_SUPPRESS_WARNING_POP
    #define MYTOML_MSVC_SUPPRESS_WARNING_WITH_PUSH(w)
#endif // MYTOML_MSVC

/** @} */

//-----------------------------------------------------------------------------
// [SECTION] Compiler Attributes
//-----------------------------------------------------------------------------

/**
 * @defgroup compiler Compiler Attributes
 * @{
 */

 /** compiler builtin check */
#ifndef MYTOML_HAS_BUILTIN
	#ifdef __has_builtin
    	#define MYTOML_HAS_BUILTIN(x) __has_builtin(x)
  	#else
    	#define MYTOML_HAS_BUILTIN(x) 0
  	#endif
#endif

/** compiler attribute check */
#ifndef MYTOML_HAS_ATTRIBUTE
  	#ifdef __has_attribute
    	#define MYTOML_HAS_ATTRIBUTE(x) __has_attribute(x)
  	#else
    	#define MYTOML_HAS_ATTRIBUTE(x) 0
  	#endif
#endif

/** compiler feature check */
#ifndef MYTOML_HAS_FEATURE
  	#ifdef __has_feature
    	#define MYTOML_HAS_FEATURE(x) __has_feature(x)
  	#else
    	#define MYTOML_HAS_FEATURE(x) 0
  	#endif
#endif

/** compiler include check */
#ifndef MYTOML_HAS_INCLUDE
  	#ifdef __has_include
    	#define MYTOML_HAS_INCLUDE(x) __has_include(x)
  	#else
    	#define MYTOML_HAS_INCLUDE(x) 0
  	#endif
#endif

/** compiler cpp attribute check */
#ifndef MYTOML_HAS_CPP_ATTRIBUTE
  	#ifdef __has_cpp_attribute
        #define MYTOML_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
  	#else
    	#define MYTOML_HAS_CPP_ATTRIBUTE(x) 0
  	#endif
#endif


 /** inline for compiler */
#ifndef MYTOML_INLINE
  	#if MYTOML_COMPILER_SINCE(MSVC, 12, 0, 0)
    	#define MYTOML_INLINE __forceinline
  	#elif MYTOML_COMPILER_IS(INTEL)
    	#define MYTOML_INLINE __inline
  	#elif MYTOML_HAS_ATTRIBUTE(always_inline) || MYTOML_COMPILER_SINCE(GCC, 4, 0, 0)
    	#define MYTOML_INLINE __inline__ __attribute__((always_inline))
  	#elif MYTOML_COMPILER_IS(CLANG) || MYTOML_COMPILER_IS(GCC)
    	#define MYTOML_INLINE __inline__
  	#elif defined(__cplusplus) || MYTOML_STDC >= 199901L
    	#define MYTOML_INLINE inline
  	#else
    	#define MYTOML_INLINE
  	#endif
#endif

/** noinline for compiler */
#ifndef MYTOML_NOINLINE
  	#if MYTOML_COMPILER_SINCE(MSVC, 14, 0, 0)
    	#define MYTOML_NOINLINE __declspec(noinline)
  	#elif MYTOML_HAS_ATTRIBUTE(noinline) || (MYTOML_COMPILER_SINCE(GCC, 4, 0, 0))
    	#define MYTOML_NOINLINE __attribute__((noinline))
  	#else
    	#define MYTOML_NOINLINE
  	#endif
#endif

/** align for compiler */
#ifndef MYTOML_ALIGN
  	#if MYTOML_COMPILER_SINCE(MSVC, 13, 0, 0)
    	#define MYTOML_ALIGN(x) __declspec(align(x))
  	#elif MYTOML_HAS_ATTRIBUTE(aligned) || defined(__GNUC__)
    	#define MYTOML_ALIGN(x) __attribute__((aligned(x)))
  	#elif MYTOML_CPP_VERSION >= 201103L
    	#define MYTOML_ALIGN(x) alignas(x)
  	#else
    	#define MYTOML_ALIGN(x)
  	#endif
#endif

/** deprecate warning */
#if defined(MYTOML_HAS_CXX_14)
    #define MYTOML_DEPRECATED(msg) [[deprecated(msg)]]
#else
  	#if MYTOML_COMPILER_SINCE(MSVC, 14, 0, 0)
    	#define MYTOML_DEPRECATED(msg) __declspec(deprecated(msg))
  	#elif MYTOML_HAS_FEATURE(attribute_deprecated_with_message) || \
    (MYTOML_COMPILER_SINCE(GCC, 4, 0, 0) ||                    \
     (MYTOML_COMPILER_VERSION_MAJOR == 5 &&                    \
      MYTOML_COMPILER_VERSION_MINOR >= 5))
    	#define MYTOML_DEPRECATED(msg) __attribute__((deprecated(msg)))
  	#elif MYTOML_COMPILER_SINCE(MSVC, 3, 0, 0)
    	#define MYTOML_DEPRECATED(msg) __attribute__((deprecated))
  	#else
    	#define MYTOML_DEPRECATED(msg)
  	#endif
#endif

// switch usage of constexpr keyword depending on active C++ standard.
#if defined(MYTOML_HAS_CXX_17)
    #define MYTOML_CONSTEXPR constexpr
#else
    #define MYTOML_CONSTEXPR
#endif

#ifndef MYTOML_NODISCARD
    #define MYTOML_NODISCARD [[nodiscard]]
#endif

// switch usage of [[likely]] C++ attribute which has been available since C++20.
#if defined(MYTOML_HAS_CXX_20) && MYTOML_HAS_CPP_ATTRIBUTE(likely) >= 201803L
    #define MYTOML_LIKELY(expr) (!!(expr)) [[likely]]
#elif MYTOML_HAS_BUILTIN(__builtin_expect)
    #define MYTOML_LIKELY(expr) (__builtin_expect(!!(expr), 1))
#else
    #define MYTOML_LIKELY(expr) (!!(expr))
#endif

// switch usage of [[unlikely]] C++ attribute which has been available since C++20.
#if defined(MYTOML_HAS_CXX_20) && MYTOML_HAS_CPP_ATTRIBUTE(unlikely) >= 201803L
    #define MYTOML_UNLIKELY(expr) (!!(expr)) [[unlikely]]
#elif MYTOML_HAS_BUILTIN(__builtin_expect)
    #define MYTOML_UNLIKELY(expr) (__builtin_expect(!!(expr), 0))
#else
    #define MYTOML_UNLIKELY(expr) (!!(expr))
#endif


// switch usage of char8_t which has been available since C++20.
#if defined(MYTOML_HAS_CXX_20) && defined(__cpp_char8_t) && __cpp_char8_t >= 201811L
    #define MYTOML_HAS_CHAR8_T (1)
#else
    #define MYTOML_HAS_CHAR8_T (0)
#endif

/** @} */


//-----------------------------------------------------------------------------
// [SECTION] API Import/Export
//-----------------------------------------------------------------------------

/**
 * @defgroup export Export Definitions
 * @{
 */

#if defined(_WIN32)
    #define MYTOML_API_EXPORT __declspec(dllexport)
    #define MYTOML_API_IMPORT __declspec(dllimport)
    #define MYTOML_NO_EXPORT
#else  // _WIN32
    #define MYTOML_API_EXPORT __attribute__((visibility("default")))
    #define MYTOML_API_IMPORT __attribute__((visibility("default")))
    #define MYTOML_NO_EXPORT __attribute__((visibility("hidden")))
#endif  // _WIN32

/**
 * @def MYTOML_API
 * @brief Macro for public API symbol export/import.
 * @details Use this macro to annotate all public API functions for correct
 * symbol visibility on all platforms.
 */

#if defined(MYTOML_BUILD_STATIC)
    #define MYTOML_API
#elif defined(MYTOML_BUILD_SHARED) || defined(MYTOML_EXPORTS)
    /* We are building this library */
    #define MYTOML_API MYTOML_API_EXPORT
#elif defined(MYTOML_LOAD_SHARED) || defined(MYTOML_IMPORTS)
    /* We are using this library */
    #define MYTOML_API MYTOML_API_IMPORT
#else  // MYTOML_BUILD_STATIC
    #define MYTOML_API
#endif  // MYTOML_BUILD_STATIC

/** @} */

#define MYTOML_VERSION_CONCAT_(major, minor, patch) v##major##_##minor##_##patch
#define MYTOML_VERSION_CONCAT(major, minor, patch) MYTOML_VERSION_CONCAT_(major, minor, patch)
#define MYTOML_VERSION_NAMESPACE_BEGIN  inline namespace \
    MYTOML_VERSION_CONCAT(MYTOML_VERSION_MAJOR, MYTOML_VERSION_MINOR, MYTOML_VERSION_PATCH) {
#define MYTOML_VERSION_NAMESPACE_END    } /* inline namespace MYTOML_VERSION */

#if (defined(__cpp_exceptions) || defined(__EXCEPTIONS)) && !defined(MYTOML_NO_EXCEPTIONS)
    #define MYTOML_THROW(exception) throw exception
    #define MYTOML_TRY try
    #define MYTOML_CATCH(exception) catch(exception)
#else
  #define MYTOML_REQUIRE(expression, error)                                                        \
    do                                                                                             \
    {                                                                                              \
      if (MYTOML_UNLIKELY(!(expression)))                                                          \
      {                                                                                            \
        printf("Error: %s (%s:%d)\n", error, __FILE__, __LINE__);        \
        abort();                                                                                   \
      }                                                                                            \
    } while (0)
    
    #define MYTOML_THROWE(exception) MYTOML_REQUIRE(false, exception.what())
    #define MYTOML_THROW(exception) abort()
    #define MYTOML_TRY if(true)
    #define MYTOML_CATCH(exception) if(false)
#endif

#ifndef MYTOML_ASSERT
    #include <assert.h> // assert
    #define MYTOML_ASSERT(x) assert(x)
#else
    #define MYTOML_ASSERT(x)
#endif

#if MYTOML_COMPILER_SINCE(GCC, 6, 0, 0)
    #define MYTOML_QUOTE_OPERATOR operator""_toml
#else
    #define MYTOML_QUOTE_OPERATOR operator"" _toml
#endif

/** 
 * @brief One.
 *
 *  This is only semantic sugar for the number `1`.
 *  @note You can instead use `1` or `true` .
 *
 */
#define MYTOML_TRUE 1

/** 
 * @brief Zero.
 *
 *  This is only semantic sugar for the number `0`.
 *  @note You can instead use `0` or `false` .
 *
 */
#define MYTOML_FALSE 0

// clang-format on

#pragma region Forward

/**
 * @namespace mytoml
 * @brief Primary library namespace for the mytoml API.
 *
 * All public types, enums and functions are declared in this namespace.
 */
namespace mytoml
{
    MYTOML_VERSION_NAMESPACE_BEGIN

    /**
     * @namespace mytoml::detail
     * @brief Internal implementation details.
     *
     * Types and helpers in this namespace are not part of the public API
     * and are subject to change without notice.
     */
    namespace detail
    {
        //-----------------------------------------------------------------------------
        // [SECTION] Details Forward
        //-----------------------------------------------------------------------------

        /** Enumerations */

        enum class token_t : uint8_t; /** enum token type. */
        enum class error_t : uint8_t; /** enum token type. */
        enum class event_t : uint8_t; /** enum token type. */
        enum class value_t : uint8_t; /** enum token type. */
        enum class break_t : uint8_t; /** enum token type. */

        /** Structures */

        struct mark;  /** struct mark. */
        struct event; /** struct event. */
        struct token; /** struct event. */

#ifndef MYTOML_NO_EXCEPTIONS
        // exception
        class exception;      /** class exception. */
        class io_error;       /** class io_error. */
        class type_error;     /** class type_error. */
        class parse_error;    /** class parse_error. */
        class access_error;   /** class access_error. */
        class encoding_error; /** class encoding_error. */
#endif                        // MYTOML_NO_EXCEPTIONS

        // iterator

        template <typename node_type>
        struct iterator_holder; /** class iterator_holder. */

        template <typename node_type>
        class iterator; /** class iterator. */

        template <typename iterator_type>
        class reverse_iterator; /** class reverse_iterator. */

        // input
        class lexer;           /** class lexer. */
        class parser;          /** class parser. */
        class iadapter;        /** class iadapter. */
        class deserializer;    /** class deserializer. */
        class file_iadapter;   /** class file_iadapter. */
        class memory_iadapter; /** class memory_iadapter. */

#ifndef MYTOML_NO_STL
        class stream_iadapter;   /** class stream_iadapter. */
        class iterator_iadapter; /** class iterator_iadapter. */
#endif                           // MYTOML_NO_STL

        // output
        class emitter;         /** class emitter. */
        class oadapter;        /** class oadapter. */
        class serializer;      /** class serializer. */
        class file_iadapter;   /** class file_iadapter. */
        class memory_oadapter; /** class memory_oadapter. */

#ifndef MYTOML_NO_STL
        class stream_oadapter;   /** class stream_oadapter. */
        class iterator_oadapter; /** class iterator_oadapter. */
#endif                           // MYTOML_NO_STL

        // encoding
        struct utf8;  /** struct utf8. */
        struct utf16; /** struct utf16. */
        struct utf32; /** struct utf32. */

    } // namespace detail

    MYTOML_VERSION_NAMESPACE_END

} // namespace mytoml

/**
 * @namespace mytoml
 * @brief The Api namespace mytoml::
 */
namespace mytoml
{
    MYTOML_VERSION_NAMESPACE_BEGIN

    //-----------------------------------------------------------------------------
    // [SECTION] Mytoml Forward
    //-----------------------------------------------------------------------------

    /** Enumerations */

    enum class encoding : uint8_t;
    enum class node_t : uint8_t;

    /** Structures */

    class toml;    /** class toml. */
    class version; /** class version. */

    MYTOML_VERSION_NAMESPACE_END

} // namespace mytoml

namespace mytoml
{
    MYTOML_VERSION_NAMESPACE_BEGIN

    /**
     * @namespace literals
     * @brief The Literals namespace mytoml::literals::
     */
    namespace literals
    {
        //-----------------------------------------------------------------------------
        // [SECTION] Literals Forward
        //-----------------------------------------------------------------------------

    } // namespace literals

    MYTOML_VERSION_NAMESPACE_END

} // namespace mytoml

#pragma endregion // Forward

#pragma region Detail

/**
 * @namespace mytoml
 * @brief The API namespace mytoml::
 */
namespace mytoml
{
    MYTOML_VERSION_NAMESPACE_BEGIN

    /**
     * @namespace detail
     * @brief The Details namespace mytoml::detail::
     */
    namespace detail
    {
        //-----------------------------------------------------------------------------
        // [SECTION] Details : Enums
        //-----------------------------------------------------------------------------

        /**
         * @defgroup enum Enums
         * @brief Detail enum types and flags.
         * @{
         */

        enum class token_t : uint8_t
        {
            unknown,
            stream_end,
            end_of_input,
            stream_start,
            object_start,
            object_end,
            array_start,
            array_end,
            name_separator,
            value_separator,
            string_value,
            basic_string,
            literal_string,
            multiline_literal_string,
            comment,
            number_value,
            true_literal,
            false_literal,
            null_literal,
            datetime_literal,
            time_literal,
            date_literal,
            multiline_basic_string,

        };

        enum class error_t : uint8_t
        {
            unknown,
            lexer,
            parser,
            reader,
            writer,
            emitter,
            encoding,
            decoding,
        };

        enum class event_t : uint8_t
        {
            unknown,
        };

        enum class value_t : uint8_t
        {
            unknown,
            null,    ///< null value
            table,   ///< table (map of string to toml)
            array,   ///< array (vector of toml values)
            string,  ///< string value
            number,  ///< numeric value (floating point)
            integer, ///< integer value
            boolean, ///< boolean value

            datetime_literal,
            time_literal,
            date_literal,
            multiline_basic_string,
            null_literal,
            false_literal,
            true_literal,

            empty,
            floating,
            offset_datetime,
            local_datetime,
            local_date,
            local_time,

        };

        enum class break_t : uint8_t
        {
            any, /** Let the parser choose the break type. */
            cr,  /** Use CR for line breaks (Mac style). */
            ln,  /** Use LN for line breaks (Unix style). */
            crln /** Use CR LN for line breaks (DOS style). */

        };

        /** @} group enum */

        //-----------------------------------------------------------------------------
        // [SECTION] Details : Data Structures
        //-----------------------------------------------------------------------------

        /**
         * @defgroup structs Data Structures
         * @brief Detail types and data structures.
         * @{
         */

        struct mark
        {
            size_t column{0}; /** The position column. */
            size_t index{0};  /** The position index. */
            size_t line{0};   /** The position line. */
        };

        struct event
        {
            event_t type{event_t::unknown}; /** The event type. */
            mark start{};                   /** The beginning of the token. */
            mark end{};                     /** The end of the token. */
        };

        struct token
        {
            token_t type{token_t::unknown}; /** The token type. */
#ifndef MYTOML_NO_STL
            std::string text; /** Token text/value (for string/number). */
#else
            union
            {
                char *value;
            } value;
#endif
            mark start{}; /** The beginning of the token. */
            mark end{};   /** The end of the token. */
        };

        /** @} group structs */

        //-----------------------------------------------------------------------------
        // [SECTION] Details : Traits
        //-----------------------------------------------------------------------------

        /**
         * @defgroup traits Traits
         * @brief Traits.
         * @{
         */

        /** @} group traits */

        //-----------------------------------------------------------------------------
        // [SECTION] Details : Exception
        //-----------------------------------------------------------------------------

#ifndef MYTOML_NO_EXCEPTIONS

        /**
         * @defgroup exception
         * @brief
         */

        /**
         * @class mytoml::exception
         * @brief A base exception class used in library.
         */
        class exception : public std::exception
        {
        public:
            /**
             * @brief Construct a new exception object without any error messages.
             */
            exception() = default;

            /**
             * @brief Construct a new exception object with an error messages.
             * @param[in] message An error message.
             */
            explicit exception(const char *message) noexcept;

            /**
             * @brief Returns an error message internally held. If nothing, a non-null,
             * empty string will be returned.
             * @return An error message internally held. The message might be empty.
             */
            MYTOML_NODISCARD const char *what() const noexcept override;

        private:
            std::string m_Message; /** An error message holder. */
        };

        class parse_error : public exception
        {
        public:
            /**
             * @brief Construct a new parse_error object.
             *
             * @param message An error message.
             */
            explicit parse_error(const char *message) noexcept;

            /**
             * @brief Construct a new encoding_error object.
             *
             * @param msg An error message.
             * @param mark The error position.
             */
            parse_error(const char *message, detail::mark mark) noexcept;

        private:
            /**
             * @brief Generate an error message from the given parameters.
             *
             * This helper constructs a human-readable error message that
             * includes the supplied @p message and positional information from
             * @p mark (line, column, index). The returned C-string pointer is a
             * pointer into an internal, thread-local buffer owned by the
             * implementation. The pointer is valid until the next call to this
             * function on the same thread. Callers (for example the
             * exception constructors) should immediately copy the returned
             * string if they need to retain it long-term.
             *
             * @param message An error message. May be nullptr.
             * @param mark The error position.
             *
             * @return Pointer to a null-terminated C-string describing the error.
             */
            static const char *generate(const char *message, detail::mark mark) noexcept;
        };

        class encoding_error : public exception
        {
        public:
            /**
             * @brief Construct a new encoding_error object.
             *
             * @param message An error message.
             */
            explicit encoding_error(const char *message) noexcept;

            /**
             * @brief Construct a new encoding_error object.
             *
             * @param encoding The encoding.
             * @param message An error message.
             * @param data The Encoded character.
             * @param size Number of bytes of data.
             */
            encoding_error(encoding encoding_type, const char *message, void *data, size_t size) noexcept;

        private:
            /**
             * @brief Generate an error message from encoding-related parameters.
             *
             * Builds a human-readable message including the @p message, the
             * detected @p encoding and a short hex representation of the
             * problematic @p data (up to a small limit). The returned pointer
             * points into an internal, thread-local buffer and is valid until
             * the next call to this function on the same thread. Callers must
             * copy the string if they need to keep it beyond the immediate use
             * (the exception constructors copy it into their member storage).
             *
             * @param encoding The detected encoding for the data.
             * @param message An error message. May be nullptr.
             * @param data Pointer to the raw encoded character bytes, or nullptr.
             * @param size Number of bytes available at @p data.
             *
             * @return Pointer to a null-terminated C-string describing the error.
             */
            static const char *generate(encoding encoding, const char *message, void *data, size_t size) noexcept;
        };

        class type_error : public exception
        {
        };

        class access_error : public exception
        {
        };

        /** @} group exception */

#endif // MYTOML_NO_EXCEPTIONS

        //-----------------------------------------------------------------------------
        // [SECTION] Details : Encoding
        //-----------------------------------------------------------------------------

        /**
         * @defgroup encoding
         * @brief Encoding helpers for UTF-8/16/32 and byte-order utilities.
         *
         * These helpers are used internally by the parser and emitter to
         * detect input encodings, decode individual code points and encode
         * code points back into the target encoding.
         */

        using encoding = mytoml::encoding;

#ifdef endian
#undef endian
#endif

        /**
         * @brief Internal numeric endianness representation.
         */
        using mytoml_endian_value_t = uint16_t;

        struct mytoml_endian_t
        {
#if MYTOML_COMPILER_IS_GCC
            static constexpr mytoml_endian_value_t little = __ORDER_LITTLE_ENDIAN__;
            static constexpr mytoml_endian_value_t big = __ORDER_BIG_ENDIAN__;
            static constexpr mytoml_endian_value_t native = __BYTE_ORDER__;
#else
            static constexpr mytoml_endian_value_t little = 0;
            static constexpr mytoml_endian_value_t big = 1;
            static constexpr mytoml_endian_value_t native = little;
#endif // MYTOML_COMPILER_IS_GCC
        };

        using endian = mytoml_endian_t;

        /**
         * @brief Determine the text encoding of the supplied buffer.
         *
         * This function performs a best-effort inspection of the initial
         * bytes available at @p data (up to @p size) and returns a
         * corresponding mytoml::encoding value describing the likely
         * encoding of the input text. Detection considers byte-order
         * marks (BOM), typical UTF-8/16/32 headers, and short sequences
         * that are unambiguous. The function never throws.
         *
         * Usage:
         * - Call with the pointer to the first byte of the input and the
         *   number of bytes available for inspection. Typical callers pass
         *   the whole buffer or the first 4 bytes. If @p size is 0 the
         *   function will return mytoml::encoding::unspecified.
         *
         * @param data Pointer to the input bytes to examine. May be nullptr
         *             if @p size is zero.
         * @param size Number of bytes available at @p data.
         *
         * @returns A value from mytoml::encoding describing the detected encoding
         *      and mytoml::encoding::unspecified if detection failed or
         *      the input is insufficient to make a determination.
         */
        encoding determine_encoding(void *data, size_t size);

        /**
         * @brief UTF-8 encoding helpers.
         *
         * The helper exposes low-level operations used by the parser and
         * encoder to convert between encoded byte sequences and Unicode
         * code points. Functions return the number of bytes consumed or
         * produced on success, and -1 on error.
         *
         * Typical usage examples:
         * - decode(): pass a pointer to UTF-8 bytes and available size to
         *   obtain the decoded Unicode code point and number of bytes
         *   consumed.
         * - encode(): provide a Unicode code point and an output buffer
         *   to write the UTF-8 encoded bytes.
         */
        struct utf8
        {

#if MYTOML_HAS_CHAR8_T
            using char_t = char8_t;
#else
            using char_t = char;
#endif // MYTOML_HAS_CHAR8_T

            /**
             * @brief Decode a single UTF-8 code point from a byte buffer.
             *
             * Reads 1..4 bytes from @p data (not more than @p size) and
             * writes the resulting Unicode code point into @p value.
             *
             * @param data Pointer to UTF-8 bytes.
             * @param size Number of bytes available.
             * @param[out] value Decoded Unicode code point on success.
             * @return Number of bytes consumed (1..4) or -1 on error.
             */
            static int decode(const char *data, size_t size, unsigned int &value);

            /**
             * @brief Encode a Unicode code point as UTF-8.
             *
             * Writes the UTF-8 byte sequence for @p codepoint into
             * @p output if there is enough room (@p size bytes available).
             *
             * @param codepoint The Unicode code point to encode.
             * @param[out] output Destination buffer for encoded bytes.
             * @param size Size of the destination buffer in bytes.
             * @return Number of bytes written (1..4) or -1 on error.
             */
            static int encode(unsigned int codepoint, char_t *output, size_t size);

            /**
             * @brief Convert a UTF-8 std::string to a UTF-16 byte vector.
             *
             * This helper is convenience for producing a sequence of bytes
             * representing UTF-16 code units (endian-aware) from a UTF-8
             * C++ string.
             *
             * @param string Input UTF-8 encoded string.
             * @param order Desired byte order for output (default: native).
             * @return A vector of bytes containing the UTF-16 encoding.
             */
            static std::vector<unsigned char> to_utf16(const std::string &string, mytoml_endian_value_t order);

            /**
             * @brief Convert a UTF-8 std::string to a UTF-32 byte vector.
             *
             * Similar to to_utf16() but produces UTF-32 (4 bytes per
             * code point), arranged according to @p order.
             *
             * @param string Input UTF-8 encoded string.
             * @param order Desired byte order for output (default: native).
             * @return A vector of bytes containing the UTF-32 encoding.
             */
            static std::vector<unsigned char> to_utf32(const std::string &string, mytoml_endian_value_t order);
        };

        /**
         * @brief UTF-16 encoding helpers.
         *
         * Handles decoding of one or two UTF-16 code units (surrogate
         * pairs) into a single Unicode code point and encoding code
         * points into UTF-16 code units. All functions accept an
         * explicit @p order describing byte order for multi-byte units.
         */
        struct utf16
        {
            using char_t = char16_t;

            /**
             * @brief Decode a UTF-16 code unit sequence into a unicode code point.
             *
             * The function reads 2 or 4 bytes depending on whether a surrogate
             * pair is present. The @p data pointer is treated as a byte pointer
             * using the @p order endianness otherwise native endianness.
             *
             * @param data Pointer to UTF-16 data (bytes).
             * @param size Number of bytes available at @p data.
             * @param[out] value Decoded Unicode code point on success.
             * @param order Order to interprete incoming bytes.
             * @return Number of bytes consumed (2 or 4) or -1 on error.
             */
            static int decode(const char *data, size_t size, unsigned int &value, mytoml_endian_value_t order);

            /**
             * @brief Encode a Unicode code point into UTF-16 code units.
             *
             * Writes one or two @p codepoint (2 or 4 bytes) to @p output
             * depending on whether the code point requires a surrogate pair
             * using the @p order endianness otherwise the native endianness.
             *
             * @param codepoint The Unicode code point to encode.
             * @param[out] output Destination buffer for UTF-16 code units.
             * @param size Size of the destination buffer in bytes.
             * @param order Order to represent/write the code units (codepoint) in.
             * @return Number of UTF-16 bytes written (2 or 4) or -1 on error.
             */
            static int encode(unsigned int codepoint, char_t *output, size_t size, mytoml_endian_value_t order);

            /**
             * @brief Convert a UTF-16 byte vector into a UTF-8 std::string.
             *
             * Interprets @p bytes as UTF-16 code units in the given
             * @p order and returns the UTF-8 encoded form.
             *
             * @param bytes Byte vector containing UTF-16 code units.
             * @param order Byte order of the input data.
             * @return UTF-8 encoded std::string on success. If input is
             *         ill-formed the function will attempt best-effort
             *         conversion and may replace invalid sequences.
             */
            static std::string to_utf8(const std::vector<unsigned char> &bytes, mytoml_endian_value_t order);
        };

        /**
         * @brief UTF-32 encoding helpers.
         *
         * UTF-32 uses a fixed 4-byte representation per Unicode code
         * point. These helpers decode and encode individual code points
         * and can convert between byte vectors and UTF-8 strings.
         */
        struct utf32
        {
            using char_t = char32_t;

            /**
             * @brief Decode a UTF-32 encoded value.
             *
             * Reads up to 4 bytes from @p data interpreting them according
             * to @p order and returns the decoded code point.
             *
             * @param data Pointer to input bytes containing a UTF-32 unit.
             * @param size Number of bytes available at @p data (should be >=4).
             * @param[out] value Decoded Unicode code point on success.
             * @param order Byte order of the input bytes.
             * @return Number of bytes consumed (4) on success, or -1 on error.
             */
            static int decode(const char *data, size_t size, unsigned int &value, mytoml_endian_value_t order);

            /**
             * @brief Encode a Unicode code point as UTF-32.
             *
             * Writes 4 bytes for @p codepoint into @p output using the
             * specified @p order.
             *
             * @param codepoint Unicode code point to encode.
             * @param[out] output Buffer to receive char32_t values.
             * @param size Number of char32_t entries available in @p output.
             * @param order Byte order to use for the output.
             * @return Number of units written (1 == 4 bytes) or -1 on error.
             */
            static int encode(unsigned int codepoint, char_t *output, size_t size, mytoml_endian_value_t order);

            /**
             * @brief Convert a UTF-32 byte vector into a UTF-8 std::string.
             *
             * @param bytes Byte vector containing UTF-32 code units.
             * @param order Byte order of the input data.
             * @return UTF-8 encoded std::string on success. Invalid input
             *         sequences are handled best-effort.
             */
            static std::string to_utf8(const std::vector<unsigned char> &bytes, mytoml_endian_value_t order);
        };

        /** @} group encoding */

        //-----------------------------------------------------------------------------
        // [SECTION] Details : Iterators
        //-----------------------------------------------------------------------------

        /**
         * @defgroup iterator Iterators
         * @brief Detail iterator classes.
         * @{
         */

        template <typename node_type>
        struct iterator_holder
        {
            using value_type = typename std::remove_const<node_type>::type;
            using table_iterator = typename std::conditional<
                std::is_const<node_type>::value,
                typename value_type::table_t::const_iterator,
                typename value_type::table_t::iterator>::type;

            using array_iterator = typename std::conditional<
                std::is_const<node_type>::value,
                typename value_type::array_t::const_iterator,
                typename value_type::array_t::iterator>::type;

            table_iterator table_iter{}; /** Underlying table iterator. */
            array_iterator array_iter{}; /** Underlying array iterator. */
        };

        template <typename node_type>
        struct iterator_traits
        {
            using value_type = typename std::remove_const<node_type>::type;
            using pointer = node_type *;
            using reference = node_type &;
            using const_pointer = const value_type *;
            using const_reference = const value_type &;
            using difference_type = std::ptrdiff_t;
            using table_iterator = typename std::conditional<
                std::is_const<node_type>::value,
                typename value_type::table_t::const_iterator,
                typename value_type::table_t::iterator>::type;

            using array_iterator = typename std::conditional<
                std::is_const<node_type>::value,
                typename value_type::array_t::const_iterator,
                typename value_type::array_t::iterator>::type;
        };

        /**
         * @class iterator
         * @brief Bidirectional iterator over TOML object and array values.
         *
         * The iterator stores either an table iterator or an array iterator and
         * exposes a unified interface used by the public toml iterator API.
         */
        template <typename node_type>
        class iterator
        {
            using other_iterator = typename std::conditional<
                std::is_const<node_type>::value, iterator<typename std::remove_const<node_type>::type>,
                iterator<const node_type>>::type;

            friend other_iterator;

        public:
            using traits_type = iterator_traits<node_type>;            /** A type for iterator traits.  */
            using iterator_category = std::bidirectional_iterator_tag; /** A type for iterator category tag. */

            using value_type = typename traits_type::value_type;           /** A toml object type. */
            using pointer = typename traits_type::pointer;                 /** A pointer to a toml object. */
            using reference = typename traits_type::reference;             /** A reference to a toml object. */
            using const_pointer = typename traits_type::const_pointer;     /** A constant pointer to a toml object. */
            using const_reference = typename traits_type::const_reference; /** A constant reference to a toml object. */
            using difference_type = typename traits_type::difference_type; /** A differences between toml iterators. */

            using table_iterator = typename traits_type::table_iterator;
            using array_iterator = typename traits_type::array_iterator;

        public:
            /**
             * @brief Runtime category of the active underlying iterator.
             */
            enum class iterator_t : uint8_t
            {
                table, /** table iterator type. */
                array, /** array iterator type. */
            };

        public:
            /**
             * @brief Default constructor
             */
            iterator() = default;

            /**
             * @brief Default copy constructor
             */
            iterator(const iterator &) = default;

            /**
             * @brief Default move constructor
             */
            iterator(iterator &&) = default;

            /**
             * @brief Construct begin-iterator from a TOML node.
             * @param toml Pointer to TOML container (array/object).
             */
            explicit iterator(pointer toml) noexcept;

            /**
             * @brief Convert mutable iterator to const iterator.
             */
            template <typename T = node_type,
                      typename std::enable_if<std::is_const<T>::value, int>::type = 0>
            iterator(const other_iterator &other) noexcept;

            /**
             * @brief Construct from table iterator.
             */
            iterator(const table_iterator &itr) noexcept;

            /**
             * @brief Construct from array iterator.
             */
            iterator(const array_iterator &itr) noexcept;

            /**
             * @brief Get runtime iterator category.
             */
            iterator_t type() const noexcept;

            /**
             * @brief Get object key at current position.
             * @throws exception when iterating non-object values.
             */
            const std::string &key() const;

            /**
             * @brief Get referenced TOML value.
             */
            reference value() const noexcept;

            /**
             * @brief Default copy assignment operator
             */
            iterator &operator=(const iterator &) = default;

            /**
             * @brief Default move assignment operator
             */
            iterator &operator=(iterator &&) = default;

            bool operator==(const iterator &rhs) const;
            bool operator==(const other_iterator &rhs) const;

            bool operator!=(const iterator &rhs) const;
            bool operator!=(const other_iterator &rhs) const;

            bool operator<(const iterator &rhs) const;
            bool operator<(const other_iterator &rhs) const;

            bool operator<=(const iterator &rhs) const;
            bool operator<=(const other_iterator &rhs) const;

            bool operator>(const iterator &rhs) const;
            bool operator>(const other_iterator &rhs) const;

            bool operator>=(const iterator &rhs) const;
            bool operator>=(const other_iterator &rhs) const;

            /**
             * @brief Access the referenced TOML value as a pointer.
             */
            pointer operator->() noexcept;

            /**
             * @brief Dereference the iterator.
             */
            reference operator*() const noexcept;

            /**
             * @brief Return an iterator advanced by @p i steps.
             */
            iterator operator+(difference_type i) const noexcept;

            /**
             * @brief Advance the iterator by @p i steps.
             */
            iterator &operator+=(difference_type i) noexcept;

            /**
             * @brief Pre-increment the iterator.
             */
            iterator &operator++() noexcept;

            /**
             * @brief Post-increment the iterator.
             */
            iterator operator++(int) & noexcept;

            /**
             * @brief Return an iterator moved backward by @p i steps.
             */
            iterator operator-(difference_type i) const noexcept;

            /**
             * @brief Move the iterator backward by @p i steps.
             */
            iterator &operator-=(difference_type i) noexcept;

            /**
             * @brief Pre-decrement the iterator.
             */
            iterator &operator--() noexcept;

            /**
             * @brief Post-decrement the iterator.
             */
            iterator operator--(int) & noexcept;

            /**
             * @brief Default destructor.
             */
            ~iterator() = default;

        private:
            pointer m_table = nullptr;             /** Parent TOML node associated with this iterator. */
            iterator_t m_type{iterator_t::table};  /** Active iterator branch (object or array). */
            iterator_holder<node_type> m_holder{}; /** Storage for the active underlying STL iterator. */
        };

        /**
         * @class reverse_iterator
         * @brief Reverse iterator adapter for toml iterators.
         *
         * Wraps a forward toml iterator and provides reverse traversal semantics
         * compatible with STL reverse iterators.
         */
        template <typename iterator_type>
        class reverse_iterator : public std::reverse_iterator<iterator_type>
        {
        public:
            using base_iterator = std::reverse_iterator<iterator_type>; /** A shortcut to the reverse iterator adapter. */
            using value_type = typename iterator_type::value_type;
            using pointer = typename iterator_type::pointer;                 /** A pointer to an iterator object. */
            using reference = typename iterator_type::reference;             /** A reference to an iterator object. */
            using const_pointer = typename iterator_type::const_pointer;     /** A constant pointer to an iterator object. */
            using const_reference = typename iterator_type::const_reference; /** A constant reference to an iterator object. */
            using difference_type = typename iterator_type::difference_type; /** Represent the differences between iterators. */

        public:
            /**
             * @brief Default constructor
             */
            reverse_iterator() = default;

            /**
             * @brief Construct from a forward iterator base.
             */
            reverse_iterator(const iterator_type &iter) noexcept;

            /**
             * @brief Construct from a std::reverse_iterator base object.
             */
            reverse_iterator(const base_iterator &iter) noexcept;

            /**
             * @brief Default copy constructor
             */
            reverse_iterator(const reverse_iterator &) = default;

            /**
             * @brief Default move constructor
             */
            reverse_iterator(reverse_iterator &&) = default;

            /**
             * @brief Get object key of the element currently referenced.
             */
            const std::string &key() const;

            /**
             * @brief Access the TOML value referenced by the reverse iterator.
             */
            reference value() const noexcept;

            /**
             * @brief Default copy assignment operator
             */
            reverse_iterator &operator=(const reverse_iterator &) = default;

            /**
             * @brief Default move assignment operator
             */
            reverse_iterator &operator=(reverse_iterator &&) = default;

            /**
             * @brief Random-access style offset read from current reverse position.
             */
            reference operator[](difference_type n) const;

            /**
             * @brief Return reverse iterator moved by @p i steps.
             */
            reverse_iterator operator+(difference_type i) const noexcept;

            /**
             * @brief Move reverse iterator by @p i steps.
             */
            reverse_iterator &operator+=(difference_type i) noexcept;

            /**
             * @brief Pre-increment (moves toward beginning in forward order).
             */
            reverse_iterator &operator++() noexcept;

            /**
             * @brief Post-increment reverse iterator.
             */
            reverse_iterator operator++(int) & noexcept;

            /**
             * @brief Return reverse iterator moved backward by @p i steps.
             */
            reverse_iterator operator-(difference_type i) const noexcept;

            /**
             * @brief Compute distance between two reverse iterators.
             */
            difference_type operator-(const reverse_iterator &other) const;

            /**
             * @brief Move reverse iterator backward by @p i steps.
             */
            reverse_iterator &operator-=(difference_type i) noexcept;

            /**
             * @brief Pre-decrement reverse iterator.
             */
            reverse_iterator &operator--() noexcept;

            /**
             * @brief Post-decrement reverse iterator.
             */
            reverse_iterator operator--(int) & noexcept;

            /**
             * @brief Default destructor.
             */
            ~reverse_iterator() = default;
        };

        /** @} group iterator */

        /**
         * @defgroup input
         * @brief
         * @{
         */

        /**
         * @brief Abstract base for input adapters used by the parser.
         *
         * Implementers must advance the internal reading position by the number of
         * bytes actually copied into the caller buffer. All methods return
         * std::nullopt on error to allow callers to detect failures uniformly.
         */
        class iadapter
        {
        public:
            /**
             * @brief Default constructor
             */
            iadapter() = default;

            /**
             * @brief Default copy constructor
             */
            iadapter(const iadapter &) = default;

            /**
             * @brief Default move constructor
             */
            iadapter(iadapter &&) noexcept = default;

            /**
             * @brief Read up to @p size bytes into @p data.
             *
             * Implementations must copy at most @p size bytes into the provided
             * buffer and advance the stream position accordingly.
             *
             * @param[out] data Buffer to receive the bytes. Must be at least @p size bytes.
             * @param[in] size Number of bytes requested.
             * @return The number of bytes actually read, or `0` if an error occurred.
             */
            virtual size_t read(void *data, size_t size) = 0;

            /**
             * @brief Default copy assignment operator
             */
            iadapter &operator=(const iadapter &) = default;

            /**
             * @brief Default move assignment operator
             */
            iadapter &operator=(iadapter &&) noexcept = default;

            /**
             * @brief Virtual destructor.
             */
            virtual ~iadapter() = default;
        };

        /**
         * @class file_iadapter
         * @brief Input adapter that reads from a C `FILE*`.
         */
        class file_iadapter : public iadapter
        {
        public:
            /**
             * @brief Construct the stream from a FILE pointer.
             * @param file File pointer to read from (must be valid for the lifetime).
             * @throws mytoml::exception on error.
             */
            explicit file_iadapter(FILE *file);

            /**
             * @brief Deleted copy constructor
             */
            file_iadapter(const file_iadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            file_iadapter(file_iadapter &&) noexcept = default;

            /**
             * @brief Read up to @p size bytes into @p data.
             *
             * @param[out] data Buffer to receive the bytes.
             * @param[in] size Number of bytes requested.
             * @return The number of bytes actually read, or `0` if an error occurred.
             */
            size_t read(void *data, size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            file_iadapter &operator=(const file_iadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            file_iadapter &operator=(file_iadapter &&) = delete;

        private:
            /** Member data */
            FILE *m_file;
        };

#ifndef MYTOML_NO_STL
        /**
         * @class stream_iadapter
         * @brief Input adapter that reads from a C++ `std::istream`.
         */
        class stream_iadapter : public iadapter
        {
        public:
            /**
             * @brief Construct a stream-based input adapter from an std::istream.
             * @param stream Reference to an opened input stream supplying bytes.
             */
            explicit stream_iadapter(std::istream &stream);

            /**
             * @brief Deleted copy constructor
             */
            stream_iadapter(const stream_iadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            stream_iadapter(stream_iadapter &&) noexcept = default;

            /**
             * @brief Read up to @p size bytes into @p data.
             *
             * @param[out] data Buffer to receive the bytes.
             * @param[in] size Number of bytes requested.
             * @return The number of bytes actually read, or `0` if an error occurred.
             */
            size_t read(void *data, size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            stream_iadapter &operator=(const stream_iadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            stream_iadapter &operator=(stream_iadapter &&) = delete;

        private:
            /** Member data */
            std::istream *m_stream;
        };

#endif // MYTOML_NO_STL

        /**
         * @class memory_iadapter
         * @brief Input adapter that reads from a `char*`.
         */
        class memory_iadapter : public iadapter
        {
        public:
            /**
             * @brief Construct the stream from a C-String pointer.
             * @param data Pointer to the data to read from.
             * @param size Size of the data, in bytes
             * @throws mytoml::exception on error.
             */
            memory_iadapter(void *data, size_t size);

            /**
             * @brief Deleted copy constructor
             */
            memory_iadapter(const memory_iadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            memory_iadapter(memory_iadapter &&) noexcept = default;

            /**
             * @brief Read up to @p size bytes into @p data.
             *
             * @param[out] data Buffer to receive the bytes.
             * @param[in] size Number of bytes requested.
             * @return The number of bytes actually read, or `0` if an error occurred.
             */
            size_t read(void *data, size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            memory_iadapter &operator=(const memory_iadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            memory_iadapter &operator=(memory_iadapter &&) = delete;

        private:
            /** Member data */
            size_t m_pos;
            size_t m_size;
            void *m_data;
        };

        /**
         * @class lexer
         * @brief Lexical Analyzer.
         *
         * The lexer reads bytes from an input adapter and produces tokens
         * consumed by the parser. It is a non-copyable, movable type.
         */
        class lexer
        {
        public:
            /**
             * @brief Construct a lexer from an input adapter.
             * @param adapter The input adapter providing raw bytes.
             */
            explicit lexer(iadapter *adapter);

            /**
             * @brief Deleted copy constructor
             */
            lexer(const lexer &) = delete;

            /**
             * @brief Deleted move constructor
             */
            lexer(lexer &&) noexcept = delete;

            /**
             * @brief Deleted copy assignment operator
             */
            lexer &operator=(const lexer &) = delete;

            /**
             * @brief Deleted move assignment operator
             */
            lexer &operator=(lexer &&) = delete;

            /**
             * @brief Advance the lexer and return the next token.
             *
             * This reads from the underlying input adapter as required and
             * updates the lexer's internal token state. The returned token
             * value represents the kind of token available.
             *
             * @return The next token produced by the lexer.
             */
            token next_token();

            /**
             * @brief Return the current token object.
             *
             * The returned token contains the token type and any value
             * information parsed by the lexer (for example string content).
             *
             * @return The current token.
             */
            [[nodiscard]] token get_token();

            /**
             * @brief Return the type of the current token.
             * @return The current token type.
             */
            [[nodiscard]] token_t get_type() const;

            [[nodiscard]] token scan();

            [[nodiscard]] mark position() const;

        private:
            /**
             * @defgroup position
             * @brief position methods.
             * @{
             */

            void advance(size_t amount = 1);

            void reverse(size_t amount = 1);

            /** @} group position */

            /**
             * @defgroup scanner
             * @brief scanner methods.
             * @{
             */

            // TOML specific string scanning
            bool scan_string();                   // Standard quoted string "..."
            bool scan_basic_string();             // Basic string '...'
            bool scan_literal_string();           // Literal string '''...'''
            bool scan_multiline_basic_string();   // Multiline basic """..."""
            bool scan_multiline_literal_string(); // Multiline literal ''''...''''

            bool scan_number(const std::string &current_text);           // Handles integers, floats, scientific notation, hex, octal, binary
            bool scan_literal(const std::string &current_text);          // Handles true, false, null
            bool scan_datetime_literal(const std::string &current_text); // Handles date, time, datetime
            bool scan_comment();                                         // Handles '#' comments

            // Helper to scan different TOML value types (numbers, booleans, datetimes, bare keys)
            bool scan_value();

            /** @} group scanner */

            int skip_ws();

            /**
             * @brief Read a character (byte) from the input adapter.
             * @return The read character value or EOF-like sentinel.
             */
            [[nodiscard]] int get_char();

            /**
             * @brief Push back the last-read character so it can be read again.
             */
            void unget_char();

            /**
             * @brief Append a character to the current token buffer.
             * @param character The character value to append.
             */
            void add_char(int charater);

        private:
            iadapter *m_adapter{nullptr};
            mytoml::encoding m_encoding;
            std::string m_string; //!
            std::string m_input{};
            size_t m_input_pos{0};
            int m_putback{-1};
            mark m_position{};
            token m_token{};
            int m_char{EOF};
        };

        /**
         * @class parser
         * @brief Recursive-descent TOML parser over a token stream.
         *
         * The parser consumes tokens from @ref lexer and materializes
         * a @ref mytoml::toml value.
         */
        class parser
        {
        public:
            /**
             * @brief Construct a parser from a lexer instance.
             * @param lexer Lexer producing TOML tokens.
             */
            explicit parser(class lexer *lexer);

            /**
             * @brief Deleted copy constructor.
             */
            parser(const parser &) = delete;

            /**
             * @brief Deleted move constructor.
             */
            parser(parser &&) = delete;

            /**
             * @brief Deleted copy assignment operator.
             */
            parser &operator=(const parser &) = delete;

            /**
             * @brief Deleted move assignment operator.
             */
            parser &operator=(parser &&) = delete;

            /**
             * @brief Parse one complete TOML value.
             * @return Parsed TOML value.
             */
            MYTOML_NODISCARD mytoml::toml parse();

        private:
            MYTOML_NODISCARD mytoml::toml parse_comment();
            MYTOML_NODISCARD mytoml::toml parse_boolean();
            MYTOML_NODISCARD mytoml::toml parse_floating();
            MYTOML_NODISCARD mytoml::toml parse_local_date();
            MYTOML_NODISCARD mytoml::toml parse_local_time();
            MYTOML_NODISCARD mytoml::toml parse_local_datetime();
            MYTOML_NODISCARD mytoml::toml parse_offset_datetime();
            MYTOML_NODISCARD mytoml::toml parse_null();
            MYTOML_NODISCARD mytoml::toml parse_key();
            MYTOML_NODISCARD mytoml::toml parse_value();
            MYTOML_NODISCARD mytoml::toml parse_array();
            MYTOML_NODISCARD mytoml::toml parse_table();
            MYTOML_NODISCARD mytoml::toml parse_table_key();
            MYTOML_NODISCARD mytoml::toml parse_simple_key();
            MYTOML_NODISCARD mytoml::toml parse_number(const std::string &text);
            MYTOML_NODISCARD std::string parse_string(const std::string &text);
            void advance();

        private:
            lexer *m_lexer{nullptr};
            token m_current{};
        };

        class deserializer
        {
        public:
            using value_type = toml;

            using key_type = std::string;
            using comment_type = std::string;
            using boolean_type = bool;
            using integer_type = int64_t;
            using floating_type = double;
            using string_type = std::string;
            using local_time_type = std::string;
            using local_date_type = std::string;
            using local_datetime_type = std::string;
            using offset_datetime_type = std::string;
            using array_type = std::vector<toml>;
            using table_type = std::map<std::string, toml>;

        public:
            /**
             * @brief Deleted copy constructor
             */
            deserializer(const deserializer &) = delete;

            /**
             * @brief Deleted move constructor
             */
            deserializer(deserializer &&) = delete;

            /**
             * @brief Deleted copy assignment operator
             */
            deserializer &operator=(const deserializer &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            deserializer &operator=(deserializer &&) = delete;
        };

        /** @} group input */

        /**
         * @defgroup output
         * @brief
         * @{
         */

        /**
         * @class oadapter
         * @brief Abstract base for output adapters used by the emitter.
         *
         * Implementations provide a concrete destination for emitted bytes
         * (files, memory buffers, ostream, etc.).
         */
        class oadapter
        {
        public:
            /**
             * @brief Default copy assignment operator
             */
            oadapter() = default;

            /**
             * @brief Default copy constructor
             */
            oadapter(const oadapter &) = default;

            /**
             * @brief Default move constructor
             */
            oadapter(oadapter &&) noexcept = default;

            /**
             * @brief Write up to @p size bytes from @p data into the stream.
             *
             * Implementations must copy at most @p size bytes from the provided
             * buffer and advance the stream position accordingly.
             *
             * @param data Pointer to the bytes to write.
             * @param size Number of bytes to write.
             * @return The number of bytes actually written, or `0` on error.
             */
            virtual size_t write(const void *data, size_t size) = 0;

            /**
             * @brief Default copy assignment operator
             */
            oadapter &operator=(const oadapter &) = default;

            /**
             * @brief Default move assignment operator
             */
            oadapter &operator=(oadapter &&) noexcept = default;

            /**
             * @brief Virtual destructor.
             */
            virtual ~oadapter() = default;
        };

        /**
         * @class file_oadapter
         * @brief Output adapter that writes to a C `FILE*`.
         */
        class file_oadapter : public oadapter
        {
        public:
            /**
             * @brief Construct the stream from a FILE pointer.
             * @param file File pointer to write to (must be valid for the lifetime).
             * @throws mytoml::exception on error.
             */
            explicit file_oadapter(FILE *file);

            /**
             * @brief Deleted copy constructor
             */
            file_oadapter(const file_oadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            file_oadapter(file_oadapter &&) noexcept = default;

            /**
             * @brief Write raw bytes to the underlying FILE*.
             * @param data Pointer to bytes to write.
             * @param size Number of bytes to write.
             * @return Number of bytes written or `0` on error.
             */
            size_t write(const void *data, size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            file_oadapter &operator=(const file_oadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            file_oadapter &operator=(file_oadapter &&) = delete;

        private:
            /** Member data */
            FILE *m_file;
        };

#ifndef MYTOML_NO_STL
        /**
         * @class stream_oadapter
         * @brief Output adapter that writes to a C++ `std::ostream`.
         */
        class stream_oadapter : public oadapter
        {
        public:
            /**
             * @brief Construct a stream-based output adapter from an std::ostream.
             * @param stream Reference to an opened output stream to write bytes to.
             */
            explicit stream_oadapter(std::ostream &stream);

            /**
             * @brief Deleted copy constructor
             */
            stream_oadapter(const stream_oadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            stream_oadapter(stream_oadapter &&) noexcept = default;

            /**
             * @brief Write up to @p size bytes from @p data into the stream.
             *
             * @param data Pointer to the bytes to write.
             * @param size Number of bytes to write.
             * @return The number of bytes actually written, or `0` on error.
             */
            size_t write(const void *data, size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            stream_oadapter &operator=(const stream_oadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            stream_oadapter &operator=(stream_oadapter &&) = delete;

        private:
            /** Member data */
            std::ostream *m_stream;
        };

#endif // MYTOML_NO_STL

        /**
         * @class memory_oadapter
         * @brief Output adapter that writes to a `char*`.
         */
        class memory_oadapter : public oadapter
        {
        public:
            /**
             * @brief Construct the stream from a C-String pointer.
             * @param data Pointer to the data to write to.
             * @param size Size of the data, in bytes
             * @throws mytoml::exception on error.
             */
            memory_oadapter(void *data, size_t size);

            /**
             * @brief Deleted copy constructor
             */
            memory_oadapter(const memory_oadapter &) = delete;

            /**
             * @brief Default move constructor
             */
            memory_oadapter(memory_oadapter &&) noexcept = default;

            /**
             * @brief Write up to @p size bytes from @p data into the stream.
             *
             * @param data Pointer to the bytes to write.
             * @param size Number of bytes to write.
             * @return The number of bytes actually written, or `0` on error.
             */
            size_t write(const void *data, size_t size) override;

            /**
             * @brief Deleted copy assignment operator
             */
            memory_oadapter &operator=(const memory_oadapter &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            memory_oadapter &operator=(memory_oadapter &&) = delete;

        private:
            /** Member data */
            size_t m_pos;
            size_t m_size;
            void *m_data;
        };

        class serializer
        {
        public:
            using value_type = toml;
            using key_type = std::string;
            using comment_type = std::string;
            using boolean_type = bool;
            using integer_type = int64_t;
            using floating_type = double;
            using string_type = std::string;
            using local_time_type = std::string;
            using local_date_type = std::string;
            using local_datetime_type = std::string;
            using offset_datetime_type = std::string;
            using array_type = std::vector<toml>;
            using table_type = std::map<std::string, toml>;

            using char_type = std::string::value_type;

        public:
            /**
             * @brief Deleted copy constructor
             */
            serializer(const serializer &) = delete;

            /**
             * @brief Deleted move constructor
             */
            serializer(serializer &&) = delete;

            /**
             * @brief Deleted copy assignment operator
             */
            serializer &operator=(const serializer &) = delete;

            /**
             * @brief Delete move assignment operator
             */
            serializer &operator=(serializer &&) = delete;
        };

        /** @} group output */

        //-----------------------------------------------------------------------------
        // [SECTION] Details : Functions
        //-----------------------------------------------------------------------------

        const char *string(token_t type);
        const char *string(error_t type);
        const char *string(event_t type);
        const char *string(value_t type);
        const char *string(break_t type);

        const char *string(mark type);
        const char *string(event type);
        const char *string(token type);

#ifndef MYTOML_NO_STL

        /**
         * @brief Write the token type string into stream.
         * @param[in] ostream An output stream object.
         * @param[in] type An token type.
         * @return Reference to the output stream object `ostream`.
         */
        std::ostream &operator<<(std::ostream &ostream, const token_t &type);

        /**
         * @brief Write the error type string into stream.
         * @param[in] ostream An output stream object.
         * @param[in] type A error type.
         * @return Reference to the output stream object `ostream`.
         */
        std::ostream &operator<<(std::ostream &ostream, const error_t &type);

        /**
         * @brief Write the event type string into stream.
         * @param[in] ostream An output stream object.
         * @param[in] type A event type.
         * @return Reference to the output stream object `ostream`.
         */
        std::ostream &operator<<(std::ostream &ostream, const event_t &type);

        /**
         * @brief Write the value type string into stream.
         * @param[in] ostream An output stream object.
         * @param[in] type A value type.
         * @return Reference to the output stream object `ostream`.
         */
        std::ostream &operator<<(std::ostream &ostream, const value_t &type);

        /**
         * @brief Write the break type string into stream.
         * @param[in] ostream An output stream object.
         * @param[in] type A break type.
         * @return Reference to the output stream object `ostream`.
         */
        std::ostream &operator<<(std::ostream &ostream, const break_t &type);

        /**
         * @brief Write the token type string into stream.
         * @param[in] ostream An output stream object.
         * @param[in] mark A mark object.
         * @return Reference to the output stream object `ostream`.
         */
        std::ostream &operator<<(std::ostream &ostream, const mark &type);

        /**
         * @brief Write the error type string into stream.
         * @param[in] ostream An output stream object.
         * @param[in] event An event object.
         * @return Reference to the output stream object `ostream`.
         */
        std::ostream &operator<<(std::ostream &ostream, const event &type);

        /**
         * @brief Write the event type string into stream.
         * @param[in] ostream An output stream object.
         * @param[in] token A event object.
         * @return Reference to the output stream object `ostream`.
         */
        std::ostream &operator<<(std::ostream &ostream, const token &type);

#endif // MYTOML_NO_STL

    }; // namespace detail

    MYTOML_VERSION_NAMESPACE_END

} // namespace mytoml

#pragma endregion // Detail

#pragma region Mytoml

/**
 * @namespace mytoml
 * @brief The API namespace mytoml::
 */
namespace mytoml
{
    MYTOML_VERSION_NAMESPACE_BEGIN

    //-----------------------------------------------------------------------------
    // [SECTION] Mytoml : Flags & Enumerations
    //-----------------------------------------------------------------------------

    /**
     * @defgroup enum Flags & Enumerations
     * @brief Core enum types and flags.
     * @{
     */

    enum class encoding : uint8_t
    {
        unspecified, /** Let the parser choose the encoding. */
        utf8,        /** The UTF-8 encoding. */
        utf16,       /** The UTF-16-LE encoding with native endianness. */
        utf16le,     /** The UTF-16-LE encoding with BOM. */
        utf16be,     /** The UTF-16-BE encoding with BOM. */
        utf32,       /** The UTF-32 encoding with native endianness. */
        utf32le,     /** The UTF-32-LE encoding with BOM. */
        utf32be,     /** The UTF-32-BE encoding with BOM. */
    };

    enum class node_t : uint8_t
    {
        unknown,
        table,
        array,
        string,
        integer,
        floating,
        boolean
    };

    /** @} */

    //-----------------------------------------------------------------------------
    // [SECTION] Mytoml : Data Structures
    //-----------------------------------------------------------------------------

    /**
     * @defgroup struct Data Structures
     * @brief Core types and data structures.
     * @{
     */

    /**
     * @class mytoml::version
     * @brief version numbering scheme @c https://semver.org/
     * @note Its members can be accessed directly (there are no accessors like
     * `SetMajor()`, `GetMajor()`). Usage example:
     */
    class version
    {
    public:
        /**
         * @brief Default constructor
         *
         * @note Creates a `version(0, 0, 0)`.
         */
        version() = default;

        /**
         * @brief Construct the version.
         *
         * @param major The major version number.
         * @param minor The minor version number.
         * @param patch The patch version number.
         *
         */
        version(int major, int minor, int patch);

    public:
        // NOLINTBEGIN(misc-non-private-member-variables-in-classes)

        int major{}; /** The major version number. */
        int minor{}; /** The minor version number. */
        int patch{}; /** The patch version number. */

        // NOLINTEND(misc-non-private-member-variables-in-classes)
    };

    /**
     * @name Version comparison operators
     * @{
     */
    bool operator<(const version &lhs, const version &rhs) noexcept;
    bool operator>(const version &lhs, const version &rhs) noexcept;
    bool operator==(const version &lhs, const version &rhs) noexcept;
    bool operator!=(const version &lhs, const version &rhs) noexcept;
    bool operator<=(const version &lhs, const version &rhs) noexcept;
    bool operator>=(const version &lhs, const version &rhs) noexcept;
    /** @} */

#ifndef MYTOML_NO_STL

    /**
     * @brief Write the version object string into stream.
     *
     * @param[in] ostream An output stream object.
     * @param[in] version A version object.
     *
     * @return Reference to the output stream object `ostream`.
     */
    std::ostream &operator<<(std::ostream &ostream, const version &version);

#endif // MYTOML_NO_STL

    enum class month_t : uint8_t
    {
        Jan = 0,
        Feb = 1,
        Mar = 2,
        Apr = 3,
        May = 4,
        Jun = 5,
        Jul = 6,
        Aug = 7,
        Sep = 8,
        Oct = 9,
        Nov = 10,
        Dec = 11
    };

    struct local_date
    {
        uint16_t year{0}; // A.D. (like, 2018)
        uint8_t month{0}; // [0, 11]
        uint8_t day{0};   // [1, 31]
    };

    struct local_time
    {
        uint8_t hour{0};         // [0, 23]
        uint8_t minute{0};       // [0, 59]
        uint8_t second{0};       // [0, 60]
        uint16_t millisecond{0}; // [0, 999]
        uint16_t microsecond{0}; // [0, 999]
        uint16_t nanosecond{0};  // [0, 999]
    };

    struct time_offset
    {
        uint8_t hour{0};   // [-12, 12]
        uint8_t minute{0}; // [-59, 59]
    };

    struct local_datetime
    {
        local_date date{};
        local_time time{};
    };

    struct offset_datetime
    {
        local_date date{};
        local_time time{};
        time_offset offset{};
    };

    /**
     * @class mytoml::toml
     * @brief Lightweight value holder.
     *
     * This is a placeholder for the public value type. Public API and
     * value accessors will be provided in the implementation file. The
     * internal representation is intentionally minimal in this header stub.
     */
    class toml
    {

        friend ::mytoml::detail::serializer;
        friend ::mytoml::detail::deserializer;
        friend class ::mytoml::detail::parser;
        friend class ::mytoml::detail::exception;

        using initializer_list_t = std::initializer_list<toml>;

    public:
        using value_t = detail::value_t; /** A node value types. */

        using value_type = toml;                    /** A toml object. */
        using pointer = value_type *;               /** A pointer to a toml object. */
        using reference = value_type &;             /** A reference to a toml object. */
        using const_pointer = const value_type *;   /** A constant pointer to a toml object. */
        using const_reference = const value_type &; /** A constant reference to a toml object. */
        using difference_type = std::ptrdiff_t;     /** A differences between toml iterators. */
        using size_type = size_t;                   /** A type to represent toml sizes. */

        /**
         * @name types
         * Type aliases for convenience.
         * {@
         */

        using node_t = detail::value_t;              /** A type for a toml value. */
        using table_t = std::map<std::string, toml>; /** A type for a toml object value. */
        using array_t = std::vector<toml>;           /** A type for a toml array value. */
        using string_t = std::string;                /** A type for a toml string value. */
        using number_t = double;                     /** A type for a toml number value. */
        using integer_t = int64_t;                   /** A type for a toml interger value. */
        using boolean_t = bool;                      /** A type for a toml boolean value. */
        using null_t = std::nullptr_t;               /** A type for a toml null value. */

        using key_type = std::string;
        using comment_type = std::string;
        using boolean_type = bool;
        using integer_type = int64_t;
        using floating_type = double;
        using string_type = std::string;
        using local_time_type = local_time;
        using local_date_type = local_date;
        using local_datetime_type = local_datetime;
        using offset_datetime_type = offset_datetime;
        using array_type = std::vector<toml>;
        using table_type = std::map<std::string, toml>;

        /* @} types */

        /**
         * @name containers
         * {@
         */

        using iterator = mytoml::detail::iterator<toml>;                                 /** An iterator type for toml. */
        using const_iterator = mytoml::detail::iterator<const toml>;                     /** A const iterator type for toml. */
        using reverse_iterator = mytoml::detail::reverse_iterator<iterator>;             /** A reverse iterator type for toml. */
        using const_reverse_iterator = mytoml::detail::reverse_iterator<const_iterator>; /** A const reverse iterator type for toml. */

        /* @} containers */

        /**
         * @name exceptions
         * Classes to implement user-defined exceptions.
         * {@
         */

        using exception = detail::exception;
        using parse_error = detail::parse_error;
        using encoding_error = detail::encoding_error;

        /* @} exceptions */

    public:
        //========== Constructors ==========

        // toml();
        // toml(table_t value);
        // toml(array_t value);
        // toml(std::string value);
        // toml(const char *value);
        // toml(int64_t value);
        // toml(double value);
        // toml(bool value);

        /**
         * @brief Construct a TOML null value.
         */
        toml() noexcept;

        /**
         * @brief Construct TOML null explicitly.
         */
        toml(std::nullptr_t) noexcept;

        /**
         * @brief Construct a TOML boolean value.
         */
        toml(bool value) noexcept;

        /**
         * @brief Construct a TOML integer from int.
         */
        toml(int value) noexcept;

        /**
         * @brief Construct an empty value of the requested TOML type.
         */
        toml(value_t value) noexcept;

        /**
         * @brief Construct a TOML integer value.
         */
        toml(integer_t value) noexcept;

        /**
         * @brief Construct a TOML floating-point value.
         */
        toml(number_t value) noexcept;

        /**
         * @brief Construct a TOML string from a C string.
         */
        toml(const char *value);

        /**
         * @brief Construct a TOML string by copy.
         */
        toml(const string_t &value);

        /**
         * @brief Construct a TOML string by move.
         */
        toml(const string_t &&value);

        /**
         * @brief Construct a TOML array by copy.
         */
        toml(const array_t &value);

        /**
         * @brief Construct a TOML array by move.
         */
        toml(const array_t &&value);

        /**
         * @brief Construct a TOML object by copy.
         */
        toml(const table_t &value);

        /**
         * @brief Construct a TOML object by move.
         */
        toml(const table_t &&value);

        /**
         * @brief Copy constructor.
         */
        toml(const toml &other);

        /**
         * @brief Move constructor.
         */
        toml(toml &&other) noexcept;

        /**
         * @brief Construct from initializer-list.
         * @param init Values to store.
         * @param type_deduction Detect table-vs-array form from @p init.
         * @param manual_type Explicitly force array or table type.
         */
        toml(initializer_list_t init,
             bool type_deduction = true,
             value_t manual_type = value_t::array);

        /**
         * @brief Create an array value from initializer-list.
         */
        static toml array(initializer_list_t init = {});

        /**
         * @brief Create a table value from initializer-list.
         */
        static toml table(initializer_list_t init = {});

        //========== Assignment ==========

        /**
         * @brief Copy assignment.
         */
        reference operator=(const toml &other);

        /**
         * @brief Move assignment.
         */
        reference operator=(toml &&other) noexcept;

        /**
         * @brief Assign a null value.
         */
        reference operator=(std::nullptr_t) noexcept;

        /**
         * @brief Assign a boolean value.
         */
        reference operator=(bool value) noexcept;

        /**
         * @brief Assign a signed integer value.
         */
        reference operator=(int value) noexcept;

        /**
         * @brief Assign a 64-bit integer value.
         */
        reference operator=(integer_t value) noexcept;

        /**
         * @brief Assign a floating-point numeric value.
         */
        reference operator=(number_t value) noexcept;

        /**
         * @brief Assign a UTF-8 string value.
         */
        reference operator=(const string_t &value);

        /**
         * @brief Assign a C-string value (nullptr becomes empty string).
         */
        reference operator=(const char *value);

        /**
         * @brief Assign an array value.
         */
        reference operator=(const array_t &value);

        /**
         * @brief Assign an object value.
         */
        reference operator=(const table_t &value);

        /**
         * @brief Assign from initializer-list with type deduction.
         */
        reference operator=(initializer_list_t init);

        //========== Objects ==========

        /**
         * @brief Access table member by key with bounds checking.
         */
        reference at(const std::string &key);

        /**
         * @brief Access table member by key with bounds checking (const).
         */
        const_reference at(const std::string &key) const;

        /**
         * @brief Access/create table member by key.
         */
        reference operator[](const std::string &key);

        /**
         * @brief Access table member by key; returns null toml if missing.
         */
        toml operator[](const std::string &key) const;

        /**
         * @brief Access/create table member by C-string key.
         */
        reference operator[](const char *key);

        /**
         * @brief Access table member by C-string key; returns null toml if missing.
         */
        toml operator[](const char *key) const;

        /**
         * @brief Check whether an object contains @p key.
         */
        MYTOML_NODISCARD bool contains(const std::string &key) const noexcept;

        /**
         * @brief Count occurrences of @p key (0 or 1 for object).
         */
        MYTOML_NODISCARD size_t count(const std::string &key) const noexcept;

        /**
         * @brief Erase table member by key; returns removed count.
         */
        size_t erase(const std::string &key) noexcept;

        //========== Arrays ==========

        /**
         * @brief Access array element with bounds checking.
         */
        reference at(size_t index);

        /**
         * @brief Access array element with bounds checking (const).
         */
        const_reference at(size_t index) const;

        /**
         * @brief Access array element by index and grow with null values if needed.
         */
        reference operator[](size_t index);

        /**
         * @brief Access array element by index (const).
         */
        const_reference operator[](size_t index) const;

        /**
         * @brief Access first array element.
         */
        reference front();

        /**
         * @brief Access first array element (const).
         */
        const_reference front() const;

        /**
         * @brief Access last array element.
         */
        reference back();

        /**
         * @brief Access last array element (const).
         */
        const_reference back() const;

        /**
         * @brief Append element to array.
         */
        void push_back(const toml &value);

        /**
         * @brief Append movable element to array.
         */
        void push_back(toml &&value);

        /**
         * @brief Insert element at the front of an array.
         */
        void push_front(const toml &value);

        /**
         * @brief Insert value at iterator position.
         */
        iterator insert(const const_iterator &pos, const toml &value);

        /**
         * @brief Insert movable value at iterator position.
         */
        iterator insert(const const_iterator &pos, toml &&value);

        /**
         * @brief Erase one element at iterator position.
         */
        iterator erase(const_iterator pos);

        /**
         * @brief Erase range [first,last).
         */
        iterator erase(const_iterator first, const_iterator last);

        //========== Iteration ==========

        /**
         * @brief Begin iterator over table members.
         */
        iterator begin();

        /**
         * @brief Begin const iterator over table members.
         */
        MYTOML_NODISCARD const_iterator begin() const;

        /**
         * @brief Constant begin iterator over table members.
         */
        MYTOML_NODISCARD const_iterator cbegin() const;

        /**
         * @brief End iterator over table members.
         */
        iterator end();

        /**
         * @brief End const iterator over table members.
         */
        MYTOML_NODISCARD const_iterator end() const;

        /**
         * @brief Constant end iterator over table members.
         */
        MYTOML_NODISCARD const_iterator cend() const;

        /**
         * @brief Reverse begin iterator over table members.
         */
        reverse_iterator rbegin();

        /**
         * @brief Reverse begin const iterator over table members.
         */
        MYTOML_NODISCARD const_reverse_iterator rbegin() const;

        MYTOML_NODISCARD const_reverse_iterator crbegin() const;

        /**
         * @brief Reverse end iterator over table members.
         */
        reverse_iterator rend();

        /**
         * @brief Reverse end const iterator over table members.
         */
        MYTOML_NODISCARD const_reverse_iterator rend() const;

        MYTOML_NODISCARD const_reverse_iterator crend() const;

        //========== Comparison ==========

        /** @brief Equality comparison. */
        bool operator==(const toml &other) const noexcept;

        /** @brief Inequality comparison. */
        bool operator!=(const toml &other) const noexcept;

        /** @brief Strict weak ordering comparison. */
        bool operator<(const toml &other) const noexcept;

        /** @brief Less-than-or-equal comparison. */
        bool operator<=(const toml &other) const noexcept;

        /** @brief Greater-than comparison. */
        bool operator>(const toml &other) const noexcept;

        /** @brief Greater-than-or-equal comparison. */
        bool operator>=(const toml &other) const noexcept;

        /**
         * @name serialization
         * {@
         */

        /**
         * @brief Serialize to TOML text (compact or pretty with indent).
         */
        MYTOML_NODISCARD string_t dump(int indent = -1) const;

        void dump(FILE *file);

        void dump(const char *str);

        void dump(const string_t &str);

#ifndef MYTOML_NO_STL
        void dump(std::ostream &stream);
#endif // MYTOML_NO_STL

        void dump(detail::oadapter &adapter);

#ifndef MYTOML_NO_IO
        friend std::ostream &operator<<(std::ostream &o, const toml &j);
#endif // MYTOML_NO_IO

        /* @} serialization */

        /**
         * @name deserialization
         * @brief Parsing
         * {@
         */

        /**
         * @brief Parse TOML from FILE.
         * @param file The TOML FILE* to parse.
         * @return Parsed toml object.
         * @throws parse_error on invalid TOML.
         */
        static toml parse(FILE *file);

        /**
         * @brief Parse TOML from C string.
         * @param str The TOML string to parse.
         * @return Parsed toml object.
         * @throws parse_error on invalid TOML.
         */
        static toml parse(const char *str);

        /**
         * @brief Parse TOML from string.
         * @param str The TOML string to parse.
         * @return Parsed toml object.
         * @throws parse_error on invalid TOML.
         */
        static toml parse(const string_t &str);

#ifndef MYTOML_NO_STL
        /**
         * @brief Parse TOML from an input stream.
         * @param stream The TOML input stream to parse.
         * @return Parsed toml object.
         * @throws parse_error on invalid TOML.
         */
        static toml parse(std::istream &stream);
#endif // MYTOML_NO_STL

        /**
         * @brief Parse TOML from an input adapter.
         * @param adapter The TOML input adapter to parse.
         * @return Parsed toml object.
         * @throws parse_error on invalid TOML.
         */
        static toml parse(detail::iadapter &adapter);

#ifndef MYTOML_NO_IO
        friend std::istream &operator>>(std::istream &i, toml &j);
#endif // MYTOML_NO_IO

        /* @} deserialization */

        MYTOML_NODISCARD node_t type() const noexcept;
        MYTOML_NODISCARD bool is_table() const noexcept;
        MYTOML_NODISCARD bool is_array() const noexcept;
        MYTOML_NODISCARD bool is_string() const noexcept;
        MYTOML_NODISCARD bool is_integer() const noexcept;
        MYTOML_NODISCARD bool is_boolean() const noexcept;
        MYTOML_NODISCARD bool is_floating() const noexcept;
        MYTOML_NODISCARD bool is_local_time() const noexcept;
        MYTOML_NODISCARD bool is_local_date() const noexcept;
        MYTOML_NODISCARD bool is_local_datetime() const noexcept;
        MYTOML_NODISCARD bool is_offset_datetime() const noexcept;

        MYTOML_NODISCARD bool is_float() const noexcept { return is_floating(); }
        MYTOML_NODISCARD bool is_null() const noexcept { return type() == value_t::null; }

        MYTOML_NODISCARD integer_type &as_int() { return as_integer(); }
        MYTOML_NODISCARD const integer_type &as_int() const { return as_integer(); }
        MYTOML_NODISCARD floating_type &as_float() { return as_floating(); }
        MYTOML_NODISCARD const floating_type &as_float() const { return as_floating(); }
        MYTOML_NODISCARD boolean_type &as_bool() { return as_boolean(); }
        MYTOML_NODISCARD const boolean_type &as_bool() const { return as_boolean(); }

        MYTOML_NODISCARD string_t serialize(int indent = -1) const { return dump(indent); }

        table_type &as_table();
        array_type &as_array();
        string_type &as_string();
        integer_type &as_integer();
        boolean_type &as_boolean();
        floating_type &as_floating();
        local_time_type &as_local_time();
        local_date_type &as_local_date();
        local_datetime_type &as_local_datetime();
        offset_datetime_type &as_offset_datetime();

        const table_type &as_table() const;
        const array_type &as_array() const;
        const string_type &as_string() const;
        const integer_type &as_integer() const;
        const boolean_type &as_boolean() const;
        const floating_type &as_floating() const;
        const local_time_type &as_local_time() const;
        const local_date_type &as_local_date() const;
        const local_datetime_type &as_local_datetime() const;
        const offset_datetime_type &as_offset_datetime() const;

        //========== Size and Capacity ==========

        /**
         * @brief Return number of elements for array/table, otherwise 0.
         */
        MYTOML_NODISCARD size_type size() const noexcept;

        /**
         * @brief Return whether container size is zero.
         */
        MYTOML_NODISCARD bool empty() const noexcept;

        /**
         * @brief Clear table/array contents.
         */
        void clear() noexcept;

        /**
         * @brief Default destructor.
         */
        ~toml() noexcept;

    private:
        void ensure_table();

        void ensure_array();

        MYTOML_NODISCARD const table_t &get_table() const;

        MYTOML_NODISCARD const array_t &get_array() const;

        table_t &get_table();

        array_t &get_array();

    private:
        std::variant<
            null_t,
            boolean_t,
            integer_t,
            number_t,
            string_t,
            std::shared_ptr<array_t>,
            std::shared_ptr<table_t>>
            m_value; /** Active TOML storage variant. */
    };

    /** @} */

    //-----------------------------------------------------------------------------
    // [SECTION] Mytoml : Functions
    //-----------------------------------------------------------------------------

    const char *string(encoding type);

    const char *string(node_t type);

#ifndef MYTOML_NO_STL

    /**
     * @brief Write the encoding type string into stream.
     * @param[in] ostream An output stream object.
     * @param[in] type An encoding type.
     * @return Reference to the output stream object `ostream`.
     */
    std::ostream &operator<<(std::ostream &ostream, const encoding &type);

    /**
     * @brief Write the node type string into stream.
     * @param[in] ostream An output stream object.
     * @param[in] type A node type.
     * @return Reference to the output stream object `ostream`.
     */
    std::ostream &operator<<(std::ostream &ostream, const node_t &type);

    /**
     * @brief A wrapper for the serialization feature.
     * @param[in] stream An output stream object.
     * @param[in] node A toml object.
     * @return Reference to the output stream object `stream`.
     */
    std::ostream &operator<<(std::ostream &stream, const toml &node);

    /**
     * @brief A wrapper for the deserialization feature.
     * @param[in] stream An input stream object.
     * @param[in] node A toml object.
     * @return Reference to the input stream object `stream`.
     */
    std::istream &operator>>(std::istream &stream, toml &node);

#endif // MYTOML_NO_STL

    MYTOML_VERSION_NAMESPACE_END

} // namespace mytoml

#pragma endregion // Mytoml

#pragma region Literal

/**
 * @namespace mytoml
 * @brief The API namespace mytoml::
 */
namespace mytoml
{
    MYTOML_VERSION_NAMESPACE_BEGIN

    /**
     * @namespace literals
     * @brief The literals namespace mytoml::literals::
     */
    namespace literals
    {

        inline namespace toml_literals
        {
            //-----------------------------------------------------------------------------
            // [SECTION] Literals : Functions
            //-----------------------------------------------------------------------------

            // Whitespace before the literal operator is deprecated in C++23 or later but required in C++11.
            MYTOML_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wdeprecated")

            /**
             * @brief Deserializes a `char` array into a `toml` object.
             *
             * @param string An input `char` array.
             * @param size The size of `string`.
             *
             * @return The resulting `toml` object deserialized from `string`.
             */
            MYTOML_INLINE toml MYTOML_QUOTE_OPERATOR(const char *string, size_t size);

#if MYTOML_HAS_CHAR8_T

            /**
             * @brief Deserializes a `char8_t` array into a `toml` object.
             *
             * @param string An input `char8_t` array.
             * @param size The size of `string`.
             *
             * @return The resulting `toml` object deserialized from `string`.
             */
            MYTOML_INLINE toml MYTOML_QUOTE_OPERATOR(const char8_t *string, size_t size);

#endif // MYTOML_HAS_CHAR8_T

            /**
             * @brief Deserializes a `char16_t` array into a `toml` object.
             *
             * @param string An input `char16_t` array.
             * @param size The size of `string`.
             *
             * @return The resulting `toml` object deserialized from `string`.
             */
            MYTOML_INLINE toml MYTOML_QUOTE_OPERATOR(const char16_t *string, size_t size);

            /**
             * @brief Deserializes a `char32_t` array into a `toml` object.
             *
             * @param string An input `char32_t` array.
             * @param size The size of `string`.
             *
             * @return The resulting `toml` object deserialized from `string`.
             */
            MYTOML_INLINE toml MYTOML_QUOTE_OPERATOR(const char32_t *string, size_t size);

            MYTOML_CLANG_SUPPRESS_WARNING_POP

        } // namespace toml_literals
    } // namespace literals

    MYTOML_VERSION_NAMESPACE_END

}; // namespace mytoml
#pragma endregion // Literal

//-----------------------------------------------------------------------------

// clang-format off

#if MYTOML_COMPILER_IS_GCC
    #pragma GCC diagnostic pop
#endif // MYTOML_COMPILER_IS_GCC

#if MYTOML_COMPILER_IS_MSVC
    #pragma warning(pop)
#endif // MYTOML_COMPILER_IS_MSVC

#if MYTOML_COMPILER_IS_CLANG
    #pragma clang diagnostic pop
#endif // MYTOML_COMPILER_IS_CLANG

// clang-format on

#endif // DJOEZEKE_MYTOML_HPP

// NOLINTEND

/**
 * HISTORY: version History
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