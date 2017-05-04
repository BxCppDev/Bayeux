======================
brio ``examples/ex02``
======================

Introduction
============

 * Description:

   This  example illustrates  how to  use the  BRIO writer  and reader
   classes with  a sample  serialiable user class  (foo). It creates
   explicitely named stores from  a Brio  writer and  save/write several
   types of objects in differents stores.  It then load again the objects
   from  a BRIO  reader.

 * Source files :

   * ``foo.h`` : the foo class header file
   * ``foo.cc`` : the foo class definition file
   * ``foo.ipp``: the foo class serialization source code
   * ``ex02.cxx`` : the main program

 * Build method: CMake

Quick start
===========

1. Build, install and setup the brio library
2. Make a copy of the example directory::

      shell> cp -a [brio install base directory]/share/brio/examples/ex02 /tmp/ex02
      shell> cd /tmp/ex02

3. Build and install the example::

      shell> mkdir __build
      shell> cd __build
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -Dbrio_DIR=$(brio-config --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

4. Run the example::

      shell> ./ex02

5. Check the output BRIO data file:

     * ``ex02_data.brio`` :

       Despite it uses a ROOT tree as the storage container, this file
       is not ROOT-browsable because its entries are Boost binary archives
       (one per foo instance) stored as binary buffers of bytes.

6. Clean::

      shell> rm ex02_data.brio
      shell> rm ex02
      shell> rm -fr __build


