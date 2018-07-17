=============================
Bayeux 3.4.0 Release Notes
=============================

Bayeux 3.4 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux 3.4.0 requires Linux (for example Ubuntu 18.04 or later),
or macOS 10.


Additions
=========

* The ``datatools::factory`` class now has a enriched registration mechanism
  with the ability to register the ``std::type_info`` object associated to
  a registrered class factory. A templatized registration method is provided
  to automate the instantiation of a factory registration record.

  - Two new methods ``datatools::factory::fetch_type_id`` are provided
    to fetch a class factory registration ID by its type info or by the proper
    template class parameter.
  - Factory macros have been updated to this new interface.
  - Several client classes using the factory registration system
    have been updated too.

* The ``datatools::handle`` class is enriched by a standard smart pointer
  interfaces, including a ``datatools::make_handle`` template method (PR #21).
  
  
Removals
=========

None.


Changes
=======

None.


Fixes
=====

None.

