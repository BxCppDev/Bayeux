=====================
dpp ``examples/ex01``
=====================


Introduction
============

 * Description :

   This example illustrates the use of the ``module::manager`` class
   with embeded data processing module objects.

 * Source files :

   * ``hit.h``, ``hit.cc`` and ``hit.ipp``  : the ``dpp_ex01::hit`` class
   * ``raw_data.h``, ``raw_data.cc`` and ``raw_data.ipp``  : the ``dpp_ex01::raw_data`` class
     which contains a collection of ``dpp_ex01::hit`` objects
   * ``the_serializable.h``, ``the_serializable.cc`` and ``the_serializable.ipp`` :
     code dedicated to serialization of the  ``dpp_ex01::hit`` and ``dpp_ex01::raw_data`` classes
   * ``raw_generator_module.h``, ``raw_generator_module.cc`` : a data processing
     module class that fills a data bank with a ``dpp_ex01::raw_data`` object

 * Namespace : ``dpp_ex01``

 * Built objects :

   * the ``dpp_ex01`` DLL : data models and processing modules binary code
   * the ``dpp_ex01_bio`` DLL : Boost serialization binary code

 * Configuration files :

   * ``config/*`` : none for now

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

4. Run the example::

     shell>  LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH} \
             dpp_processing \
             --load-dll "dpp_ex01"  \
	     --load-dll "dpp_ex01_bio" \
	     --modulo 5 \
	     --max-events 1000 \
	     --module-manager-config "config/module_manager.conf" \
	     --module "pipeline" \
	     --max-events-per-output-file 250 \
	     --output-file dpp_ex01_01.data \
	     --output-file dpp_ex01_02.data \
	     --output-file dpp_ex01_03.data \
	     --output-file dpp_ex01_04.data

5. Check the output file:

6. Clean::

     shell> rm -fr ./lib
     shell> rm -fr __build


