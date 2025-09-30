cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)
project(Mytoml-Tests)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(WIN32)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

include_directories(".")

#--------------------------------------------------------------------
# Test Functions
#--------------------------------------------------------------------

function(add_c_test NAME)
  add_executable(${NAME} ${NAME}.c)
  target_link_libraries(${NAME} PRIVATE ${MYTOML_LIB_NAME})
  set_property(TARGET ${NAME} PROPERTY C_STANDARD 17)
  add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

function(add_cpp_test NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE ${MYTOML_LIB_NAME})
  set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 17)
  add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

#--------------------------------------------------------------------
# Recursively add Tests
#--------------------------------------------------------------------

# Automatically add all .c tests in this folder
file(GLOB C_TEST_SOURCES "*.c")
foreach(TEST_FILE ${C_TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_c_test(${TEST_NAME})
endforeach()

# Automatically add all .cpp tests in this folder
file(GLOB CPP_TEST_SOURCES "*.cpp")
foreach(TEST_FILE ${CPP_TEST_SOURCES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_cpp_test(${TEST_NAME})
endforeach()
