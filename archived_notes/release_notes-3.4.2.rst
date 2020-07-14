=============================
Bayeux 3.4.2 Release Notes
=============================

Bayeux 3.4.2 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux 3.4.2 requires Linux (recommended Ubuntu >=18.04 (preferred 20.04), CentOS >=7.5),
or macOS 10.


Additions
=========

* PR #49:       Modernize      ``datatools::properties``      and
  ``datatools::multi_properties`` classes.
* PR #50:       Modernize some other *data model* classes.
* Add  optional  support  for  external BxDecay0_  C++  library  (from
  version 1) in place of the  C++ port of the legacy Decay0/Genbb code
  embedded in the  genbb_help module.  The BxDecay0  is an independant
  reimplementation  of the  Decay0 Fortran  code which  should be  far
  easier  to maintain  and keep  synchronized with  Vladimir Tretiak's
  work.
* Describe more available build options.
* Implement the new ``print_tree`` method for various classes.
* Implement new file inclusion mechanism from  ``datatools::properties``
  and ``datatools::multi_properties`` formatted files.
* Support array append mode for the ``datatools::properties`` parser
* Support array item override mode for the ``datatools::properties`` parser
* PR #57 : The ``bxg4_seeds`` program allows to extend a list of simulation runs
  with statistically independant PRNG seeds.
* Support with GCC 9.3.0 (default on Ubuntu 20.04)
* Support Boost 1.71 (default on Ubuntu 20.04) and set minimum version for Boost at 1.69
* Support CLHEP 2.1.4.2
* Support CAMP 0.8.4 (default on Ubuntu 20.04)
  
Removals
=========

* Remove   unused  ``debug``   attribute   and   related  methods   in
  ``datatools::multi_properties``  class, propagate  this change  to a
  few places in the library.
* Remove deprecated Doxygen directives

Changes
=======
  
Fixes
=====

* PR   #58  :   Fix  a   badly   computed  event   timestamp  in   the
  ``genbb::time_slicer_generator`` class
* Add some pragmas for GCC (9) to workaround warnings from Geant4 (for
  the G4 plugin only)
* Fix a bug in ``datatools::ui::ihs`` class 
* Fix a bug  in ``BinReloc`` that occurs with GCC  9 : unresolved path
  due  to an  unexpected ``r--p``  permissions  in ``/proc/self/maps``
  while ``r-xp`` was used before  (no documentation found so far about
  it)
    
Bugs
====


.. _BxDecay0: https://github.com/BxCppDev/bxdecay0

.. end
