================================
datatools ``examples/ex_things``
================================

Introduction
============

 * Description:
   This example illustrates how to use some of the functionnalities
   of the ``datatools::things`` class.

 * Source files :

   * ``ex_things_1.cxx`` : the example main program source

 * Configuration file: none yet
 * Build method: CMake
 * Built object(s) :

   * ``ex_things_1`` : executable linked with the ``datatools`` DLL

Quick start
===========

1. Build, install and setup the datatools library
2. Make a copy of the example directory::

      shell> cp -a [datatools install base directory]/share/datatools/examples/things /tmp/datatools_ex_things
      shell> cd /tmp/datatools_ex_things

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

      shell> ./ex_things_1 --debug

5. Clean::

      shell> rm ex_things
      shell> rm -fr __build


