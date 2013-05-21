=====================
dpp ``examples/ex01``
=====================


Introduction
============

 * Description :

   This example illustrates the use of the ``module::manager`` class
   with embeded data processing module objects from the ``dpp_processing``
   program.

   It defines a simple and serializable data model representing a collection
   of raw hits in a detector. It also defines a special data processing module
   for the generation of simulated raw hits and storage in the main
   data/event record (implemented with a ``datatools::things`` instance).

   The example creates a core DLL, an associated DLL dedicated to
   serialization, and provides a test executable.

   Finally, it shows how to use the ``dpp_processing`` executable to
   run a data processing pipeline.

 * Source files :

   * The ``dpp_ex01::hit`` class:

     * ``dpp_ex01/hit.h``
     * ``dpp_ex01/hit.ipp``
     * ``src/hit.cc``

   * The ``dpp_ex01::raw_data`` class contains a collection
     of ``dpp_ex01::hit`` objects:

     * ``dpp_ex01/raw_data.h``
     * ``dpp_ex01/raw_data.ipp``
     * ``src/raw_data.cc``

   * Serialization code for the  ``dpp_ex01::hit`` and ``dpp_ex01::raw_data`` classes :

     * ``dpp_ex01/the_serializable.h``
     * ``dpp_ex01/the_serializable.ipp``
     * ``src/the_serializable.cc``

   * A data processing module class that fills a data bank
     with a ``dpp_ex01::raw_data`` object

     * ``dpp_ex01/raw_generator_module.h``
     * ``src/raw_generator_module.cc``

   * Testing :

     * ``testing/test_dpp_ex01.cxx`` : a test source program
     * ``testing/rgm.conf`` : a test configuration file

 * Namespace : ``dpp_ex01``

 * Built objects :

   * the ``dpp_ex01`` DLL : data models (``dpp_ex01::hit`` and
     ``dpp_ex01::raw_data`` classes) and processing modules
     (``dpp_ex01::raw_generator_module``)
   * the ``dpp_ex01_bio`` DLL : Boost serialization of the
     ``dpp_ex01::hit`` and ``dpp_ex01::raw_data`` classes
   * ``test_dpp_ex01`` : test executable

 * Configuration files :

   * ``config/module_manager.conf`` : the configuration parameters for
     the module manager embeded in ``dpp_processing``
   * ``config/modules.conf`` : the configuration parameters for some data
     processing modules to be dynamically loaded by ``dpp_processing``

 * Outputs :

   * ``test_dpp_ex01.xml`` : test program output file to check
     serialization of the ``dpp_ex01::hit`` and ``dpp_ex01::raw_data``
     classes
   * ``dpp_ex01_*.xml`` : final output files of the ``dpp_processing``
   * ``dpp_ex01_intermediate_*.brio`` : intermediate output files
     generated during the running of ``dpp_processing``

 * Build method : CMake


Quick start
===========

1. Build, install and setup the dpp library
2. Make a copy of the example directory::

     shell> cp -a [dpp install base directory]/share/dpp/examples/ex01 /tmp/dpp_ex01
     shell> cd /tmp/dpp_ex01

3. Build and install the example::

     shell> mkdir __build
     shell> cd __build
     shell> cmake \
       -DCMAKE_INSTALL_PREFIX=.. \
       -Ddpp_DIR=$(dpp-config --prefix) \
       ..
     shell> make
     shell> make install
     shell> cd ..

4. Testing::

     * List of registered class IDs in the ``dpp_ex01`` DLL::

        shell> export LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH}
        shell> ocd_manual --load-dll dpp_ex01 --action list | grep "dpp_ex01::" 2> /dev/null

     * Generate the OCD documentation (ReST+HTML) for class ``dpp_ex01::raw_generator_module``::

        shell> ocd_manual --load-dll dpp_ex01 --action show \
                --class-id dpp_ex01::raw_generator_module > dpp_ex01_RGM.rst
        shell> pandoc -r rst -w html dpp_ex01_RGM.rst > dpp_ex01_RGM.html

     * Run the test program and check its output file::

        shell> ./test_dpp_ex01
        shell> less test_dpp_ex01.xml

5. Run the example::

     shell> LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH} \
              dpp_processing \
	        --verbose \
	        --load-dll "dpp_ex01_bio" \
	        --modulo 1 \
	        --max-records 1000 \
	        --module-manager-config "config/module_manager.conf" \
	        --module "pipeline" \
	        --max-records-per-output-file 250 \
	        --output-file dpp_ex01_01.xml \
	        --output-file dpp_ex01_02.xml \
	        --output-file dpp_ex01_03.xml \
	        --output-file dpp_ex01_04.xml

6. Check the output files::

     shell> ls -l dpp_ex01_intermediate_0?.brio
     shell> ls -l dpp_ex01_0?.xml
     shell> less  dpp_ex01_01.xml

7. Clean::

     shell> rm -f ./dpp_ex01_intermediate_*.brio
     shell> rm -f ./dpp_ex01_*.xml
     shell> rm -f ./test_dpp_ex01.xml
     shell> rm -f ./test_dpp_ex01
     shell> rm -fr ./lib
     shell> rm -fr ./__build


