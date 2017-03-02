# -*- mode: rst; -*-

Bayeux/brio is a C++ library which provides some writer/reader classes
that  enable to  store/load arbitrary  serializable objects  (from the
Bayeux/datatools library) using embedded  portable binary (default) or
text archives from the Boost/Serialization system on top of ROOT files
(TFile) and  trees (TTree) as  storage media.

At  loading, serialized  data  can then  be accessed  non-sequentially
thanks to the ROOT I/O system.  Files may contains several *stores* of
archived data.  Each store can contain a  set of data of the same type
or data from mixed types, at user choice.

Bayeux/brio depends on the  Bayeux/datatools module and ROOT libraries
for low level I/O functionalities.

Bayeux/brio is distributed as a module of the Bayeux software suite.

Authors :

* François Mauger  (LPC Caen, CNRS/IN2P3, Université de Caen
Normandie) :  original author  and maintener,
* Xavier  Garrido (LAL,
CNRS/IN2P3,  Université   Paris-Sud)  :   bug  fixes,
*   Ben  Morgan
(University of Warwick) : CMake build system and packaging.

Please read the COPYING.txt file for licensing terms.
