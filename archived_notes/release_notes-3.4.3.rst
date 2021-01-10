=============================
Bayeux 3.4.3 Release Notes
=============================

Bayeux 3.4.3 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux  3.4.3 requires  Linux (recommended  Ubuntu >=18.04  (preferred
20.04), CentOS >=7.5), or macOS 10.


Additions
=========

* Add class ``geomtools::logical_volume_selector`` and use it in ``mctools::g4::detector_construction`` for SD association. Add a related test program.

  
Removals
=========


Changes
=======

* Bump to BxDecay0 version 1.0.5

Fixes
=====

* Fix syntax in ``mctools::g4::detector_construction`` config parsing.
* Fix a few typos and comments.
    
Bugs
====


.. end
