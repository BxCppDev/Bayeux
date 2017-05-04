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

   * the ``datatools_ex_OCD`` : DLL linked with the ``Bayeux`` DLL
   * the ``ex_OCD`` : executable linked with the ``Bayeux`` DLL

 * Build method: CMake


Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory:

.. code:: sh

   shell> cp -a $(bxquery --exampledir)/datatools/OCD /tmp/datatools_ex_OCD
   shell> cd /tmp/datatools_ex_OCD
..

3. Build and install the example::

.. code:: sh

   shell> mkdir _build.d
   shell> cd _build.d
   shell> cmake \
        -DCMAKE_INSTALL_PREFIX=../_install.d  \
        -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
        ..
   shell> make
   shell> make install
   shell> cd ..
..

4. Run the example::

.. code:: sh

   shell> tree ./_install.d/
   shell> ./_install.d/ex_OCD
   shell> LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH} \
     bxocd_manual --load-dll "datatools_ex_OCD" \
     --class-id "foo"            \
     --action show
..

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

.. code:: sh

   shell> rm ex_OCD.foo.doc.rst
   shell> rm ex_OCD.foo.skeleton.conf
   shell> rm ex_OCD.service_manager.doc.rst
   shell> rm ex_OCD.service_manager.skeleton.conf
   shell> rm -fr _build.d
   shell> rm -fr _install.d
..
