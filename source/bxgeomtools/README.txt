
Bayeux/geomtools is a  C++ library which provides  some basic geometry
classes   and   tools   for  geometric   modelling   and   management.

Bayeux/geomtools  reimplements   in  some  way  the   Geant4  geometry
modelling  system. However  it  adds high  level functionalities  like:
- the support for dynamic shape  objects factories,
- a collection of configurable  geometry models with smart placement policies,
- a generic geometry identification scheme for  physical volumes
  within  a geometry  hierarchical  model
- a plugin system which can accomodate a database of arbitrary materials
  or the description of electromagnetic fields of any kind.

It is  compatible with  the GDML concepts  for geometry  modelling. It
provides naturally an  GDML export interface with the  Geant4 and ROOT
geometry systems through this language.

Bayeux/geomtools depends on the  Bayeux/materials and Bayeux/mygsl C++
libraries.  It  relies on  some CLHEP classes.

Bayeux/geomtools is  distributed as  a module  of the  Bayeux software
suite.

Authors :
 - François Mauger <mauger@lpccaen.in2p3.fr> (Université de Caen Normandie, LPC Caen (CNRS/IN2P3)) : geometric modelling engine and core functionalities
 - Xavier Garrido (LAL (CNRS/IN2P3), Université Paris-Sud) : bug fixes, tests and validation
 - Ben Morgan (University of Warwick) : build system

Please read the COPYING.txt file for licensing terms.
