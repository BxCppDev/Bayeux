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

Bayeux 3.4.2 requires Linux (recommended Ubuntu >=18.04, CentOS >=7.5),
or macOS 10.


Additions
=========

* PR #49:       Modernize      ``datatools::properties``      and
  ``datatools::multi_properties`` classes (B.Morgan).
* PR #50:       Modernize some other *data model* classes (B.Morgan).
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
* PR #57 : The ``bxg4_seeds`` program allows to extend a list of simulation runs
  with statistically independant PRNG seeds.
  
Removals
=========

* Remove   unused  ``debug``   attribute   and   related  methods   in
  ``datatools::multi_properties``  class, propagate  this change  to a
  few places in the library.

Changes
=======
  
Fixes
=====

* PR #58 : Fix a badly computed event timestamp in the ``genbb::time_slicer_generator`` class

Bugs
====


.. _BxDecay0: https://github.com/BxCppDev/bxdecay0

.. end
