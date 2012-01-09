
brio  is a  C++  library developped  by  François Mauger  at LPC  Caen
(CNRS/IN2P3)     and     Université     de    Caen     Basse-Normandie
(http://lpccaen.in2p3.fr). It provides some writer/reader classes that
enable  to store/load  arbitrary serializable  objects  using portable
binary (or text) archives from the Boost/Serialization system and ROOT
files  (TFile)  and  trees  (TTree)  as storage  media.   At  loading,
serialized data  can then be  accessed non-sequentially thanks  to the
ROOT  I/O system.   Files may  contains several  'stores'  of archived
data.  Each store can contains a set  of data of the same type or data
from mixed types.

brio depends on the datatools and ROOT libraries.

There are other contributors to this library :
 - Xavier Garrido (LAL (CNRS/IN2P3), Université Paris-Sud) : bug fixes
 - Ben Morgan (University of Warwick) : CMake build system

Please read the INSTALL.txt file for installation instructions.
Please read the COPYING.txt file for licensing terms.


