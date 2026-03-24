# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# Project and version.
#--------------------------------------------------------------------

project('mytoml',
	'cpp',
	version : '0.1.0',
	license : 'MIT',
	default_options : [
		'buildtype=release',
		'default_library=static',
		'cpp_std=c++17'
		]
)

compiler = meson.get_compiler('cpp')
message('target cpu_family: @0@'.format(host_machine.cpu_family()))
message('target cpu: @0@'.format(host_machine.cpu()))
message('target system: @0@'.format(host_machine.system()))
message('target endian: @0@'.format(host_machine.endian()))

#--------------------------------------------------------------------
# Configure Project Variables.   
#--------------------------------------------------------------------

license = files('LICENSE')

mytoml_is_subproject = meson.is_subproject()
mytoml_include_dirs = include_directories('include')
mytoml_build_tests = get_option('mytoml_build_tests') and not mytoml_is_subproject
mytoml_build_examples = get_option('mytoml_build_examples') and not mytoml_is_subproject

src_files = []

#--------------------------------------------------------------------
# Build Project Libraries.
#--------------------------------------------------------------------

#===========================
# Build C++ Library        #
#===========================

mytoml_lib = static_library(
	'mytoml',
	'src/mytoml.cpp',
	include_directories : mytoml_include_dirs,
	install : true,
)

#===========================
# Build C Library          #
#===========================

mytomlc_lib = static_library(
	'mytomlc', 
	'src/mytomlc.cpp',
	include_directories : mytoml_include_dirs,
	link_with : mytoml_lib,
	install : true,
)

#--------------------------------------------------------------------
# Add Project subdirectories.
#--------------------------------------------------------------------

if mytoml_build_tests
	subdir('tests')
endif

if mytoml_build_examples
	subdir('examples')
endif

#--------------------------------------------------------------------
# Install Project Packages.
#--------------------------------------------------------------------

if not meson.is_subproject()

	install_headers('include/mytoml/mytoml.h', subdir : 'mytoml')
	install_headers('include/mytoml/mytoml.hpp', subdir : 'mytoml')

	pkgc = import('pkgconfig')
	pkgc.generate(
		name : meson.project_name(),
		version: meson.project_version(),
		description: 'TOML for Modern C/C++',
		url: 'https://github.com/djoezeke/mytoml',
	)

endif

# ===================================================================
