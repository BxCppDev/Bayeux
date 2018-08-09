===================================
mctools ``validation/serialization``
===================================

Introduction
============

This project illustrates de/serialization of
objects of ``mctools::base_step_hit`` class
and validate versioning support.


Contents
========

* ``exstore.cxx`` : Program used to generate reference Boost XML archive files.
* ``exload.cxx`` : Program used to load reference Boost XML archive files
  and perform backward compatibility checks.
* ``samples/`` : Collection of reference validation files:

  * ``out-1.xml`` : Boost XML archive file with instances of
    ``mctools::base_step_hit`` class using the serialization version 1
    (generated with Bayeux 3.3.0).
  * ``out-2.xml`` : Boost XML archive file with instances of
    ``mctools::base_step_hit`` class using the serialization version 2
    (generated with Bayeux 3.4.0).
  * ``test-1.txt`` : ASCII output of instances obtained from deserialization
    of the ``out-1.xml`` file.
  * ``test-2.txt`` : ASCII output of instances obtained from deserialization
    of the ``out-2.xml`` file (must be identical to ``test-1.txt``).
* ``process.sh`` : Shell script to prepare versioned archive data files.
* ``run.sh`` : Shell script to run the validation.
* ``CMakeLists.txt``: CMake script.
    
Usage
=====
   
#. Build and run the (de)serialization program with Bayeux <=3.3.0
   with class ``mctools::base_step_hit`` version 1:

   .. code:: shell

      $ ./prepare.sh --store -C --downgrade
   ..

#. Build and run the (de)serialization program with Bayeux >=3.4.0
   with class ``mctools::base_step_hit`` version 2:

   .. code:: shell

      $ ./prepare.sh --store -C
   ..




.. end
