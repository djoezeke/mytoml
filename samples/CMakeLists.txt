cmake_minimum_required(VERSION 3.16...4.1.1 FATAL_ERROR)
project(Mytoml-Examples)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

#--------------------------------------------------------------------
# Sample Functions
#--------------------------------------------------------------------

function(add_c_sample NAME)
  add_executable(${NAME} ${NAME}.c)
  target_include_directories(${NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
  target_link_libraries(${NAME} ${MYJSON_LIB_NAME})
  set_property(TARGET ${NAME} PROPERTY C_STANDARD 17)
endfunction()

function(add_cpp_sample NAME)
  add_executable(${NAME} ${NAME}.cpp)
  target_include_directories(${NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
  target_link_libraries(${NAME} ${MYJSON_LIB_NAME})
  set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 17)
endfunction()

#--------------------------------------------------------------------
# Recursively add Samples
#--------------------------------------------------------------------

# Automatically add all .c samples in this folder
file(GLOB C_SAMPLE_SOURCES "*.c")
foreach(SAMPLE_FILE ${C_SAMPLE_SOURCES})
  get_filename_component(SAMPLE_NAME ${SAMPLE_FILE} NAME_WE)
  add_c_sample(${SAMPLE_NAME})
endforeach()


# Automatically add all .cpp samples in this folder
file(GLOB CPP_SAMPLE_SOURCES "*.cpp")
foreach(SAMPLE_FILE ${CPP_SAMPLE_SOURCES})
  get_filename_component(SAMPLE_NAME ${SAMPLE_FILE} NAME_WE)
  add_cpp_sample(${SAMPLE_NAME})
endforeach()

install(TARGETS ${SAMPLE_NAME} DESTINATION bin)