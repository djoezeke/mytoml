cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)
project(Mytoml-Examples)

#--------------------------------------------------------------------
# Basic Sample Configures   
#--------------------------------------------------------------------

include(CMakeParseArguments)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


#--------------------------------------------------------------------
# Add Sample Function
#--------------------------------------------------------------------

# add a new target which is a MYTOML example
# example: 
#   mytoml_add_example(mytoml-sample basic.cpp ... libmytoml)
#   mytoml_add_example(mytoml-sample SHARED basic.cpp ... mytoml)
function(mytoml_add_example target SOURCES DEPENDS)

  if(NOT SOURCES)
    message(FATAL_ERROR "mytoml_add_example needs at least one source")
  endif()

  if(NOT DEPENDS)
    message(FATAL_ERROR "mytoml_add_example needs a dependency")
  endif()

    # set a source group for the source files
    source_group("" FILES ${SOURCES})

    # create the target
    add_executable(${target} ${SOURCES})

    # set the debug suffix
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)

    # set the target's folder (for IDEs that support it, e.g. Visual Studio)
    set_target_properties(${target} PROPERTIES FOLDER "Examples")

    # set the target C/C++ standard.
    set_property(TARGET ${target} PROPERTY C_STANDARD 17)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)

    # set the Visual Studio startup path for debugging
    set_target_properties(${target} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  if(ARGS_SHARED)
    set_target_properties(${target} PROPERTIES DEFINE_SYMBOL MYTOML_IMPORT)
  endif()

    # link the target to its dependencies
    target_link_libraries(${target} PUBLIC ${DEPENDS})

    # Enable support for UTF-8 characters in source code
    if(MSVC)
        target_compile_options(${target} PRIVATE /utf-8)
    endif()

    # Add the test
  add_test(NAME ${target} COMMAND ${target})

endfunction()

#--------------------------------------------------------------------
# Sample Functions
#--------------------------------------------------------------------

function(add_default_sample target sourcefiles)
  mytoml_add_example(${target} ${sourcefiles} "${MYTOML_LIB_NAME}")
endfunction()

function(add_debug_sample target sourcefiles)
  if(MYTOML_BUILD_DEBUG)
  mytoml_add_example(${target} ${sourcefiles} "${MYTOML_LIB_NAME}-d")
  endif()
endfunction()

#--------------------------------------------------------------------
# Recursively add Samples
#--------------------------------------------------------------------

# Automatically add all .c samples in this folder
file(GLOB C_SAMPLE_SOURCES "*.c")
foreach(SAMPLE_FILE ${C_SAMPLE_SOURCES})
  get_filename_component(SAMPLE_NAME ${SAMPLE_FILE} NAME_WE)
endforeach()


# Automatically add all .cpp samples in this folder
file(GLOB CPP_SAMPLE_SOURCES "*.cpp")
foreach(SAMPLE_FILE ${CPP_SAMPLE_SOURCES})
  get_filename_component(SAMPLE_NAME ${SAMPLE_FILE} NAME_WE)
endforeach()

add_default_sample(basic-sample basic.c)
