cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)
project(Mytoml-Tests)

#--------------------------------------------------------------------
# Basic Test Configures   
#--------------------------------------------------------------------

include(CMakeParseArguments)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(WIN32)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

include_directories("." "../")

#--------------------------------------------------------------------
# Add Test Function
#--------------------------------------------------------------------

# add a new target which is a MYTOML test
# example: 
#   mytoml_add_test(mytoml-test basic.cpp ... libmytoml)
#   mytoml_add_test(mytoml-test SHARED basic.cpp ... mytoml)
function(mytoml_add_test target SOURCES DEPENDS)

    # set a source group for the source files
    source_group("" FILES ${SOURCES})

    # create the target
    add_executable(${target} ${SOURCES})

    # set the debug suffix
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)

    # set the target's folder (for IDEs that support it, e.g. Visual Studio)
    set_target_properties(${target} PROPERTIES FOLDER "Tests")

    # set the target C/C++ standard.
    set_property(TARGET ${target} PROPERTY C_STANDARD 17)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)

    set_target_properties(${target} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} # set the Visual Studio startup path for debugging
        VS_DEBUGGER_COMMAND_ARGUMENTS "-b" # Break into debugger

        XCODE_GENERATE_SCHEME ON # Required to set arguments
        XCODE_SCHEME_ARGUMENTS "-b" # Break into debugger
    )

    if(ARGS_SHARED)
      set_target_properties(${target} PROPERTIES DEFINE_SYMBOL MYTOML_IMPORT)
    endif()

    # link the target to its dependencies
    target_link_libraries(${target} PRIVATE ${DEPENDS})

    # Enable support for UTF-8 characters in source code
    if(MSVC)
        target_compile_options(${target} PRIVATE /utf-8)
    endif()

    # Add the test
  add_test(NAME ${target} COMMAND ${target})

endfunction()

#--------------------------------------------------------------------
# Test Functions
#--------------------------------------------------------------------

function(add_test_default target sourcefiles)
  mytoml_add_test(${target} ${sourcefiles} "${MYTOML_LIB_NAME}")
endfunction()

function(add_test_debug target sourcefiles)
  if(MYTOML_BUILD_DEBUG)
  mytoml_add_test(${target} ${sourcefiles} "${MYTOML_LIB_NAME}-d")
  endif()
endfunction()


#--------------------------------------------------------------------
# Recursively add Tests
#--------------------------------------------------------------------

# Automatically add all .c tests in this folder
file(GLOB C_TEST_SOURCES "*.c")
foreach(TEST_FILE ${C_TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
endforeach()

# Automatically add all .cpp tests in this folder
file(GLOB CPP_TEST_SOURCES "*.cpp")
foreach(TEST_FILE ${CPP_TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
endforeach()

