=============================
Bayeux 3.3.0 Release Notes
=============================

Bayeux 3.3 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux 3.3.0 requires Linux (for example Ubuntu 16.04 or later),
or macOS 10.


Additions
=========

* Issue #17 : Add query methods in the ``datatools::urn_db_service`` and
  ``datatools::urn_to_path_resolver_service`` classes.
* Add the ``genbb::time_slicer_generator`` class.
* Add support for regexp for logical volumes' name in Geant4 region (detector construction
  and the ``mctools::g4::region_info`` class.

  Example:
  
.. code:: 
	  
   #@description List of regions
   regions.names : string[1] = "MyRegion"

   #@description Explicit list of attached volumes/models for region "MyRegion"
   regions.MyRegion.volumes  : string[2] = "block1.log" "block2.log"

   #@description List of regex for volumes/models for region "MyRegion"
   regions.MyRegion.volumes_regexp : string[2] = \
      "^sub_block_(.*)_front\.log$" \
      "^sub_block_(.*)_back\.log$" 

..

  This is a request from Falaise_ development team to support a large number of logical volumes (>200)
  of which names use a regular pattern but which does not share the same material and thus cannot be
  aggregated in the same region using such a criterion.

.. _Falaise: https://github.com/SuperNEMO-DBD/Falaise   
  
Removals
=========

None.


Changes
=======

None.


Fixes
=====

* Fix issue  #18 : Fix  initialization stage which mount  external URN
  Database Service instances.

* Fix issue #16 : In the ``bayeux::detail::bayeux_library`` singleton,
  the system URN Database Service cannot  be mounted by any client URN
  Database Service if it is not locked. The fix consists in explicitly
  locking(unlocking) the system URN  Database service of the Bayeux's
  library  before(after) the  *kernel push*(*pop*)  call (registration
  in/deregistration  from  the  Bayeux's kernel  URN  Query  Singleton
  Service).

* Fix warning issues:

  * Add ``-Wno-error=noexcept-type`` in global settings
  * ``geomtools/utils.h``:       add        pragma       for       the
    ``geomtools::randomize_direction`` template function which emits a
    warning with GCC 7 on Ubuntu 18.04.

