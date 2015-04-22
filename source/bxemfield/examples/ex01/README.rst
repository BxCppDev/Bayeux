==========================
emfield ``examples/ex01``
==========================

Introduction
============

 * Description:

   This  example illustrates  how to  use the EM field manager to
   setup electromagnetic fields based on the ``emfield``
   library.

 * Source files :

   * ``ex01.cxx`` : the main program

 * Configuration files :

   * ``config/fields/manager.conf`` : the main configuration file of the EM fields
     manager.
   * ``config/fields/emfields.def`` : the definition of the electromagnetic fields.

 * Built object(s) :

     * ``ex01`` : the example executable linked to the ``Bayeux`` library.

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the geomtools library
2. Make a copy of the example directory::

      shell> cp -a $(bxquery --exampledir)/emfield/examples/ex01 /tmp/emfield_ex01
      shell> cd /tmp/emfield_ex01

3. Build and install the example::

      shell> mkdir __build
      shell> cd __build
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

4. Run the example::

      shell> CONFIG_DIR=./config ./ex01

8. Clean::

      shell> rm ex01
      shell> rm -fr __build
