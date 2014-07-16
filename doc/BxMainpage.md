Welcome to the Bayeux Documentation {#mainpage}
===================================

Overview {#bxmainpageoverview}
========

Bayeux provides a collection of C++ classes and functions designed
for the simulation, recording and analysis of data for experimental
particle and nuclear physics projects. This functionality is split
into several specialized submodules:

**datatools**
: Serializable data structures based on Boost.

**brio**
:     A Boost over ROOT I/O serialization system (extends datatools).

**cuts**
:     Generic tools for making arbitrary data selections.

**materials**
:     Description of isotopes, elements and materials plus tools
      for input to simulation applications (i.e. GDML/Geant4).

**mygsl**
:     C++ wrapper and extensions to the GNU Scientific Library.

**geomtools**
:     Generic tools for working with experiment geometries and
      provide input to simulation tools (i.e. GDML/Geant4).

**emfield**
:     Electromagnetic field modelling and management.

**genbb_help**
:     C++ wrapper and extensions to the Decay0/GENBB program
      by Vladimir Tretyak, for input to simulation applications.

**genvtx**
:     Vertex random generator tools for input to
      simulation applications.

**dpp**
:     A basic data processing pipeline API.

**mctools**
:     Utilities for particle and nuclear physics simulation with
      a Geant4 interface.


Authors {#bxmainpage_authors}
=======

  - François Mauger (LPC Caen, CNRS/IN2P3, ENSICAEN, Université de Caen Basse-Normandie, Caen, France)
  - Xavier Garrido (LAL, CNRS/IN2P3, Université Paris-Sud, Orsay, France)
  - Benoit Guillon (LPC Caen, CNRS/IN2P3, ENSICAEN, Caen, France)
  - Ben Morgan (The University of Warwick, Coventry, United Kingdom)

Acknowlegments {#bxmainpage_acknowlegments}
==============

Bayeux also makes use of some code fragments or contributions written by other people and/or companies/institutions:
  - Hongli Lai, for the BinReloc tools (WTFPL licence)
  - Christian Pfligersdorffer, for the portable binary archive (version 5.0, Boost software licence)
  - Kitware, Inc., Insight Software Consortium, for dynamic loader class (OSI-approved BSD License)
  - Vladimir Tretyak, for the original genbb/decay0 event generator
  - Nicolas Devillard, for the Gnuplot pipe utility (GNU GPL v2)

Licence {#bxmainpage_license}
=======

Bayeux  is distributed  under  the  terms of  the  GNU General  Public
License as published by the Free Software Foundation, either version 3
of the License, or (at your option) any later version.

~~~~~~~

 Copyright (C) 2008-2014 François Mauger, LPC Caen CNRS/IN2P3 and Université de Caen Basse-Normandie
 Contact: mauger@lpccaen.in2p3.fr

 This file is part of the Bayeux library.

 Bayeux is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Bayeux is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Bayeux.  If not, see <http://www.gnu.org/licenses/>.

~~~~~~~

Getting Started {#bxmainpage_gettingstarted}
===============

If you're reading this online and  don't yet have Bayeux installed, an
[Installation  Guide](bxinstallationguide.html)  is  available.   Note
that at present Bayeux is only  supported on POSIX platforms, and even
here not all variants are guaranteed to work!


Core Applications and Libraries {#bxmainpage_coreapplicationsandlibraries}
===============================

Once  you have  an install  of  Bayeux, the  following components  are
available to build client libraries and applications:

**libBayeux**
:     This is the main shared library that embeds all Bayeux's classes from
      Bayeux's submodules as well
      as core utilities like *system* registers of object factories, the `datatools`
      kernel and a few singletons.

**bxquery**
:     Request system informations about Bayeux.

**bxmaterials_inspector**
:     Inspect the configuration of a manager of materials (class `materials::manager`).

**bxgeomtools_inspector**
:     Inspect the configuration of a geometry modeling manager (class `geomtools::manager`).

**bxgeomtools_mkskelcfg**
:     TODO

**bxgenbb_inspector**
:     Inspect the configuration of a manager of particle generators (class `genbb::manager`).

**bxgenbb_mkskelcfg**
:     TODO

**bxgenvtx_mkskelcfg**
:     TODO

**bxgenvtx_production**
:     Inspect the configuration of a manager of vertex generators (class `genvtx::manager`).

**bxdpp_processing**
:     Run a pipeline of data processing modules (class `dpp::module_manager`)

**OCD support**
:     [Object Configuration Description](@ref bxocd)

User Guides {#bxmainpage_userguides}
===========

Documents detailing the use and design of Bayeux may be found on under
the  [Related  Pages](pages.html) tab  if  you  are reading  the  HTML
documentation, or in the following sections in the printed manual.

TODO