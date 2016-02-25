=================================
datatools ``examples/properties``
=================================

Introduction
============

 * Description:
   this example illustrates how to use some of the functionnalities
   of the ``datatools::properties`` class.

 * Source files :

   * ``ex_properties.cxx`` : the example main program source.

 * Configuration file: none
 * Build method: CMake
 * Built object :

   * ``ex_properties`` : executable linked with the ``Bayeux`` shared library.

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory::

      shell> cp -a $(bxquery --exampledir)/datatools/examples/properties /tmp/datatools_ex_properties
      shell> cd /tmp/datatools_ex_properties

3. Build and install the example::

      shell> mkdir _build.d
      shell> cd _build.d
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

4. Run the example::

      shell> ./ex_properties --debug

5. Check the output backup file:

     * ``foo.conf`` : the ASCII file that contains the description of the
       ``datatools::properties`` container in a human readable format.

6. Clean::

      shell> rm foo.conf
      shell> rm ex_properties
      shell> rm -fr _build.d
