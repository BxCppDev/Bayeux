=============================
datatools ``examples/ex_OCD``
=============================

Introduction
============
 * Description:
   This example illustrates the use of the Object Configuration
   Description mechanism (OCD) associated to an arbitrary user
   class and an official class from the datatools library.
 * Program source: ``ex_OCD.cxx``
 * Configuration file: ``config/ex_OCD.foo.candidate.conf``
 * Build method: CMake

Quick start
============

 1. Build, install and setup the datatools library
 2. Build and install the example::

      shell> mkdir __build
      shell> cd __build
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -Ddatatools_DIR=$(datatools-config --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

 3. Run the example::

      shell> ./ex_OCD

 4. Check the output OCD and skeleton files:
     * ``ex_OCD.foo.doc.txt``
     * ``ex_OCD.foo.skeleton.conf``
     * ``ex_OCD.service_manager.doc.txt``
     * ``ex_OCD.service_manager.skeleton.conf``

 5. Clean::

      shell> rm ex_OCD.foo.doc.txt
      shell> rm ex_OCD.foo.skeleton.conf
      shell> rm ex_OCD.service_manager.doc.txt
      shell> rm ex_OCD.service_manager.skeleton.conf
      shell> rm ex_OCD
      shell> rm -fr __build


