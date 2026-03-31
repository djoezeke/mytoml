# ===================================================================
# clang-format check script
# ===================================================================

if(NOT DEFINED PROJECT_SOURCE_DIR OR PROJECT_SOURCE_DIR STREQUAL "")
    get_filename_component(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()

if(NOT DEFINED PROJECT_BINARY_DIR OR PROJECT_BINARY_DIR STREQUAL "")
    set(PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/build")
endif()

set(_clang_format_candidates)
if(DEFINED CLANG_FORMAT_EXECUTABLE AND NOT CLANG_FORMAT_EXECUTABLE STREQUAL "")
    list(APPEND _clang_format_candidates "${CLANG_FORMAT_EXECUTABLE}")
endif()
list(APPEND _clang_format_candidates clang-format clang-format-18 clang-format-17 clang-format-16 clang-format-15)

find_program(CLANG_FORMAT_BIN NAMES ${_clang_format_candidates})
if(NOT CLANG_FORMAT_BIN)
    message(FATAL_ERROR "clang-format executable not found. Set CLANG_FORMAT_EXECUTABLE or install clang-format.")
endif()

file(GLOB_RECURSE FORMAT_FILES
    LIST_DIRECTORIES false
    "${PROJECT_SOURCE_DIR}/include/*/*.h"
    "${PROJECT_SOURCE_DIR}/include/*/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

if(FORMAT_FILES STREQUAL "")
    message(STATUS "No source files found for format check")
    return()
endif()

set(_format_check_failed OFF)
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/.cmake-format")

foreach(_file IN LISTS FORMAT_FILES)
    execute_process(
        COMMAND "${CLANG_FORMAT_BIN}" -style=file "${_file}"
        OUTPUT_VARIABLE _formatted
        ERROR_VARIABLE _clang_format_stderr
        RESULT_VARIABLE _clang_format_result
    )

    if(NOT _clang_format_result EQUAL 0)
        message(FATAL_ERROR "clang-format failed for ${_file}:\n${_clang_format_stderr}")
    endif()

    string(MD5 _file_hash "${_file}")
    set(_tmp_file "${PROJECT_BINARY_DIR}/.cmake-format/${_file_hash}.tmp")
    file(WRITE "${_tmp_file}" "${_formatted}")

    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E compare_files "${_file}" "${_tmp_file}"
        RESULT_VARIABLE _compare_result
    )

    if(NOT _compare_result EQUAL 0)
        message(SEND_ERROR "Formatting mismatch: ${_file}")
        set(_format_check_failed ON)
    endif()
endforeach()

if(_format_check_failed)
    message(FATAL_ERROR "clang-format check failed")
endif()

message(STATUS "clang-format check passed")
