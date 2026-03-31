# ===================================================================
# clang-tidy script
# ===================================================================

if(NOT DEFINED PROJECT_SOURCE_DIR OR PROJECT_SOURCE_DIR STREQUAL "")
    get_filename_component(PROJECT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()

if(NOT DEFINED PROJECT_BINARY_DIR OR PROJECT_BINARY_DIR STREQUAL "")
    set(PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/build")
endif()

set(_clang_tidy_candidates)
if(DEFINED CLANG_TIDY_EXECUTABLE AND NOT CLANG_TIDY_EXECUTABLE STREQUAL "")
    list(APPEND _clang_tidy_candidates "${CLANG_TIDY_EXECUTABLE}")
endif()
list(APPEND _clang_tidy_candidates clang-tidy clang-tidy-18 clang-tidy-17 clang-tidy-16 clang-tidy-15 clang-tidy-14)

find_program(CLANG_TIDY_BIN NAMES ${_clang_tidy_candidates})
if(NOT CLANG_TIDY_BIN)
    message(FATAL_ERROR "clang-tidy executable not found. Set CLANG_TIDY_EXECUTABLE or install clang-tidy.")
endif()

if(NOT EXISTS "${PROJECT_BINARY_DIR}/compile_commands.json")
    message(FATAL_ERROR "compile_commands.json not found in ${PROJECT_BINARY_DIR}. Configure with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON.")
endif()

file(GLOB_RECURSE TIDY_FILES
    LIST_DIRECTORIES false
    "${PROJECT_SOURCE_DIR}/include/*/*.h"
    "${PROJECT_SOURCE_DIR}/include/*/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

if(TIDY_FILES STREQUAL "")
    message(STATUS "No source files found for clang-tidy")
    return()
endif()

string(REPLACE "\\" "/" _include_dir_regex "${PROJECT_SOURCE_DIR}/include/.*")
set(_tidy_failed OFF)

foreach(_file IN LISTS TIDY_FILES)
    execute_process(
        COMMAND "${CLANG_TIDY_BIN}" "${_file}" -p "${PROJECT_BINARY_DIR}" "--header-filter=^${_include_dir_regex}" -quiet
        RESULT_VARIABLE _tidy_result
    )

    if(NOT _tidy_result EQUAL 0)
        message(SEND_ERROR "clang-tidy failed: ${_file}")
        set(_tidy_failed ON)
    endif()
endforeach()

if(_tidy_failed)
    message(FATAL_ERROR "clang-tidy check failed")
endif()

message(STATUS "clang-tidy check passed")
