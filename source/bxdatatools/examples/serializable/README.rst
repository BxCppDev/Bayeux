===================================
datatools ``examples/serializable``
===================================

Introduction
============

 * Description:

   This example illustrates  how to create a  class with serialization
   support within  the ``Bayeux/datatools``  framework.  Serialization
   support means  that any object  of this class will  be serializable
   through  the Boost/Serialization  library  and  Bayeux I/O  derived
   tools. This  also means that such  objects will be storable  in the
   banks of a ``datatools::things`` container.

 * Source files :

   * Class ``raw_hit`` : represent a hit in some detector

     * ``raw_hit.h`` : header file
     * ``raw_hit.cc`` : source file
     * ``raw_hit.ipp`` : serialization code

   * Class ``raw_data`` : represent a data structure with
     a collection of hits (``raw_hit``)

     * ``raw_data.h`` : header file
     * ``raw_data.cc`` : source file
     * ``raw_data.ipp`` : serialization code

   * Dedicated files for the instantiation of the serialization code
     associated to classes and registration of classes :

     * ``the_serializable.h`` : forced instantiation of template serialization code
       and registration of the classes in the  Boost/Serialization system

     * ``the_serializable.cc`` : wrapper to build the ``datatools_ex_serializable_bio``
       DLL (see below)

 * Build method: CMake
 * Built object(s) :

   * ``datatools_ex_serializable`` : the DLL linked with the ``datatools`` DLL, contains
     the ``raw_hit`` class
   * ``datatools_ex_serializable_bio`` : the serialization DLL linked with the ``datatools_ex_serializable`` DLL
   * ``ex_serializable_1`` : executable linked with the ``datatools_ex_serializable_bio`` DLLs

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory::

      shell> cp -a $(bxquery --exampledir)/datatools/examples/serializable /tmp/datatools_ex_serializable
      shell> cd /tmp/datatools_ex_serializable

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

      shell> ./ex_serializable_1 --debug

5. Have a look on the output human readable XML archives:

   * ``raw_hit.xml`` : output from the ``ex_raw_hit_1`` function, input for the
     ``ex_raw_hit_2`` function
   * ``raw_hits.xml`` : output from the ``ex_raw_hit_3`` function, input for the
     ``ex_raw_hit_4`` function
   * ``raw_hits_h.xml`` : output from the ``ex_raw_hit_5`` function, input for the
     ``ex_raw_hit_6`` function
   * ``raw_hit_banks.xml`` : corrupted output from the ``ex_raw_hit_7`` function
   * ``raw_data_banks.xml`` : output from the ``ex_raw_hit_8`` function, input for the
     ``ex_raw_hit_9`` function


6. Clean::

      shell> rm -f raw_*.xml
      shell> rm -f ex_serializable_1
      shell> rm -fr ./lib
      shell> rm -fr __build
