# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

if(NOT TARGET mytoml)
	message(FATAL_ERROR "The 'mytoml' target must be created before building modules.")
endif()

add_library(mytoml-module)
add_library(Mytoml::Module ALIAS mytoml-module)

target_compile_features(mytoml-module PUBLIC cxx_std_20)

target_sources(mytoml-module
	PUBLIC
		FILE_SET modules TYPE CXX_MODULES FILES
			${CMAKE_CURRENT_SOURCE_DIR}/mytoml.cppm
)

target_link_libraries(mytoml-module PUBLIC Mytoml::Mytoml)

set_target_properties(mytoml-module PROPERTIES
	EXPORT_NAME Module
	FOLDER "Mytoml"
)
