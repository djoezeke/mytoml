# ===================================================================
# Copyright © 2025 Sackey Ezekiel Etrue
# Distributed under the terms of the MIT license.
# ===================================================================


if mytoml_build_examples

    #===========================
    # C++ Library API Examples #
    #===========================

	mytoml_examples = []

    foreach example : mytoml_examples

		exe = executable(
            example, example,
            include_directories: incdir,
            link_with: [mytoml_lib],
            install: false
        )
        
	endforeach

    #===========================
    # C Library API Examples   #
    #===========================

	mytomlc_examples = []

    foreach example : mytomlc_examples

		exe = executable(example + 'c',
            'apis/' + example,
            include_directories: incdir,
            link_with: [mytomlc_lib], 
            install: false
        )

	endforeach

endif

# ===================================================================
