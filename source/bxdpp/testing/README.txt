dpp library

README.txt
----------


Test the module manager class :

DPP_TMP_TEST_DIR=$(pwd) \
            __build-Linux-x86_64/testing/test_module_manager \
            --debug \
            --verbose \
            -l mygsl \
	    --config ${DPP_TESTING_DIR}/config/test_module_manager.conf \
	    --module "chain0" \
	    ${DPP_TESTING_DIR}/data/data_0.txt.gz \
	    ${DPP_TESTING_DIR}/data/data_1.txt.gz \
	    ${DPP_TESTING_DIR}/data/data_2.txt.gz \
	    ${DPP_TESTING_DIR}/data/data_3.txt.gz \
	    test_module_chain_out.txt


--
