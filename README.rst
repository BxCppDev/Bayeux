============================================================
Bayeux C++ Library for Experimental Particle/Nuclear Physics
============================================================

:Authors: Arnaud Chapon, Xavier Garrido, Benoit Guillon, François Mauger, Ben Morgan
:Date:    2016-03-16

.. contents::
   :depth: 3
..

About Bayeux
============

.. _Bayeux: https://nemo.lpc-caen.in2p3.fr/wiki/Software/Bayeux

Bayeux_ provides a collection of C++ classes and functions designed for
the  simulation,  recording  and  analysis of  data  for  experimental
particle  and nuclear  physics projects.  This functionality  is split
into several specialized submodules:

- datatools :  Serializable data  structures based  on Boost  and core
   utilities.

- brio  :  A  Boost  over   ROOT  I/O  serialization  system  (extends
   datatools).

- cuts : Generic tools for making arbitrary data selections.

- materials: Description  of isotopes,  elements and  materials plus
   tools for input to simulation applications (i.e. GDML/Geant4).

- mygsl : C++ wrapper and extensions to the GNU Scientific Library.

- dpp : A basic data processing pipeline API.

- geomtools : Generic tools for working with experiment geometries and
   provide input to simulation tools (i.e. GDML/Geant4).

- emfield : Electromagnetic field modelling and management.

- genbb_help : C++ port and  extensions to the Decay0/GENBB program by
   Vladimir Tretyak, for input to simulation applications.

- genvtx  : Vertex  random  generator tools  for  input to  simulation
   applications.

- mctools : Utilities for particle and nuclear physics simulation with
   an optional Geant4 interface.

- lahague : Utilities for dosimetry and radioprotection.



Naming and logo
---------------

Bayeux is  named thus  because it weaves  together several  threads of
software developed in Normandy ;-).

The use of the Bayeux name and logo (an extract of the lower frieze of
the Bayeux Tapestry) have been  courteously authorized by the Mayor of
the  City of  Bayeux  in December  2014 at  the  condition the  Bayeux
software tools are distributed under an open source software license.



Licensing
---------

Please study the  file ``LICENSE.txt`` for the  distribution terms and
conditions of use of Bayeux.

Bayeux contains some code extracted  from the *Kitware System* library
(OSI-approved BSD License),  the *BinReloc* library (Do  What The Fuck
You Want To Public License, Version  2) and the *EOS portable archive*
library (MIT License). Vladimir Tretyak  has accepted that
the  C++ wrapper  and port  of  the original  GENBB/DECAY0 library  is
published under the GPL 3 within Bayeux.


Getting Help
============

If you have problems, questions, ideas or suggestions on Bayeux or any
of its submodules, please contact the SuperNEMO Software Working Group
via  the  main   development  website  https://nemo.lpc-caen.in2p3.fr/
(protected access, only available to authorized collaborators).

You may  ask to  be registered  in one of  the protected  Bayeux users
diffusion lists:

 * ``bayeux-users-l@in2p3.fr`` : general,
 * ``bayeux-geometry-l@in2p3.fr`` : dedicated to geometry modelling,
 * ``bayeux-simulation-l@in2p3.fr`` : dedicated to simulation and the Geant4 module.

Please contact the Bayeux  team at bayeux@lpccaen.in2p3.fr (in English
or French).



Getting Bayeux
===============

You  can  obtain  the  Bayeux  source code  from  the  main  LPC  Caen
Subversion repository.

For example, to download Bayeux version 2.1.0, you may use:

.. code:: sh

    $ cd ${HOME}
    $ mkdir -p ${HOME}/Software/Bayeux/Source
    $ cd ${HOME}/Software/Bayeux/Source
    $ svn co [--username <your login>] https://nemo.lpc-caen.in2p3.fr/svn/Bayeux/tags/Bayeux-2.1.0 Bayeux-2.1.0

The  ``${HOME}/Software/Bayeux/Source/Bayeux-2.1.0`` source  directory
is thus created.

If you want  to download the development  version (possibly unstable),
use:

.. code:: sh

    $ svn co [--username <your login>] https://nemo.lpc-caen.in2p3.fr/svn/Bayeux/trunk Bayeux-trunk

The  ``${HOME}/Software/Bayeux/Source/Bayeux-trunk`` source  directory
is thus created.

You can  now create a  dedicated directory  to build Bayeux  2.1.0 (or
*trunk*) following  the guidelines in the  *Installing Bayeux* section
below.   Note that  different  versions of  Bayeux  may have  slightly
different build/installation procedures, so  you should read carefully
the ``README.rst`` file supplied with the source code.


Preparing Your System for Bayeux Installation
==============================================

To install Bayeux, you first need the following sofware:

Prerequisite Software
---------------------

-  **UNIX Operating System**:

   -  Linux
   -  Mac OS X 10.9 (Mavericks) or higher

We expect  Bayeux to  work on  most modern BSD  based Unices  or Linux
distributions  derived from  Fedora(RedHat)  and  Debian provided  the
software listed below is installed. However, we can only fully support
and test the following at present:

-  Ubuntu 12.04LTS and 14.04LTS: http://www.ubuntu.com
-  Scientific Linux 5/6: http://www.scientificlinux.org

If you have problems on systems other than these, please contact us,
though support will be on a best effort basis.

-  **Core Software Required**:

   -  CMake 3.3.0 or higher: http://www.cmake.org
   -  C/C++ compiler supporting at least C++03 standard
      (GNU/Clang/Intel)

On Linux,  you should  install these through  the package  manager for
your distribution. Some older  Linux systems (SL/CentOS, especially on
clusters) may  not provide CMake  3.3. If this  is the case,  then you
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

   -  Boost 1.55.0 or higher: http://www.boost.org
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

For ease of use, the  SuperNEMO collaboration provides the Cadfaelbrew_
SDK which bundles  these libraries for easy use by  Bayeux and clients
of  Bayeux.  It  is  strongly  advised to  use  this  bundle. It  will
provide, for Linux  and Mac systems, an  uniform software environement
with a selected set of blessed software, including the C++ compiler.

.. _Cadfael: https://nemo.lpc-caen.in2p3.fr/wiki/Software/Cadfael
.. _Cadfaelbrew: https://github.com/SuperNEMO-DBD/brew

-  **Additional Software Required**

    - datatools requires the Qt4 library when the ``BAYEUX_WITH_QT_GUI``
      option is set.

      On Ubuntu 14.04, this implies the installation of the following packages:

      .. code:: sh

		$ sudo apt-get install libqt4-dev libqt4-dev-bin libqt4-gui

   - geomtools also requires Gnuplot 4.0 or higher: http://www.gnuplot.info

      On Ubuntu 14.04, this implies the installation of the following packages:

      .. code:: sh

		$ sudo apt-get install gnuplot gnuplot-doc gnuplot-mode

   - datatools and geomtools uses the Readline library, if available:

      * http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html
      * http://askubuntu.com/questions/194523/how-do-i-install-gnu-readline

      On Ubuntu 14.04, this implies the installation of the following packages:

      .. code:: sh

		$ sudo apt-get install libreadline-dev readline-common

   - pandoc (http://johnmacfarlane.net/pandoc/) is  useful to generate
     documentation in user friendly format:

      On Ubuntu 14.04, this implies  the installation of the following
      packages:

      .. code:: sh

	 $ sudo apt-get install pandoc pandoc-data

   - docutils  (http://docutils.sourceforge.net/)  is also  useful  to
     generate documentation from ReST format in user friendly format:

      On Ubuntu 14.04, this implies the installation of the following packages:

      .. code:: sh

	 $ sudo apt-get install docutils-common docutils-doc python-docutils


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

   - Build type, e.g. ``Release``, ``Debug``. You will want this to be
      set  to ``Release``  in most  cases. ``Debug``  builds are  only
      needed if you  are needing to follow debugging  symbols into one
      of Cadfael's binaries.  It defaults to ``Release``,  so you will
      not need to change it in most cases.

Bayeux Configuration Options
----------------------------

These options control the core configuration of Bayeux.

-  ``BAYEUX_CXX_STANDARD``

   - Select the C++  Standard to compile against. Recognized values are:

     * ``98`` (default) : complete C++98 standard
     * ``11`` : all features of the C++11 standard in GCC 4.9 (provided
       for forward compatibility)
     * ``14``  :  same  as  ``11``  plus at  least  one  C++14  feature
       (provided for forward compatibility)

-  ``BAYEUX_COMPILER_ERROR_ON_WARNING``

   - Turn warnings into errors. Default is ON.

-  ``BAYEUX_WITH_IWYU_CHECK``

   - Run include-what-you-use on Bayeux sources. Default is OFF.

-  ``BAYEUX_WITH_DEVELOPER_TOOLS``

   -  Build and install additional tools for developers and *normal* users.
      Default is ON.

-  ``BAYEUX_WITH_BRIO``

   - Build the Bayeux/brio library module. Default is ON.

-  ``BAYEUX_WITH_CUTS``

   - Build the Bayeux/cuts library module. Default is ON.

-  ``BAYEUX_WITH_MYGSL``

   - Build the Bayeux/mygsl library module. Default is ON.

-  ``BAYEUX_WITH_DPP``

   - Build the Bayeux/dpp library module. Default is ON.

-  ``BAYEUX_WITH_MATERIALS``

   - Build the Bayeux/materials library module. Default is ON.

-  ``BAYEUX_WITH_GEOMTOOLS``

   - Build the Bayeux/geomtools library module. Default is ON.

-  ``BAYEUX_WITH_EMFIELD``

   - Build the Bayeux/emfield library module. Default is ON.

-  ``BAYEUX_WITH_GENBB_HELP``

   - Build the Bayeux/genbb_help library module. Default is ON.

-  ``BAYEUX_WITH_GENVTX``

   - Build the Bayeux/genvtx library module. Default is ON.

-  ``BAYEUX_WITH_MCTOOLS``

   - Build the Bayeux/mctools library module. Default is ON.

-  ``BAYEUX_WITH_GEANT4_MODULE``

   - Build the Bayeux/mctools Geant4 library extension module. Default is ON.

-  ``BAYEUX_WITH_MCNP_MODULE``

   - Build the Bayeux/mctools MCNP library extension module (experimental). Default is OFF.

-  ``BAYEUX_WITH_LAHAGUE``

   - Build the Bayeux/lahague library module. Default is OFF.

-  ``BAYEUX_WITH_QT_GUI``

   - Build the Qt-based GUI components. Default is OFF.

-  ``BAYEUX_ENABLE_TESTING``

   -  Build unit testing system for Bayeux. Default is OFF.

-  ``BAYEUX_WITH_DOCS``

   -  Build Bayeux documentation products. Default is ON.

-  ``BAYEUX_WITH_DOCS_OCD``

   -  Build OCD documentation. Default is OFF. Implies ``BAYEUX_WITH_DOCS``.


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
WIP


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

* Implement support for radioactive decays  using ENSDF from Geant4 in
  the genbb_help module.
* Implement the Bayeux/mctools MCNP extension library module and companion tools.
* Implement the Bayeux/datatools bxvariant_inspector application.


Acknowledgements
================

The authors gratefully thank the following persons for their direct or
indirect contributions to the Bayeux library:

* Vladimir  Tretyak  is  the  author of  the  original  *Decay0/GENBB*
  generator  (written in  Fortran 77)  from  which large  part of  the
  genbb_help module is derived.
* Christian Pfligersdorffer  is the author of  the Boost/Serialization
  *portable  binary archive*  classes which  is supported  by the  I/O
  *system of the datatools and brio modules.
* Nicolas Devillard and Rajarshi Guha  are the authors of the *Gnuplot
  pipe* library that is embedded in geomtools.
* Sylvette Lemagnen (Curator at the  Bayeux Museum) and Patrick Gomont
  (Mayor  of the  City  of  Bayeux) for  their  authorization for  the
  library's name and logo.
