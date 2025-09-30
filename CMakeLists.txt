cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)

project(Mytoml
        VERSION 0.2.0 
        DESCRIPTION "Toml parser for C/C++"
        HOMEPAGE_URL "https://github.com/djoezeke/mytoml"
        LANGUAGES  C
)

#--------------------------------------------------------------------
# Configure variables and build options for the library
#--------------------------------------------------------------------

# Check if Mytoml is the main project
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MYTOML_IS_TOP_LEVEL ON)
else()
  set(MYTOML_IS_TOP_LEVEL OFF)
endif()

set(MYTOML_LIB_NAME "mytoml" CACHE STRING "Base name of library output")

set(MYTOML_SOURCES mytoml.c)
set(MYTOML_INCLUDES mytoml.h)

# Set required C/C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

option(MYTOML_BUILD_SHARED "Build shared library" OFF)
option(MYTOML_BUILD_EXAMPLES "Build the ${PROJECT_NAME} example applications" ${MYTOML_IS_TOP_LEVEL})
option(MYTOML_BUILD_TESTS "Build the ${PROJECT_NAME} test programs" ${MYTOML_IS_TOP_LEVEL})

if(MYTOML_BUILD_SHARED)
    option(BUILD_SHARED_LIBS "Build shared libraries" ON)
endif()


#--------------------------------------------------------------------
# Build library targets
#--------------------------------------------------------------------

add_library(${MYTOML_LIB_NAME} ${MYTOML_SOURCES})

target_include_directories(${MYTOML_LIB_NAME} 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

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

if(BUILD_SHARED_LIBS)
    set_target_properties(${MYTOML_LIB_NAME} 
    PROPERTIES DEFINE_SYMBOL MYTOML_BUILD_SHARED
    )
else()
    set_target_properties(${MYTOML_LIB_NAME} 
    PROPERTIES DEFINE_SYMBOL MYTOML_BUILD_STATIC
    )
endif()

#--------------------------------------------------------------------
# Add Project subdirectories
#--------------------------------------------------------------------

# examples
if(MYTOML_BUILD_EXAMPLES)
    add_subdirectory(samples)
endif()

# tests
if(MYTOML_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
