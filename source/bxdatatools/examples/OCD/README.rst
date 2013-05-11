=============================
datatools ``examples/ex_OCD``
=============================

Introduction
============

 * Description:
   This example illustrates the use of the Object Configuration
   Description mechanism (OCD) associated to an arbitrary user
   class and an official class from the datatools library.

 * Source files :

   * ``foo.h`` : the foo class header file
   * ``foo.cc`` : the foo class definition file
   * ``ex_OCD.cxx`` : the main program

 * Configuration file: ``config/ex_OCD.foo.candidate.conf``
 * Build method: CMake

Quick start
===========

1. Build, install and setup the datatools library
2. Make a copy of the example directory::

      shell> cp -a [datatools install base directory]/share/datatools/examples/OCD /tmp/datatools_ex_OCD
      shell> cd /tmp/datatools_ex_OCD

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

      shell> ./ex_OCD

5. Check the output OCD and skeleton files:

     * ``ex_OCD.foo.doc.rst`` : The description of the ``foo`` configuration
       parameters in ReStructuredText format
     * ``ex_OCD.foo.skeleton.conf`` : A configuration skeleton file for an
       object of class ``foo``
     * ``ex_OCD.service_manager.doc.rst``: The description of the
       ``datatools::service_manager``  configuration parameters in ReStructuredText format
     * ``ex_OCD.service_manager.skeleton.conf`` : A configuration skeleton file for an
       object of class ``datatools::service_manager``

6. Clean::

      shell> rm ex_OCD.foo.doc.rst
      shell> rm ex_OCD.foo.skeleton.conf
      shell> rm ex_OCD.service_manager.doc.rst
      shell> rm ex_OCD.service_manager.skeleton.conf
      shell> rm ex_OCD
      shell> rm -fr __build


