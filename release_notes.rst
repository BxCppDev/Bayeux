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
* datatools: Add new smart print interface to the datatools::properties
  and datatools::multi_properties classes.
* mctools: Add a flag in the biasing point_of_interest class
* PR#27: Bayeux supports C++17 and C++20.
* PR#33: Bayeux CMake config script now defines the ``Bayeux_INCLUDE_DIRS`` variable.
* Support Boost 1.69.0
* Support Camp 0.8.2
* Support CLHEP 2.4.1.0
* Add preliminary support for Geant4 10.5 with new datasets
* Support ROOT 6.16
* Issue #39: Bayeux does not support Boost 1.68.0
  
Removals
=========

None.

Changes
=======

* brio: Allow file opening through the static API ``TFile::Open`` using URL
  and not only local filesystem path.

Fixes
=====

* datatools: Fix smart print methods of the datatools::properties class.
* genbb: Fix bug in probability normalization in beta decay classes with electron
  shakeoff randomized charge state.
* mctools: Fix bug in sensitive detector (material name stored in place of particle name).
* Fix a few bugs detected with new Boost/CLHEP...
