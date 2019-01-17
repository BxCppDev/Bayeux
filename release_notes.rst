=============================
Bayeux 3.4.1 Release Notes
=============================

Bayeux 3.4.1 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux 3.4.1 requires Linux (for example Ubuntu >=18.04, CentOS >=7.5),
or macOS 10.


Additions
=========

* mygsl : Add serialization and export methods in mygsl::tabulated_function.
* mygsl : Add kernel smoother algo (mygsl/kernel_smoother.h) with
          Nadaraya-Watson kernel-weigthed average functor with Gauss kernel smoother.
* datatools: Add parsing options and documentation for the datatools::properties class.
* datatools: Add some functionality tests of the datatools::properties class.

Removals
=========

None.

Changes
=======

None.


Fixes
=====

* datatools: Fix smart print method of the datatools::properties class.

