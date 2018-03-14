=============================
Bayeux 3.2.0 Release Notes
=============================

Bayeux 3.2 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux 3.2.0 requires Linux (for example Ubuntu 16.04 or later),
or macOS 10.


Additions
=========

* Add explicit release notes and archived notes.

* Add ``cmake/BayeuxDependencies.cmake`` file for the description of Bayeux's dependencies.

Removals
=========

None.

Changes
=======

* The  ``datatools::properties::fetch_real_with_explicit_unit`` method
  now uses  a more strict approach  to parse the unit  associated to a
  real  parameter with  its  expected explicit  unit (commit  5d71f78,
  merged PR #15)

* A new initialization  interface from the ``datatools::base_service``
  class.      The     (protected)      method     is     now     named
  ``datatools::base_service::_common_initialize(...)``  and should  be
  used  by  daughter  service  classes  in  place  of  the  deprecated
  ``datatools::base_service::common_initialize(...)``. See  an example
  in the ``cuts::cut_service::initialize`` method.

Fixes
=====

* Fix issue #11: remove a cyclic dependency (ea99c5a, merged from the release 3.1.X).
