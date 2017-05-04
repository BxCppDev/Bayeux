
Bayeux/mctools  (Monte-Carlo Tools)  is  a C++  program library  which
provides  some  utilities  to   ease  the  generation  of  Monte-Carlo
simulation data particularly within the Geant4 simulation framework.

Bayeux/mctools implements a generic data model to represent the output
of a simulation program that tracks particles into a geometry model.

A  plugin  and  a  generic  application  dedicated  to  Geant4  driven
simulation are available.  They wrap  a Geant4 session manager and are
thus  able to  handle arbitrary  Monte Carlo  data production  through
Geant4. The output  simulated data (*MC truth hits*)  can be processed
by some external client programs, independently of the Geant4 engine.

Bayeux/mctools   depends   on  the   Bayeux/geomtools,   Bayeux/mygsl,
Bayeux/genvtx,  Bayeux/genbb_help  and  Bayeux/datatools  C++  program
library. It relies on the underlying CLHEP, GSL and Geant4 libraries.

Bayeux/mctools is distributed as a module of the Bayeux software suite.

Authors :
- François Mauger, Université de Caen Normandie, LPC Caen (CNRS/IN2P3)
- Xavier Garrido, Université Paris XI, LAL (CNRS/IN2P3)
- Arnaud Chapon, Cerap
- Ben Morgan, University of Warwick

Please read the COPYING.txt file for licensing terms.
