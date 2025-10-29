cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)

#--------------------------------------------------------------------
# Project and version.
#--------------------------------------------------------------------

set (MYTOML_VERSION_MAJOR 0)
set (MYTOML_VERSION_MINOR 1)
set (MYTOML_VERSION_PATCH 1)
set (MYTOML_VERSION_STRING ${MYTOML_VERSION_MAJOR}.${MYTOML_VERSION_MINOR}.${MYTOML_VERSION_PATCH})

project(Mytoml
        VERSION ${MYTOML_VERSION_STRING} 
        DESCRIPTION "Toml parser for C/C++"
        HOMEPAGE_URL "https://github.com/djoezeke/mytoml"
        LANGUAGES  C
)

#--------------------------------------------------------------------
# Preparations depending on the specified build options  
#--------------------------------------------------------------------

# Check if Mytoml is the main project
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MYTOML_IS_TOP_LEVEL ON)
else()
  set(MYTOML_IS_TOP_LEVEL OFF)
endif()

# Set required C/C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# Add UTF-8 encoding support for MSVC compiler.
# This ensures that the MSVC compiler interprets source files as UTF-8 encoded,
# which is useful for projects containing non-ASCII characters in source files.
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

#--------------------------------------------------------------------
# Configure variables for the library   
#--------------------------------------------------------------------

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(MYTOML_LIB_NAME "mytoml" CACHE STRING "Base name of library output name")

set(MYTOML_SOURCE mytoml.c)
set(MYTOML_HEADER mytoml.h)
set(MYTOML_TARGET_NAME  "${MYTOML_LIB_NAME}")

option(MYTOML_BUILD_EXAMPLES "Build the ${PROJECT_NAME} example applications" ${MYTOML_IS_TOP_LEVEL})
option(MYTOML_BUILD_SHARED "Build shared library" ${MYTOML_IS_TOP_LEVEL})
option(MYTOML_BUILD_DEBUG "Build debug version of library" ${MYTOML_IS_TOP_LEVEL})
option(MYTOML_BUILD_TESTS "Build the ${PROJECT_NAME} test programs" ${MYTOML_IS_TOP_LEVEL})
option(MYTOML_INSTALL_DOCS "Enable installation of documentation." ${MYTOML_IS_TOP_LEVEL})

option(MYTOML_ENABLE_INSTALL "Enable installation." ${MYTOML_IS_TOP_LEVEL})
option(MYTOML_ENABLE_DOXYGEN "Build documentation with Doxygen." ${MYTOML_IS_TOP_LEVEL})
option(MYTOML_ENABLE_WARNING "Enable warning messages." ${MYTOML_IS_TOP_LEVEL})
option(MYTOML_ENABLE_PACKING "Enable packing with CPack." ${MYTOML_IS_TOP_LEVEL})


set(MYTOML_CMAKE_CONFIG_NAME "${PROJECT_NAME}Config")
set(MYTOML_CMAKE_TARGET_NAME "${PROJECT_NAME}Target")

set(MYTOML_INCLUDE_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MYTOML_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(MYTOML_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}" CACHE INTERNAL "Install directory path for config files.")

set(MYTOML_CMAKE_CONFIG_TEMPLATE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(MYTOML_CMAKE_CONFIG_TEMPLATE "${MYTOML_CMAKE_CONFIG_TEMPLATE_DIR}/${MYTOML_CMAKE_CONFIG_NAME}.cmake.in")
set(MYTOML_CMAKE_VERSION_CONFIG_TEMPLATE "${MYTOML_CMAKE_CONFIG_TEMPLATE_DIR}/${MYTOML_CMAKE_CONFIG_NAME}Version.cmake.in")
set(MYTOML_CMAKE_PACKAGE_CONFIG_TEMPLATE "${MYTOML_CMAKE_CONFIG_TEMPLATE_DIR}/pkg-config.pc.in")

set(MYTOML_CMAKE_CONFIG_DIR  ${CMAKE_CURRENT_BINARY_DIR})
set(MYTOML_CMAKE_CONFIG_FILE  ${MYTOML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Config.cmake)
set(MYTOML_CMAKE_TARGETS_FILE  ${MYTOML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}Targets.cmake)
set(MYTOML_CMAKE_PACKAGE_FILE  ${MYTOML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}.pc)
set(MYTOML_CMAKE_VERSION_CONFIG_FILE  ${MYTOML_CMAKE_CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake)

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

# static library
option(BUILD_SHARED_LIBS OFF)
add_library("${MYTOML_LIB_NAME}" ${MYTOML_SOURCE})

target_include_directories("${MYTOML_LIB_NAME}" 
    PUBLIC 
        $<BUILD_INTERFACE:${MYTOML_INCLUDE_BUILD_DIR}>
        $<INSTALL_INTERFACE:${MYTOML_INCLUDE_INSTALL_DIR}>
)
set_target_properties("${MYTOML_LIB_NAME}" PROPERTIES DEFINE_SYMBOL MYTOML_BUILD_STATIC)
set_target_properties("${MYTOML_LIB_NAME}" PROPERTIES VERSION ${PROJECT_VERSION})

# shared library
if(MYTOML_BUILD_SHARED)

    add_library("${MYTOML_LIB_NAME}s" SHARED ${MYTOML_SOURCE})

    target_include_directories("${MYTOML_LIB_NAME}" 
        PUBLIC 
            $<BUILD_INTERFACE:${MYTOML_INCLUDE_BUILD_DIR}>
            $<INSTALL_INTERFACE:${MYTOML_INCLUDE_INSTALL_DIR}>
    )
    set_target_properties("${MYTOML_LIB_NAME}s" PROPERTIES DEFINE_SYMBOL MYTOML_BUILD_SHARED )
    set_target_properties("${MYTOML_LIB_NAME}s" PROPERTIES VERSION ${PROJECT_VERSION})

endif()

if (MYTOML_BUILD_DEBUG)

    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()

    # static library
    add_library("${MYTOML_LIB_NAME}-d" ${MYTOML_SOURCE})

    target_include_directories("${MYTOML_LIB_NAME}-d" 
        PUBLIC 
            $<BUILD_INTERFACE:${MYTOML_INCLUDE_BUILD_DIR}>
    )
    set_target_properties("${MYTOML_LIB_NAME}-d" PROPERTIES DEFINE_SYMBOL MYTOML_BUILD_STATIC)
    set_target_properties("${MYTOML_LIB_NAME}-d" PROPERTIES VERSION ${PROJECT_VERSION})

    # shared library
    if(MYTOML_BUILD_SHARED)

        add_library("${MYTOML_LIB_NAME}s-d" SHARED ${MYTOML_SOURCE})

        target_include_directories("${MYTOML_LIB_NAME}s-d" 
            PUBLIC 
                $<BUILD_INTERFACE:${MYTOML_INCLUDE_BUILD_DIR}>
        )
        set_target_properties("${MYTOML_LIB_NAME}s-d" PROPERTIES DEFINE_SYMBOL MYTOML_BUILD_SHARED )
        set_target_properties("${MYTOML_LIB_NAME}s-d" PROPERTIES VERSION ${PROJECT_VERSION})
    endif()
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

#--------------------------------------------------------------------
# Add Project subdirectories
#--------------------------------------------------------------------

# Build the example apps   
if(MYTOML_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Build the documentation   
if(MYTOML_ENABLE_DOXYGEN)
    add_subdirectory(docs)
endif()

# Build the test apps   
if(MYTOML_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)

    # Set the unit test app project as the Visual Studio startup project
    # if the target compiler is some version of Microsoft Visual C++ and
    # if this project is the main project.
    if(MSVC AND CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MyTomlUnitTest)
    endif()
endif()

#--------------------------------------------------------------------
# Install a pkg-config file
#--------------------------------------------------------------------


#--------------------------------------------------------------------
# Configure package.
#--------------------------------------------------------------------

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
)

#--------------------------------------------------------------------
# Install package.
#--------------------------------------------------------------------

if(MYTOML_ENABLE_INSTALL)

    install(
        DIRECTORY ${MYTOML_INCLUDE_BUILD_DIR}
        DESTINATION ${MYTOML_INCLUDE_INSTALL_DIR}
    )

    install(
        FILES ${MYTOML_CMAKE_CONFIG_FILE} ${MYTOML_CMAKE_VERSION_CONFIG_FILE}
        DESTINATION ${MYTOML_CONFIG_INSTALL_DIR}
    )

    export(
        TARGETS ${MYTOML_TARGET_NAME}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${MYTOML_CMAKE_TARGETS_FILE}
    )

    install(
        TARGETS ${MYTOML_TARGET_NAME}
        EXPORT ${MYTOML_CMAKE_TARGET_NAME}
    )

    install(
        EXPORT ${MYTOML_CMAKE_TARGET_NAME}
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${MYTOML_CONFIG_INSTALL_DIR}
    )

    install(
        FILES "${MYTOML_CMAKE_PACKAGE_FILE}"
        DESTINATION ${MYTOML_CONFIG_INSTALL_DIR}
    )

endif()
