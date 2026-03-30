# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

#--------------------------------------------------------------------
# Project and version.
#--------------------------------------------------------------------

project('mytoml',
	['c', 'cpp'],
	version : '0.1.0',
	license : 'MIT',
	default_options : [
		'buildtype=release',
		'default_library=static',
		'cpp_std=' + get_option('mytoml_cpp_std')
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
mytoml_install = get_option('mytoml_install') and not mytoml_is_subproject
mytoml_build_c_api = get_option('mytoml_build_c_api')

src_files = []

#--------------------------------------------------------------------
# Build Project Libraries.
#--------------------------------------------------------------------

#===========================
# Build C++ Library        #
#===========================

mytoml_lib = library(
	'mytoml',
	'src/mytoml.cpp',
	include_directories : mytoml_include_dirs,
	install : mytoml_install,
)

#===========================
# Build C Library          #
#===========================

mytoml_dep = declare_dependency(
	link_with : mytoml_lib,
	include_directories : mytoml_include_dirs,
)

mytomlc_dep = mytoml_dep

if mytoml_build_c_api
	mytomlc_lib = library(
		'mytomlc', 
		'src/mytomlc.cpp',
		include_directories : mytoml_include_dirs,
		link_with : mytoml_lib,
		install : mytoml_install,
	)

	mytomlc_dep = declare_dependency(
		link_with : mytomlc_lib,
		include_directories : mytoml_include_dirs,
	)
endif

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

	if mytoml_install
		install_headers('include/mytoml/mytoml.h', subdir : 'mytoml')
		install_headers('include/mytoml/mytoml.hpp', subdir : 'mytoml')
	endif

	pkgc = import('pkgconfig')
	pkg_libraries = [mytoml_lib]
	if mytoml_build_c_api
		pkg_libraries += [mytomlc_lib]
	endif
	pkgc.generate(
		name : meson.project_name(),
		version: meson.project_version(),
		description: 'TOML for Modern C/C++',
		url: 'https://github.com/djoezeke/mytoml',
		libraries: pkg_libraries,
		subdirs : 'mytoml',
	)

endif

# ===================================================================
