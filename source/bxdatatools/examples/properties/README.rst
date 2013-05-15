=================================
datatools ``examples/properties``
=================================

Introduction
============

 * Description:
   This example illustrates how to use some of the functionnalities
   of the ``datatools::properties`` class.

 * Source files :

   * ``ex_properties.cxx`` : the example main program source

 * Configuration file: none yet
 * Build method: CMake
 * Built object :

   * ``ex_properties`` : executable linked with the ``datatools`` DLL

Quick start
===========

1. Build, install and setup the datatools library
2. Make a copy of the example directory::

      shell> cp -a [datatools install base directory]/share/datatools/examples/properties /tmp/datatools_ex_properties
      shell> cd /tmp/datatools_ex_properties

3. Build and install the example::

      shell> mkdir __build
      shell> cd __build
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -Ddatatools_DIR=$(datatools-config --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

4. Run the example::

      shell> ./ex_properties --debug

5. Check the output backup file:

     * ``foo.conf`` : the ASCII file that containes the description of the
       ``datatools::properties`` container in human readable format.

6. Clean::

      shell> rm foo.conf
      shell> rm ex_properties
      shell> rm -fr __build


