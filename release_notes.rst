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
* PR #50:       Modernize some other *data model* classes.

Removals
=========

* Remove   unused  ``debug``   attribute   and   related  methods   in
  ``datatools::multi_properties``  class, propagate  this change  to a
  few places in the library.

Changes
=======

Fixes
=====

Bugs
====


.. end
