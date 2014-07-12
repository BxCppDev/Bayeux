BAYEUX(1) {#bayeuxman}
=========

% BAYEUX(1) Bayeux Tools Documentation
% François Mauger
% July 2014

# NAME

Bayeux - A data analysis and simulation library

# SYNOPSIS

bxquery [options]


# OVERVIEW

Bayeux is a software suite to read, simulate, process and analyse raw data
from nuclear and particle physics experiments.

Bayeux is free software delivered as a ready-to-run binary distribution or
as source code that you can freely use, copy, modify and distribute. It is
licensed under the GPL version 3. It runs on POSIX operating systems.

It builds heavily on third party software (Boost, CAMP, CLHEP, GSL, Geant4, Root),
which by default is distributed with the `Cadfael` SDK.

The functionality of Bayeux is broken down into several submodules

**datatools**
:      Serializable data structures based on Boost.


**brio**
:      A Boost over ROOT I/O serialization system (extends datatools).

**cuts**
:      Generic tools for making arbitrary data selections.

**materials**
:      Description of isotopes, elements and materials plus tools
       for input to simulation applications (i.e. GDML/Geant4).

**mygsl**
:      C++ wrapper and extensions to the GNU Scientific Library.

**geomtools**
:      Generic tools for working with experiment geometries and
       provide input to simulation tools (i.e. GDML/Geant4).

**emfield**
:      Electromagnetic field modelling and management.

**dpp**
:      A basic data processing pipeline API.

**genbb_help**
:      C++ wrapper and extensions to the Decay0/GENBB program
       by Vladimir Tretyak, for input to simulation applications.

**genvtx**
:      Vertex random generator tools for input to
       simulation applications.

**mctools**
:      Utilities for particle and nuclear physics simulation with
       a Geant4 interface.


For more information about Bayeux, point your browser to docs...

# SEE ALSO

`bxquery`(1), `bxocd_manual`(1), `bxgeomtools_inspector`(1).

# COPYRIGHT

Copyright (C) 2014 SuperNEMO Collaboration
