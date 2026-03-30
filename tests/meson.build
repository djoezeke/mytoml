# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

if mytoml_build_tests
	catch2_include_dirs = include_directories('vendor/Catch2/src')

    #===========================
    # C++ Library Tests        #
    #===========================

	mytoml_test_sources = [
		'cxx/Main.test.cpp',
		'cxx/Encoding.test.cpp',
		'cxx/Exceptions.test.cpp',
		'cxx/IAdapter.test.cpp',
		'cxx/Lexer.test.cpp',
		'cxx/Literals.test.cpp',
		'cxx/OAdapter.test.cpp',
		'cxx/Parser.test.cpp',
		'cxx/Serializer.test.cpp',
		'cxx/Traits.test.cpp',
		'cxx/Version.test.cpp'
	]

	mytoml_test_exe = executable(
		'mytoml_cpp_tests', 
		mytoml_test_sources,
		include_directories: [mytoml_include_dirs, catch2_include_dirs],
		dependencies: [mytoml_dep], 
		install: false
	)

	test('mytoml_cpp_tests', mytoml_test_exe)

    #===========================
    # C Library Tests          #
    #===========================

	mytomlc_test_sources = [
		'c/Main.test.cpp'
	]

	mytomlc_test_exe = executable(
		'mytoml_c_tests', 
		mytomlc_test_sources,
		include_directories: [mytoml_include_dirs, catch2_include_dirs],
		dependencies: [mytomlc_dep], 
		install: false
	)

	test('mytoml_c_tests', mytomlc_test_exe)

endif

# ===================================================================
