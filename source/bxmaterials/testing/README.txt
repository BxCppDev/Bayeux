README.txt
----------

These  sample files and  are used  during the  testing step  of materials.
They are  provided as  is as  examples of some  possible use  of materials
classes.

---------------------------------------------------------------------------

From  the build  directory, run  the test_manager program :

bash$ MATERIALS_TESTING_DIR=$(pwd)/testing __build-Linux-x86_64/testing/test_manager \
            --alias-allow-overload \
            ${MATERIALS_TESTING_DIR}/config/test_isotopes.def \
	    ${MATERIALS_TESTING_DIR}/config/test_elements.def \
	    ${MATERIALS_TESTING_DIR}/config/test_materials.def \
	    ${MATERIALS_TESTING_DIR}/config/test_materials_aliases.def \
	    ${MATERIALS_TESTING_DIR}/config/test_materials_aliases.redef

try also with one of these additional files:
	    ${MATERIALS_TESTING_DIR}/config/test_materials_aliases.bad
	    ${MATERIALS_TESTING_DIR}/config/test_materials_aliases.bad2

--
