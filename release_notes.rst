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

Bayeux 3.4.1 requires Linux (recommended Ubuntu >=18.04, CentOS >=7.5),
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
* PR #27: Bayeux supports C++17 and C++20.
* PR #33: Bayeux CMake config script now defines the ``Bayeux_INCLUDE_DIRS`` variable.
* Support Boost 1.69.0 (recommended)
* Support only Boost 1.63 and 1.69 
* Support Camp 0.8.2 (recommended)
* Support CLHEP 2.4.1.0 (recommended)
* Add preliminary support for Geant4 10.5 with new datasets
* Support ROOT 6.16 (recommended)
* Issue #39: Bayeux does not support Boost 1.65 to 1.68 which
  have a broken serialization library.
  Force Bayeux to accept only Boost 1.63 and Boost 1.69.
* Issue #40: Portable binary archives (EOS) version 5.0
  used within Bayeux rely on Boost/Spirit FP utilities but
  Boost 1.69 does not provide it anymore.
  The Boost/Math  FP utilities are now used.

  Support a new version of the Portable binary archives (EPA) based on
  version 6.0 (candidate for inclusion in Boost/Serialization)
  which are fixed to support new versions of Boost. Automatic selection
  of the legacy EOS or EPA.
* mctools:  biaising   now  supports   an  oriented  disk   shape  for
  attractive/repulsive point of interest.

Removals
=========

* Remove option for experimental support of Bayeux/mctools  MCNP
  Monte-Carlo module.

Changes
=======

* brio: Allow file opening through the static API ``TFile::Open`` using URL
  and not only local filesystem path.

Fixes
=====

* datatools:  Fix smart  print  methods  of the  datatools::properties
  class.
* genbb: Fix  bug in probability  normalization in beta  decay classes
  with electron shakeoff randomized charge state.
* mctools:  Fix bug  in sensitive  detector (*material  name* stored  in
  place of *particle name*).
* Fix a few bugs detected with new Boost/CLHEP releases.

Bugs
====

* Geant4 10.5 (no MT build) is  not supported yet since some segfaults
  occur  while managing  hit collections  through sensitive  detectors
  (see issue #43).  Special option ``BAYEUX_WITH_GEANT4_EXPERIMENTAL``
  has  been introduced  to allow  the build  of Bayeux/mctools  Geant4
  Monte-Carlo module with experimental Geant4 10.5 support (for expert
  developpers only).


.. end
