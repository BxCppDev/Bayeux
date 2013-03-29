dpp library 

README.txt
----------


Test the module manager class :

export DPP_DATA_DIR=$(pwd)
DPP_TMP_TEST_DIR=${DPP_DATA_DIR} \
            __build-Linux-x86_64/testing/test_module_manager \
            --debug \
            --verbose \
            -l mygsl \
	    --config ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \
	    --module "chain0" \
	    ${DPP_DATA_DIR}/testing/data/data_0.txt.gz \
	    ${DPP_DATA_DIR}/testing/data/data_1.txt.gz \
	    ${DPP_DATA_DIR}/testing/data/data_2.txt.gz \
	    ${DPP_DATA_DIR}/testing/data/data_3.txt.gz \
	    test_module_chain_out.txt 


--

