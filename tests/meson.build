# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

if mytoml_build_tests
	catch2_include_dirs = include_directories('vendor/Catch2/src')

	catch2_dep = dependency(
		'catch2-with-main',
        fallback: ['Catch2', 'catch2_with_main_dep'],
		required: true
	)

    #===========================
    # C++ Library Tests        #
    #===========================

	mytoml_test_sources = [
		'cxx/Main.test.cpp',
		'cxx/Api.test.cpp',
		'cxx/Emit.test.cpp',
		'cxx/Encoding.test.cpp',
		'cxx/Errors.test.cpp',
		'cxx/Parse.test.cpp',
		'cxx/Values.test.cpp',
		'cxx/Version.test.cpp',
	]

	mytoml_test_exe = executable(
		'mytoml_cpp_tests', 
		mytoml_test_sources,
		include_directories: [mytoml_include_dirs, catch2_include_dirs],
		dependencies: [mytoml_dep, catch2_dep], 
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
		dependencies: [mytomlc_dep, catch2_dep], 
		install: false
	)

	test('mytoml_c_tests', mytomlc_test_exe)

    #===========================
    # C++ Benchmarks           #
    #===========================

	if get_option('mytoml_build_benchmarks')
		mytoml_benchmark_exe = executable(
			'mytoml_cpp_benchmarks',
			'bench/Benchmarks.bench.cpp',
			include_directories: [mytoml_include_dirs, catch2_include_dirs],
			dependencies: [mytoml_dep, catch2_dep],
			install: false
		)

		# Benchmarks are registered as a Meson benchmark suite and are only
		# run explicitly with `meson test --benchmark`.
		benchmark('mytoml_cpp_benchmarks', mytoml_benchmark_exe)
	endif

endif

# ===================================================================
