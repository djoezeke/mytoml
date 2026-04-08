# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)

# TODO: Enable Custom Compiler Warning Flags
# TODO: Enable Address Sanitizers.
# TODO: Run Clang-tidy and Clang-format.
# TODO: Add C++ Modules Support (optional).
# TODO: Add debug view definition file for msvc (natvis)
# TODO: Execute test suite with Valgrind.

# define a macro that helps defining an option
macro(mytoml_set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

#--------------------------------------------------------------------
# Project and version.
#--------------------------------------------------------------------

# set a default build type if none was provided
# this has to be done before the project() instruction!
mytoml_set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")

set (MYTOML_VERSION_MAJOR 0)
set (MYTOML_VERSION_MINOR 1)
set (MYTOML_VERSION_PATCH 1)
set (MYTOML_VERSION_STRING "0.1.0")

project(Mytoml
        VERSION ${MYTOML_VERSION_STRING} 
        DESCRIPTION "Toml parser for C/C++"
        HOMEPAGE_URL "https://github.com/djoezeke/mytoml"
        LANGUAGES  C CXX
)

#--------------------------------------------------------------------
# Preparations depending on the specified build options  
#--------------------------------------------------------------------

# Check if this project is the main project
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MYTOML_IS_MAIN ON)
else()
  set(MYTOML_IS_MAIN OFF)
endif()

# Add UTF-8 encoding support for MSVC compiler.
# This ensures that the MSVC compiler interprets source files as UTF-8 encoded,
# which is useful for projects containing non-ASCII characters in source files.
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# we use the paths from the cmake GNUInstallDirs library as defaults
# you can override these if you like
# https://cmake.org/cmake/help/latest/library/GNUInstallDirs.html
include(GNUInstallDirs)

include(CMakeDependentOption)

#--------------------------------------------------------------------
# Configure variables for the library   
#--------------------------------------------------------------------

mytoml_set_option(MYTOML_ENABLE_WARNING ${MYTOML_IS_MAIN} BOOL "ON to enable warning messages.")

mytoml_set_option(MYTOML_BUILD_CMYTOML ON BOOL "ON to build MYTOMLC")

# an option for choosing the build type (shared or static)
mytoml_set_option(MYTOML_BUILD_SHARED ON BOOL "ON to build MYTOML as shared libraries, OFF to build it as static libraries")

# an option for linking to mytoml either statically or dynamically
set(LINK_STATICALLY_DEFAULT ON)
mytoml_set_option(CMYTOML_LINK_MYTOML_STATICALLY ${LINK_STATICALLY_DEFAULT} BOOL "ON to link to a static version of MYTOML, OFF to link dynamically")

mytoml_set_option(MYTOML_INSTALL ${MYTOML_IS_MAIN} BOOL "ON to Generate installation target" )
mytoml_set_option(MYTOML_ENABLE_INSTALL ${MYTOML_INSTALL} BOOL "Enable installation.")
mytoml_set_option(MYTOML_ENABLE_PACKING ${MYTOML_INSTALL} BOOL "Enable packing with CPack.")

cmake_dependent_option(MYTOML_INSTALL_DOCS "Install docs for Mytoml" ${MYTOML_IS_MAIN} "MYTOML_INSTALL" ON)
cmake_dependent_option(MYTOML_INSTALL_TESTS "Install test-cases" ${MYTOML_IS_MAIN} "MYTOML_INSTALL" ON)
cmake_dependent_option(MYTOML_INSTALL_CPACK "Create binary Mytoml archive using CPack" ${MYTOML_IS_MAIN} "MYTOML_INSTALL" ON)

# option(MYTOML_BUILD_SHARED "Build shared library" ${MYTOML_IS_MAIN})
# option(MYTOML_BUILD_DEBUG "Build debug version of library" ${MYTOML_IS_MAIN})
# option(MYTOML_ENABLE_DOXYGEN "Build documentation with Doxygen." ${MYTOML_IS_MAIN})

option(MYTOML_BUILD_C11 "Build Mytoml with C11" OFF)
option(MYTOML_BUILD_C17 "Build Mytoml with C17" OFF)
option(MYTOML_BUILD_C23 "Build Mytoml with C23" OFF)

option(MYTOML_BUILD_CXX11 "Build Mytoml with C++11" OFF)
option(MYTOML_BUILD_CXX14 "Build Mytoml with C++14" OFF)
option(MYTOML_BUILD_CXX17 "Build Mytoml with C++17" OFF)
option(MYTOML_BUILD_CXX20 "Build Mytoml with C++20" OFF)
option(MYTOML_BUILD_CXX23 "Build Mytoml with C++23" OFF)

# Set required C standard
if(MYTOML_BUILD_C23)
    set(CMAKE_C_STANDARD 23)
elseif(MYTOML_BUILD_C17)
    set(CMAKE_C_STANDARD 17)
elseif(MYTOML_BUILD_C11)
    set(CMAKE_C_STANDARD 11)
else()
    set(CMAKE_C_STANDARD 17)
endif()
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# Set required C++ standard
if(MYTOML_BUILD_CXX23)
    set(CMAKE_CXX_STANDARD 23)
elseif(MYTOML_BUILD_CXX20)
    set(CMAKE_CXX_STANDARD 20)
elseif(MYTOML_BUILD_CXX17)
    set(CMAKE_CXX_STANDARD 17)
elseif(MYTOML_BUILD_CXX14)
    set(CMAKE_CXX_STANDARD 14)
elseif(MYTOML_BUILD_CXX11)
    set(CMAKE_CXX_STANDARD 11)
else()
    set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(DEFINED BUILD_SHARED_LIBS)
    # When defined, use BUILD_SHARED_LIBS as default
    if(BUILD_SHARED_LIBS)
        set(MYTOML_STATIC OFF)
    else()
        set(MYTOML_STATIC ON)
    endif()
else()
    # Default to just building the static library
    set(MYTOML_STATIC ON)
endif()

# Add compiler warnings
if(MYTOML_ENABLE_WARNING)
    if(MSVC)
        set(MYTOML_WARNINGS /W4 /permissive-)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
        set(MYTOML_WARNINGS -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion)
    endif()
endif()

option(MYTOML_ENABLE_SANITIZERS "Enable sanitizers" OFF)
if(MYTOML_ENABLE_SANITIZERS)
    string(APPEND CMAKE_CXX_FLAGS " -fno-omit-frame-pointer -fno-sanitize-recover=all -fsanitize=undefined")
endif()

option(MYTOML_ENABLE_STDLIB_ASSERTIONS "Enable standard library assertions" OFF)
if(MYTOML_ENABLE_STDLIB_ASSERTIONS)
    # intentionally using `add_compile_definitions` here to propagate defines to subdirectories
    add_compile_definitions(_GLIBCXX_ASSERTIONS=1)                                   # see https://gcc.gnu.org/wiki/LibstdcxxDebugMode

    # Xcode has a property to control this - adding the define ourselves causes multiple definitions
    if(XCODE)
        set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_STANDARD_LIBRARY_HARDENING "Yes (extensive)")
    else()
        add_compile_definitions(_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE) # see https://libcxx.llvm.org/Hardening.html
    endif()
endif()

set(MYTOML_CMAKE_CONFIG_NAME "${PROJECT_NAME}Config")
set(MYTOML_CMAKE_TARGET_NAME "${PROJECT_NAME}Targets")

set(MYTOML_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(MYTOML_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(MYTOML_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(MYTOML_CMAKE_CONFIG_TEMPLATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(MYTOML_CMAKE_CONFIG_TEMPLATE "${MYTOML_CMAKE_CONFIG_TEMPLATE_DIR}/${MYTOML_CMAKE_CONFIG_NAME}.cmake.in")
set(MYTOML_CMAKE_VERSION_CONFIG_TEMPLATE "${MYTOML_CMAKE_CONFIG_TEMPLATE_DIR}/${MYTOML_CMAKE_CONFIG_NAME}Version.cmake.in")
set(MYTOML_CMAKE_PACKAGE_CONFIG_TEMPLATE "${MYTOML_CMAKE_CONFIG_TEMPLATE_DIR}/${PROJECT_NAME}.pc.in")

set(MYTOML_CMAKE_CONFIG_DIR  ${CMAKE_CURRENT_BINARY_DIR})
set(MYTOML_CMAKE_CONFIG_FILE  ${MYTOML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Config.cmake)
set(MYTOML_CMAKE_TARGETS_FILE  ${MYTOML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Targets.cmake)
set(MYTOML_CMAKE_PACKAGE_FILE  ${MYTOML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}.pc)
set(MYTOML_CMAKE_VERSION_CONFIG_FILE  ${MYTOML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake)

#--------------------------------------------------------------------
# Macro to Build libraries
#--------------------------------------------------------------------

include(CMakeParseArguments)

# example: mytoml_make_library(Mytoml
#                       SOURCES mytomlc.cpp ...
#                       DEPENDS Mytoml::MytomlC
#                       [STATIC]) # Always create a static library
macro(mytoml_make_library library)

    # parse the arguments
    cmake_parse_arguments(THIS "" "" "SOURCES;DEPENDS" ${ARGN})
    if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "Extra unparsed arguments when calling mytoml_make_library: ${THIS_UNPARSED_ARGUMENTS}")
    endif()

    # create the target
    string(TOLOWER ${library} target)
    if(MYTOML_STATIC)
        add_library(${target} STATIC ${THIS_SOURCES})
    else()
        add_library(${target} SHARED ${THIS_SOURCES})
    endif()
    add_library(Mytoml::${library} ALIAS ${target})

    # require C11 support
    target_compile_features(${target} PUBLIC c_std_11)

    # enable C++17 support
    target_compile_features(${target} PUBLIC cxx_std_17)

    # define the export symbol of the library
    string(REPLACE "-" "_" NAME_UPPER "${target}")
    string(TOUPPER "${NAME_UPPER}" NAME_UPPER)
    set_target_properties(${target} PROPERTIES DEFINE_SYMBOL ${NAME_UPPER}_EXPORTS)

    # define the export name of the library
    set_target_properties(${target} PROPERTIES EXPORT_NAME ${library})

    # set the target's folder (for IDEs that support it, e.g. Visual Studio)
    set_target_properties(${target} PROPERTIES FOLDER "Mytoml")

    # <project>/include as public include directory
    target_include_directories(${target} 
        PUBLIC $<BUILD_INTERFACE:${MYTOML_INCLUDE_BUILD_DIR}>
        PUBLIC $<INSTALL_INTERFACE:${MYTOML_INCLUDE_INSTALL_DIR}>
    )

    if(THIS_DEPENDS)
        target_link_libraries(${target} PUBLIC ${THIS_DEPENDS})
    endif()

    if(NOT MYTOML_STATIC)
        set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)
        if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
            # include the major version number in Windows shared library names (but not import library names)
            set_target_properties(${target} PROPERTIES SUFFIX "-${PROJECT_VERSION_MAJOR}${CMAKE_SHARED_LIBRARY_SUFFIX}")

            # on Windows/gcc get rid of "lib" prefix for shared libraries,
            # and transform the ".dll.a" suffix into ".a" for import libraries
            if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
                set_target_properties(${target} PROPERTIES PREFIX "")
                set_target_properties(${target} PROPERTIES IMPORT_SUFFIX ".a")
            endif()
        endif()
    else()
        set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)
        set_target_properties(${target} PROPERTIES RELEASE_POSTFIX "")
        set_target_properties(${target} PROPERTIES MINSIZEREL_POSTFIX "")
        set_target_properties(${target} PROPERTIES RELWITHDEBINFO_POSTFIX "")
    endif()

    # set the version and soversion of the target (for compatible systems -- mostly Linuxes)
    set_target_properties(${target} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
    set_target_properties(${target} PROPERTIES VERSION ${PROJECT_VERSION})

    if(MYTOML_WARNINGS)
        target_compile_options(${target} PRIVATE ${MYTOML_WARNINGS})
    endif()

    if(NOT MYTOML_STATIC)
        # define MYTOML_BUILD_SHARED if the build type is set to 'shared'
        target_compile_definitions(${target} PRIVATE MYTOML_BUILD_SHARED)
        set_target_properties(${target} PROPERTIES DEFINE_SYMBOL MYTOML_BUILD_SHARED)
    else()
        # define MYTOML_BUILD_STATIC if the build type is set to 'static'
        target_compile_definitions(${target} PRIVATE MYTOML_BUILD_STATIC)
        set_target_properties(${target} PROPERTIES DEFINE_SYMBOL MYTOML_BUILD_STATIC)
    endif()

    # Enable support for UTF-8 characters in source code
    if(MSVC)
        target_compile_options(${target} PRIVATE /utf-8)
    endif()
endmacro()

#--------------------------------------------------------------------
# Build library targets
#--------------------------------------------------------------------

# If both shared and static libs are build at once with MSVC it generates a
# shared library consisting of
#
# * a DLL (that contains the actual code) with filename suffix `.dll`,
# * a correspoding import library (against which users should link) with
#   filename suffix `.lib`.
#
# and a static library with filename suffix `.lib`
#
# As both, the shared import library and the static library share the same
# basename and suffix one would overwrite the other.
#
# To solve this issue we set the PREFIX for *static* libs explicitely so MSVC
# will build:
#
# * shared: mytoml.dll
# * import: mytoml.lib
# * static: libmytoml.lib
#
# and this for debug:
#
# * shared: mytoml-d.dll
# * import: mytoml-d.lib
# * static: libmytoml-d.lib
#
# As all other platforms already set `PREFIX` for all lib types they remain
# unchanged. Therefore no `if(MSVC)` check is needed here.

if(NOT (MYTOML_STATIC OR BUILD_SHARED_LIBS))
    message(FATAL_ERROR "MYTOML_STATIC and BUILD_SHARED_LIBS cannot both be disabled")
endif()

# define the mytoml target
mytoml_make_library(Mytoml
                    SOURCES src/mytoml.cpp
                    )

set(MYTOML_EXPORT_TARGETS mytoml)

# define the mytomlc target
if(MYTOML_BUILD_CMYTOML)
    mytoml_make_library(MytomlC
                        SOURCES src/mytomlc.cpp
                        DEPENDS Mytoml::Mytoml
                        )
    list(APPEND MYTOML_EXPORT_TARGETS mytomlc)
endif()

#--------------------------------------------------------------------
# Configurations
#--------------------------------------------------------------------

# Default to build type "Release" unless tests are being built
if(NOT CMAKE_BUILD_TYPE)
	if (NOT MYTOML_BUILD_TESTS)
		message(STATUS "No build type selected, default to Release")
		set(CMAKE_BUILD_TYPE "Release")
	else()
		message(STATUS "No build type selected but tests are being built, default to Debug")
		set(CMAKE_BUILD_TYPE "Debug")
	endif()
endif()

# Output directories for a build tree
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

#--------------------------------------------------------------------
# Add Project subdirectories
#--------------------------------------------------------------------

# an option for building the API documentation
mytoml_set_option(MYTOML_BUILD_DOCS OFF BOOL "ON to generate the API documentation, OFF to ignore it")

# Build the documentation   
if(MYTOML_BUILD_DOCS)
    add_subdirectory(docs)
endif()

# an option for building the examples
mytoml_set_option(MYTOML_BUILD_EXAMPLES OFF BOOL "ON to build the CMYTOML examples, OFF to ignore them")

# Build the example apps   
if(MYTOML_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# an option for building the tests
mytoml_set_option(MYTOML_BUILD_TESTS OFF BOOL "ON to build the CMYTOML test suite, OFF to ignore it")

# Build the test apps   
if(MYTOML_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)

    # Set the unit test app project as the Visual Studio startup project
    # if the target compiler is some version of Microsoft Visual C++ and
    # if this project is the main project.
    if(MSVC AND CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MytomlUnitTest)
    endif()
endif()

#--------------------------------------------------------------------
# Configure package.
#--------------------------------------------------------------------

include(CMakePackageConfigHelpers)

configure_file(
    ${MYTOML_CMAKE_CONFIG_TEMPLATE}
    ${MYTOML_CMAKE_CONFIG_FILE}
    @ONLY
)

configure_file(
    ${MYTOML_CMAKE_VERSION_CONFIG_TEMPLATE}
    ${MYTOML_CMAKE_VERSION_CONFIG_FILE}
    @ONLY
)

configure_file(
    "${MYTOML_CMAKE_PACKAGE_CONFIG_TEMPLATE}"
    "${MYTOML_CMAKE_PACKAGE_FILE}"
    @ONLY
)

#--------------------------------------------------------------------
# Install package.
#--------------------------------------------------------------------

if(MYTOML_ENABLE_INSTALL)

    install(
        TARGETS ${MYTOML_EXPORT_TARGETS}
        EXPORT ${MYTOML_CMAKE_TARGET_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${MYTOML_INCLUDE_INSTALL_DIR}
    )

    install(
        DIRECTORY ${MYTOML_INCLUDE_BUILD_DIR}
        DESTINATION ${MYTOML_INCLUDE_INSTALL_DIR}
    )

    install(
        EXPORT ${MYTOML_CMAKE_TARGET_NAME}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${MYTOML_CMAKE_TARGET_NAME}.cmake
        DESTINATION ${MYTOML_CONFIG_INSTALL_DIR}
    )

    install(FILES 
        ${MYTOML_CMAKE_CONFIG_FILE} 
        ${MYTOML_CMAKE_VERSION_CONFIG_FILE}
        DESTINATION ${MYTOML_CONFIG_INSTALL_DIR}
    )

    if(UNIX)
        install(
            FILES "${MYTOML_CMAKE_PACKAGE_FILE}"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )
    endif()

endif()

if(MYTOML_INSTALL AND MYTOML_INSTALL_DOCS)
    install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})
    install(FILES README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})
endif()

#--------------------------------------------------------------------
# Package with CPack.
#--------------------------------------------------------------------

if(MYTOML_ENABLE_PACKINGK)

    if(WIN32 AND NOT CYGWIN)
        find_program(ARGPARSER_MAKENSIS_EXECUTABLE makensis)
        if(ARGPARSER_MAKENSIS_EXECUTABLE)
            set(CPACK_GENERATOR NSIS ZIP)
        else()
            message(STATUS "NSIS not found: falling back to ZIP package generator")
            set(CPACK_GENERATOR ZIP)
        endif()
    endif()

    if(APPLE)
        set(CPACK_GENERATOR DragNDrop TGZ)
    endif()

    if (UNIX AND NOT APPLE AND NOT WIN32)
        set(CPACK_GENERATOR DEB TGZ)
    endif ()

    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_PACKAGE_VENDOR "${PROJECT_NAME} developers")
    set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
    set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
    set(CPACK_PACKAGE_MAINTAINER "Sackey Ezekiel Etrue")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

    set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
    set(CPACK_DEBIAN_PACKAGE_DEBUG OFF)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6-dev")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_MAINTAINER}")
    set(CPACK_DEBIAN_PACKAGE_SUGGESTS "cmake, pkg-config, pkg-conf")

    set(CPACK_RPM_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    set(CPACK_RPM_PACKAGE_SUGGESTS "${CPACK_DEBIAN_PACKAGE_SUGGESTS}")

    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_RPM_COMPONENT_INSTALL ON)
    set(CPACK_NSIS_COMPONENT_INSTALL ON)

    include(CPack)

endif()

#--------------------------------------------------------------------
# Clang CI
#--------------------------------------------------------------------

mytoml_set_option(CLANG_FORMAT_EXECUTABLE clang-format STRING "Override clang-format executable, requires version 17")
add_custom_target(format
    COMMAND ${CMAKE_COMMAND}
            -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
            -DPROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}
            -DCLANG_FORMAT_EXECUTABLE=${CLANG_FORMAT_EXECUTABLE}
            -P ${PROJECT_SOURCE_DIR}/cmake/Format.cmake
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR} VERBATIM)

mytoml_set_option(CLANG_TIDY_EXECUTABLE clang-tidy STRING "Override clang-tidy executable, requires minimum version 14")
add_custom_target(tidy
    COMMAND ${CMAKE_COMMAND}
            -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
            -DPROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}
            -DCLANG_TIDY_EXECUTABLE=${CLANG_TIDY_EXECUTABLE}
            -P ${PROJECT_SOURCE_DIR}/cmake/Tidy.cmake
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR} VERBATIM)

    
#--------------------------------------------------------------------
# Uninstall Target
#--------------------------------------------------------------------

if(MYTOML_UNINSTALL)
    if(NOT TARGET uninstall)
        configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

        add_custom_target(uninstall
            COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
    endif()
endif()
