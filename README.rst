============================================================
Bayeux C++ Library for Experimental Particle/Nuclear Physics
============================================================

:Authors: Xavier Garrido, Benoit Guillon, François Mauger, Ben Morgan
:Date:    2014-06-03

.. contents::
   :depth: 3
..

About Bayeux
============

Bayeux provides a collection of C++ classes and functions designed for
the  simulation,  recording  and  analysis of  data  for  experimental
particle  and nuclear  physics projects.  This functionality  is split
into several specialized submodules:

-  datatools :  Serializable data structures based on Boost and core utilities.

-  cuts :       Generic tools for making arbitrary data selections.

-  materials :  Description of isotopes, elements and materials plus tools
	        for input to simulation applications (i.e. GDML/Geant4).

-  mygsl :      C++ wrapper and extensions to the GNU Scientific Library.

-  geomtools :  Generic tools for working with experiment geometries and
                provide input to simulation tools (i.e. GDML/Geant4).

-  emfield :    Electromagnetic field modelling and management.

-  brio :       A Boost over ROOT I/O serialization system (extends datatools).

-  dpp :        A basic data processing pipeline API.

-  genbb_help : C++ wrapper and extensions to the Decay0/GENBB program
                by Vladimir Tretyak, for input to simulation applications.

-  genvtx :     Vertex random generator tools for input to
                simulation applications.

-  mctools :    Utilities for particle and nuclear physics simulation with
                a Geant4 interface.



Naming
------

Bayeux is  named thus  because it weaves  together several  threads of
software developed in Normandy ;-).


Licensing
---------

Please study the  file ``LICENSE.txt`` for the  distribution terms and
conditions of use of Bayeux.

Bayeux contains some code extracted  from the *Kitware System* library
(OSI-approved BSD License),  the *BinReloc* library (Do  What The Fuck
You Want To Public License, Version  2) and the *EOS portable archive*
library (Boost Software License).


Getting Help
============

If you have problems, questions, ideas or suggestions on Bayeux or any
of its submodules, please contact the SuperNEMO Software Working Group
via the main development website https://nemo.lpc-caen.in2p3.fr/.


Getting Bayeux
===============

You can obtain the Bayeux source code from the main LPC Caen Subversion
repository.

Preparing Your System for Bayeux Installation
==============================================

To install Bayeux, you first need the following sofware:

Prerequisite Software
---------------------

-  **UNIX Operating System**:

   -  Linux
   -  Mac OS X 10.7 (Lion) or higher

We expect  Bayeux to  work on  most modern BSD  based Unices  or Linux
distributions  derived from  Fedora(RedHat)  and  Debian provided  the
software listed below is installed. However, we can only fully support
and test the following at present:

-  Ubuntu 12.04LTS: http://www.ubuntu.com
-  Scientific Linux 5/6: http://www.scientificlinux.org

If you have problems on systems other than these, please contact us,
though support will be on a best effort basis.

-  **Core Software Required**:

   -  CMake 2.8.0 or higher: http://www.cmake.org
   -  C/C++ compiler supporting at least C++03 standard
      (GNU/Clang/Intel)

On Linux,  you should  install these through  the package  manager for
your distribution. Some older  Linux systems (SL/CentOS, especially on
clusters) may  not provide CMake  2.8. If this  is the case,  then you
should download the latest Linux *binary .sh* file from:

-  http://www.cmake.org/cmake/resources/software.html

and follow the instructions on that page to install it.

On Mac OS X, simply install CMake from the latest ``Darwin64`` dmg
bundle available from:

-  http://www.cmake.org/cmake/resources/software.html

To obtain the other tools, simply  install the latest version of Xcode
from the  Mac App Store.  After installation, you should  also install
the Xcode command line tools via going to the menu Xcode->Preferences,
clicking on the Downloads tab, and then installing Command Line Tools.

-  **Core Libraries Required**

   -  Boost 1.53.0 or higher: http://www.boost.org
       - with serialization, filesystem, system libraries.
   -  Camp 0.7.1 : https://github.com/tegesoft/camp
   -  GSL 1.15 or higher: http://www.gnu.org/s/gsl
   -  CLHEP 2.1.3.2 or higher: http://proj-clhep.web.cern.ch
   -  Geant4 9.6.0 or higher: http://geant4.cern.ch
       - with GDML support enabled (through the XercesC library)
   -  ROOT 5.34.0 or higher: http://root.cern.ch
      geomtools requires you setup ROOT at least with support for:

       * minimal X11,
       * GDML,
       * OpenGL.

For ease of use, LPC Caen provides the Cadfael SDK which bundles these
libraries for  easy use by Bayeux  and clients of Bayeux.  You can use
this bundle, or, at your discretion,  your own, or system, installs of
these packages.

-  **Additional Software Required**

   - geomtools also requires Gnuplot 4.0 or higher: http://www.gnuplot.info
   - geomtools uses the Readline library, if available:

      * http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html
      * http://askubuntu.com/questions/194523/how-do-i-install-gnu-readline

   - pandoc is useful to generate documentation in user friendly format:
     http://johnmacfarlane.net/pandoc/

Known Issues on Tested Platforms
--------------------------------
None known at present.


Installing Bayeux
=================

Bayeux provides a  CMake based build system. We'll  assume for brevity
that you  are using  a UNIX system  on the command  line (i.e.  Mac or
Linux).  We'll also assume that you're going to use the Cadfael SDK to
provide the required third party packages.

Configuring the Installation
----------------------------

The directory in which this  ``README.rst`` file resides is called the
"source directory"  of Bayeux. Because  CMake generates many  files as
part of the configuration and  build process, we perform configuration
in a directory isolated from the  source directory. This enables us to
quickly clean  up in  the event  of issues,  and prevents  commital of
generated (and hence system dependent) files to the repository.

To configure Bayeux, simply do, from the source directory of Bayeux:

.. code:: sh

    $ mkdir Bayeux-build
    $ cd Bayeux-build
    $ cmake -DCMAKE_INSTALL_PREFIX=<where you want to install> -DCMAKE_PREFIX_PATH=<path to your Cadfael install> ..

You  may also  use  an  arbitrary build  directory  somewhere in  your
filesystem:

.. code:: sh

    $ mkdir /tmp/Bayeux-build
    $ cd /tmp/Bayeux-build
    $ cmake -DCMAKE_INSTALL_PREFIX=<where you want to install> -DCMAKE_PREFIX_PATH=<path to your Cadfael install> <path to the Bayeux source directory>

CMake Configuration Options
---------------------------

These options control the underlying CMake system, a full list can be
obtained from the CMake documentation, but in Bayeux you will only need
to deal with the following three in most cases:

-  ``CMAKE_INSTALL_PREFIX``

   -  Path under which to install Bayeux. It should point to an empty,
      writable directory. It defaults to ``/usr/local`` so you will want
      to change this.

-  ``CMAKE_PREFIX_PATH``

   -  Path under which Cadfael is installed.


-  ``CMAKE_BUILD_TYPE``

   -  Build type, e.g. Release, Debug. You  will want this to be set to
      Release in most  cases. Debug builds are only needed  if you are
      needing  to  follow  debugging  symbols into  one  of  Cadfael's
      binaries. It defaults to Release, so you will not need to change
      it in most cases.

Bayeux Configuration Options
----------------------------

These options control the core configuration of Bayeux.

-  ``Bayeux_ENABLE_TESTING``

   -  Build the collection of Bayeux's test programs. Default is OFF.

-  ``Bayeux_WITH_GEANT4``

   -  Build the Bayeux/mctools Geant4 extension module. Default is OFF.

-  ``Bayeux_WITH_EXAMPLES``

   -  Install the Bayeux example projects. Default is ON.

-  ``Bayeux_BUILD_DOCS``

   -  Build and install the documentation. Default is OFF.

-  ``Bayeux_BUILD_DEVELOPER_TOOLS``

   -  Build and install additional tools for developers and *normal* users.
      Default is OFF.

Building and Installing
-----------------------

Once  you have  generated  the buildsystem  for  Bayeux, as  described
earlier, you are ready to build.  Note that if you want to reconfigure
at  any  time, you  can  simply  run  ``ccmake``  again in  the  build
directory.

By default Bayeux  generates a Makefile based system, so  to build and
install Bayeux, simply run

.. code:: sh

    $ make [-j4]
    $ make install

where ``-j4`` indicates  the number of processors to be  used to build
Bayeux.


Running test
------------

In order  to run the  test programs  provided with the  various Bayeux
submodules,  you should  have activated  the ``Bayeux_ENABLE_TESTING``
configuration option. From the build directory, simply run

.. code:: sh

    $ make test



Troubleshooting
===============
WIP


Using Bayeux
============
See the projects under the examples directory.


Developing Bayeux
=================

WIP

A note on RPATHs
----------------

You should not use the  (DY)LD_LIBRARY_PATH variables because they are
intended for testing,  not production (see the man  pages of ld/dyld).
Bayeux uses  rpaths to provide a  simple setup that allows  apps to be
run directly with guaranteed  library lookup. Morever, relative rpaths
are used that generally allow Bayeux to be relocatable.

However, these settings are platform dependent and CMake has only added
support for this gradually. In particular, see these references:

* [Kitware Blog article on Mac OS X RPATH handling](http://www.kitware.com/blog/home/post/510)
* [Handling Mac RPATH on older CMake](http://www.mail-archive.com/cmake@cmake.org/msg47143.html)
* [CMake's general RPATH handling](http://www.cmake.org/Wiki/CMake_RPATH_handling)

Note  also that  if  you  have (DY)LD_LIBRARY_PATH  set,  you may  see
startup errors if  any of the paths contain libraries  used by Bayeux,
e.g. ROOT.  In general, you should never need to set the library path,
though many scientific software projects (badly mis)use it.


To do
=====

* Implement support for configuration variants in the datatools module
  (through  the datatools::properties  and datatools::multi_properties
  classes).
* Implement support for radioactive decays  using ENSDF from Geant4 in
  the genbb_help module.
* Implement support  for mapped electromagnetic fields  in the emfield
  module.


Acknowledgements
================

The authors gratefully thank the following persons for their direct or
indirect contributions to the Bayeux library:

* Vladimir  Tretyak  is  the   author  of  the  original  *Decay0/GENBB*
  generator  (written in  Fortran 77)  from  which large  part of  the
  genbb_help module is derived.
* Christian Pfligersdorffer  is the author of  the Boost/Serialization
  *based portable  binary archive* classes  which is supported  by the
  I/O system of the datatools and brio modules.
* Nicolas Devillard and Rajarshi Guha are the authors of the *Gnuplot pipe*
  library that is embedded in geomtools.
