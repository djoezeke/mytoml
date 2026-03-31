# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# Recursively Add Examples
#--------------------------------------------------------------------

# Automatically add all .c samples in this folder
file(GLOB C_SAMPLE_SOURCES "*.c")
foreach(EXAMPLE_FILE ${C_SAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)
	mytoml_add_example("${EXAMPLE_NAME}"
						SOURCES "${EXAMPLE_FILE}"
						DEPENDS Mytoml::MytomlC)
endforeach()


# Automatically add all .cpp samples in this folder
file(GLOB CPP_SAMPLE_SOURCES "*.cpp")
list(REMOVE_ITEM CPP_SAMPLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp")
foreach(EXAMPLE_FILE ${CPP_SAMPLE_SOURCES})
  	get_filename_component(EXAMPLE_NAME ${EXAMPLE_FILE} NAME_WE)
	mytoml_add_example("${EXAMPLE_NAME}"
						SOURCES "${EXAMPLE_FILE}"
						DEPENDS Mytoml::Mytoml)
endforeach()

if(TARGET mytoml-module)
    mytoml_add_example("ModuleExample"
                        SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/ModuleExample.cpp"
                        DEPENDS Mytoml::Module)
    target_compile_features(ModuleExample PRIVATE cxx_std_20)
endif()
