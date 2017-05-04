Development of genvtx has migrated to Git


Bayeux/genvtx (generator of  vertexes) is a C++  program library which
provides some utilities to easily generate randomized vertexes in a 3D
geometry.  It aims  to be used at the generation  of primary events in
software framework such as Geant4 or whatever other physics simulation
library.

Bayeux/genvtx   provides  some   primitive   generators  of   vertexes
associated to several  2D/3D shapes of interest:  box, cylinder, tube,
polycons...  Most generators support uniformly distributed vertexes on
shape's surface of from shape's bulk volume.

Coupled  to  the geometry  mapping  provided  by the  Bayeux/geomtools
module  (identification scheme  of  physical volumes),  it enables  to
create  very easily  powerful  and complex  vertexes  generators in  a
geometry model.

Bayeux/genvtx  depends  on   the  Bayeux/geomtools,  Bayeux/mygsl  and
Bayeux/datatools C++ program libraries.

Bayeux/genvtx is distributed as a module of the Bayeux software suite.

Authors :
- François Mauger, Université de Caen Basse-Normandie, LPC Caen (CNRS/IN2P3)
- Xavier Garrido, Université Paris XI, LAL (CNRS/IN2P3)
- Ben Morgan, University of Warwick

Please read the COPYING.txt file for licensing terms.
