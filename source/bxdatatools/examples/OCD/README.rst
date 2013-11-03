==========================
datatools ``examples/OCD``
==========================

Introduction
============

 * Description:

   This  example  illustrates  the  use of  the  Object  Configuration
   Description mechanism  (OCD) associated to an  arbitrary user class
   and  an official  class from  the Bayeux/datatools  module library.
   The user  class being  part of a  DLL, one show  how to  browse the
   associated OCD material through the ``bxocd_manual`` executable.

 * Source files :

   * The ``foo`` class :

     * ``foo.h`` : header file
     * ``foo.cc`` :  definition file

   * ``ex_OCD.cxx`` : the example main program source

 * Configuration file: ``config/ex_OCD.foo.candidate.conf``

 * Built objects :

   * the ``datatools_ex_OCD`` : DLL linked with the ``datatools`` DLL
   * the ``ex_OCD`` : executable linked with the ``datatools_ex_OCD`` DLL

 * Build method: CMake


Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory::

      shell> cp -a $(bxquery --exampledir)/datatools/OCD /tmp/datatools_ex_OCD
      shell> cd /tmp/datatools_ex_OCD

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

      shell> ls -l ./lib/
      shell> ls -l ./ex_OCD
      shell> ./ex_OCD
      shell> LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH} \
             bxocd_manual --load-dll "datatools_ex_OCD" \
                          --class-id "foo"            \
                          --action show

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
