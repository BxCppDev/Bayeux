======================
brio ``examples/ex01``
======================

Introduction
============

 * Description:

   This  example illustrates  how to  use the  BRIO writer  and reader
   classes with  a sample  serialiable user class  (foo). It  uses the
   so-called  default store of  a Brio  writer and  save/write several
   instances of  the foo class in  a BRIo archive data  file.  It then
   uses  a BRIO  reader to  sequentially load, from the  file, the foo
   objects in memory, again using the default store.

 * Source files :

   * ``foo.h`` : the foo class header file
   * ``foo.cc`` : the foo class definition file
   * ``foo.ipp``: the foo class serialization source code
   * ``ex01.cxx`` : the main program

 * Build method: CMake

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory:

.. code:: sh

   shell> cp -a $(bxquery --exampledir)/brio/ex01 /tmp/brio_ex01
   shell> cd /tmp/brio_ex01
..


3. Build and install the example:

.. code:: sh

   shell> mkdir _build.d
   shell> cd _build.d
   shell> cmake \
	  -DCMAKE_INSTALL_PREFIX=../_install.d \
          -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
          ..
   shell> make
   shell> make install
   shell> cd ..
..

4. Run the example:

.. code:: sh

   shell> ./_install.d/ex01
..

5. Check the output BRIO data file:

     * ``ex01_data.brio`` :

       Despite it uses a ROOT tree as the storage container, this file
       is not ROOT-browsable because its entries are Boost binary archives
       (one per foo instance) stored as binary buffers of bytes.

6. Clean:

.. code:: sh

   shell> rm ex01_data.brio
   shell> rm -fr _build.d
   shell> rm -fr _install.d
..
