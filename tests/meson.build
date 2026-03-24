# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================

if mytoml_build_tests

    #===========================
    # C++ Library Tests        #
    #===========================

	mytoml_test_sources = [
		'cxx/Version.test.cpp'
	]

	mytoml_test_exe = executable(
		mytoml_test, 
		mytoml_test_sources,
		include_directories: mytoml_include_dirs,
		link_with: [mytoml_lib], 
		install: false
	)

	test(mytoml_test, mytoml_test_exe)

    #===========================
    # C Library Tests          #
    #===========================

	mytomlc_test_sources = [
		'c/Version.test.cpp'
	]

	mytomlc_test_exe = executable(
		mytomlc_test, 
		mytomlc_test_sources,
		include_directories: mytoml_include_dirs,
		link_with: [mytomlc_lib], 
		install: false
	)

	test(mytomlc_test, mytomlc_test_exe)

endif

# ===================================================================
