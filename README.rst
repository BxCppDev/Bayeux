================================================================
Bayeux C++ Library for Experimental Particle and Nuclear Physics
================================================================

.. image:: source/bxbayeux/logo/logo_bayeux_240x200_transparent.png
   :width: 200pt

:Authors: The BxCppDev_ group
:Date:    2023-09-18
:Contact: bayeux@lpccaen.in2p3.fr

.. contents::
   :depth: 3
..

.. _BxCppDev: https://github.com/BxCppDev

.. raw:: pdf

   PageBreak oneColumn


About Bayeux
============

.. _Bayeux: https://github.com/BxCppDev/Bayeux

Bayeux_ provides  a collection of  C++ classes and  functions designed
for the  simulation, recording and  analysis of data  for experimental
particle  and nuclear  physics  projects.   These functionalities  are
splitted into several specialized submodules:

datatools
  Generic data  structures with  Boost serialization support  and core
  utilities  for software  configuration and  management (support  for
  variant configurations, object factories, service management...).

brio
  A *Boost/serialization over ROOT  I/O* system (extends datatools I/O
  tools).

cuts
  Generic tools for making arbitrary data selections.

materials
  Description  of isotopes,  elements and  materials, providing
  input to simulation applications (i.e. GDML/Geant4).

mygsl
  C++ wrappers to the  GNU Scientific Library and extension for
  numerical tools.

dpp
  A basic data processing pipeline API with support for processor and service plugins.

geomtools
  Generic tools for the modelling of experimental geometries,
  providing input to simulation tools (i.e. GDML/Geant4).

emfield
  Electromagnetic field modelling and management.

genbb(_help)
  C++ port and  extensions to the Genbb/Decay0 program by
  Volodymyr Tretyak, providing input to simulation applications.
  Now depends on the external BxDecay0_ library.

genvtx
  Vertex  random  generator tools providing input to  simulation
  applications.

mctools
  Utilities for particle and nuclear physics simulation with
  a Geant4 interface.

lahague
  Utilities for dosimetry and radiation protection (optional).


.. _BxDecay0: https://github.com/BxCppDev/bxdecay0

.. raw:: pdf

   PageBreak oneColumn

Bayeux's roots
--------------

Original work on Bayeux was  initiated at the `Laboratoire de Physique
Corpusculaire de  Caen`_ (CNRS/IN2P3)  in the  framework of  the NEMO3
double-beta  decay  experiment and  the  R&D  for its  successor:  the
SuperNEMO experiment.  Bayeux  is developped by a  group of physicists
working in Nuclear  and Particle Physics. It  provides generic classes
and tools that can be used in  many different contexts.  It is the
cornerstone of the SuperNEMO_  experiment's production software chain.
As a  generic toolbox,  it is  also used by  several projects  for the
simulation of  particle transport  through matter (through  its Geant4
extension  module), detector  design  for  nuclear physics,  detection
efficiency  calculation,  data   serialization,  data  processing  and
analysis,  radiation  protection   and  dosimetry  studies  (including
industrial projects).

.. _SuperNEMO: https://github.com/SuperNEMO-DBD
.. _Linuxbrew: https://github.com/topics/linuxbrew

Naming and logo
---------------

Bayeux is  named thus  because it weaves  together several  threads of
software developed in Normandy ;-).

The use of the Bayeux name and logo (an extract of the lower frieze of
the Bayeux Tapestry) have been  courteously authorized by the Mayor of
the  City of  Bayeux  in December  2014 at  the  condition the  Bayeux
software tools are distributed under an open source software license.

This  authorization  is explicitely  granted  to  the
`Laboratoire de Physique Corpusculaire de Caen`_
(UMR6534,  CNRS/IN2P3,  UNICAEN, ENSICAEN, Normandie Univ.).

.. _`Laboratoire de Physique Corpusculaire de Caen`: http://www.lpc-caen.in2p3.fr/


Licensing
---------

Please study the  file ``LICENSE.txt`` for the  distribution terms and
conditions of use of Bayeux.

Bayeux contains some code extracted  from :

* the *Kitware System* library (OSI-approved BSD License),
* the *BinReloc* library (Do  What The Fuck You Want To Public License, Version  2),
* the *EOS portable archive* library (MIT License).

Volodymyr Tretyak has kindly accepted that the C++ wrapper and C++ port
of the  original Genbb/Decay0 Fortran  library is published  under the
GPL 3 within Bayeux. This part of the code is now hosted in the
BxDecay0_ project.

Some parts of code or  management/build scripts are released under the
MIT License.

All other parts of Bayeux's C++ source code are released under the GNU
General Public License 3.0.


Supported environments
----------------------

Bayeux  is expected  to work  on modern  Linux distributions  based on
Debian  or Fedora,  as Linux  is our  main development  and production
environment.  Other  UNIX-like OSes  (Linux, BSD, macOS)  **may** work
with some work of adapting.

Bayeux uses the C++11 standard by default so this implies the use of a
modern C++ compiler (example: GCC version >= 4.9).

You may  use a dedicated `Linuxbrew`_  *tap* like the one  proposed by
the BxCppDev_  group (`bxcppdev/bxtap`_). It  may help you to  setup a
suitable  working environment  on  your system.   However we  consider
Linuxbrew as  error-prone and we recommend  to use as far  as possible
the package manager of your system.   You may also consider to use the
Spack_ package manager to satisfy Bayeux's software dependencies.

.. _`Spack`: https://spack.io/


Releases
--------

* Latest release: `Bayeux-3.5.4`_

.. _`Bayeux-3.5.4`: https://github.com/BxCppDev/Bayeux/releases/tag/Bayeux-3.5.4

.. raw:: pdf

   PageBreak oneColumn

Getting Help
============

If you have problems, questions, ideas or suggestions on Bayeux or any
of  its submodules,  please contact  the BxCppDev  Group via  the main
development  platform   https://github.com/BxCppDev.   You   may  also
contact  the Bayeux  team  at bayeux@lpccaen.in2p3.fr  (in English  or
French).


Getting Bayeux
===============

You can  obtain the Bayeux source  code from the main  BxCppDev GitHub
repository.

For example, to download Bayeux version 3.5.4, you may use, assuming a
Linux system:

.. code:: sh

   $ cd ${HOME}
   $ mkdir -p ${HOME}/BxCppDev
   $ cd ${HOME}/BxCppDev
   $ wget https://github.com/BxCppDev/Bayeux/archive/Bayeux-3.5.4.tar.gz
   $ mkdir Bayeux-3.5.4 && \
       tar xvzf Bayeux-3.5.4.tar.gz -C Bayeux-3.5.4 --strip-components=1
   $ cd Bayeux-3.5.4/
..


The  ``${HOME}/BxCppDev/Bayeux-3.5.4/``   source  directory   is  thus
created.

You  can  now create  a  dedicated  directory  to build  Bayeux  3.5.4
following  the guidelines  in the  *Installing Bayeux*  section below.
Note that  different versions  of Bayeux  may have  slightly different
build/installation  procedures,  so  you  should  read  carefully  the
``README.rst`` file supplied with the source code.


If you  want to use  the development version (possibly  unstable), please use
Git:

.. code:: sh

   $ cd ${HOME}
   $ mkdir -p ${HOME}/BxCppDev
   $ cd ${HOME}/BxCppDev
   $ git clone https://github.com/BxCppDev/Bayeux Bayeux.git
   $ cd Bayeux.git
   $ git checkout develop
..

.. raw:: pdf

   PageBreak oneColumn

Preparing Your System for Bayeux Installation
==============================================

To install Bayeux, you first need the following software:

Prerequisite Software
---------------------

UNIX Operating System
.....................

*  Linux
*  macOS 10.9 (Mavericks) or higher

We expect  Bayeux to  work on  most modern BSD  based Unices  or Linux
distributions  derived from  Debian and Fedora (RedHat) provided  the
software listed below is installed. However, we can only fully support
and test the following at present:

- Ubuntu  20.04LTS (current  development  system), 18.04LTS,  16.04LTS
  (deprecated but should work with  upgrade of some software tools) :
  http://www.ubuntu.com
- CentOS 7.5: https://www.centos.org/

If you have problems on systems other than these, please contact us,
though support will be on a best effort basis.

Core Software Required
......................

* CMake 3.10.2 or higher: http://www.cmake.org
  
  * Ubuntu 18.04 provides CMake version 3.10.2.
  * Ubuntu 20.04 provides CMake version 3.16.3.

* C/C++ compiler supporting at least C++11 standard
  (GNU/Clang/Intel)
  
  * Ubuntu 18.04 provides GCC version 6.5 and 7.3.
  * Ubuntu 20.04 provides GCC version 9.3.
  * Bayeux is known to work on CentOS with GCC 4.9
  
On Linux,  you should  install these through  the package  manager for
your distribution. Some older  Linux systems (SL/CentOS, especially on
institutional computing clusters)  may not provide CMake  3.3. If this
is the  case, then you should  download the latest Linux  *binary .sh*
file (example: ``cmake-3.17.2-Linux-x86_64.sh``) from:

  http://www.cmake.org/cmake/resources/software.html

and follow the  instructions on that page to install  it. You can also
install CMake from source.

On macOS, simply install CMake from the latest ``Darwin64`` dmg
bundle available from:

  http://www.cmake.org/cmake/resources/software.html

To obtain the other tools, simply  install the latest version of Xcode
from the  Mac App Store.  After installation, you should  also install
the Xcode command line tools via going to the menu Xcode->Preferences,
clicking on the Downloads tab, and then installing Command Line Tools.

Core Libraries Required
.......................


It is  possible to use system  installation of the below  libraries if
your OS distribution provides  adequate support. Some software package
managers  (Linuxbrew,  Spack) may  help  too.


.. https://github.com/BxCppDev/BxInstallers

.. _`Bayeux-3.5.4`: https://github.com/BxCppDev/Bayeux/releases/tag/Bayeux-3.5.4

We list below the third party softwares required by Bayeux. If some of
them are not available from the  system package manager, you will have
to install them manually.


* Boost  1.69.0, 1.71.0 (tough 1.70 and 1.72+ should work):  http://www.boost.org  with the  following
  libraries:    filesystem,    system,    serialization,    iostreams,
  program_options, regex and thread.

  * Ubuntu 18.04  provides version 1.65.0  which is broken  for Bayeux
    (bug in the Boost/serialization library) so you'll need to install
    Boost >= 1.69 by yourself
  * Ubuntu 20.04 provides version 1.71.0 (libboost-all-dev)
  * Ubuntu 22.04 provides version 1.74.0 (libboost-all-dev)

  **Beware**: Boost  versions 1.65 (default  on Ubuntu 18.04)  to 1.68
  are  expected to  cause some  crash with  GCC under  Linux due  to a
  subtle bug concerning the Boost/Serialization singleton class. Boost
  1.69 should fix this issue.
  
* Camp >=0.8.0 (or 0.8.4 recommended) : https://github.com/IRCAD-IHU/camp
  
  * Ubuntu 18.04 does not provide Camp
  * Ubuntu 20/22.04 provide version 0.8.4 (libcamp-dev)

* GSL 2.4 or higher: http://www.gnu.org/s/gsl

  * Ubuntu 18.04 provides version 2.4 (libgsl-dev)
  * Ubuntu 20.04 provides version 2.5 (libgsl-dev)
  * Ubuntu 22.04 provides version 2.7.1 (libgsl-dev)
  
* CLHEP 2.1.3.1, 2.1.4.2 (recommended), 2.4.1.0: http://proj-clhep.web.cern.ch
  
  * Ubuntu 18.04 does not provide CLHEP
  * Ubuntu 20.04 provides   version   2.1.4.1   (libclhep-dev)   but
    unfortunately  this  version has  no  CMake  support so  you  should
    install this library by yourself. We thus recommend version 2.1.4.2.
  * Ubuntu 22.04 (not documented yet)
    
* Geant4 9.6 (optional) : http://geant4.cern.ch
  with GDML support enabled (through the XercesC library)

  **Warning:** Geant4 version 11 support is in preparation (issue #43).

  You must  install Geant4  and its  associated datasets  by yourself.
  Please do not build Geant4 with  internal CLHEP but the CLHEP system
  or manual install recommended above because Bayeux itself uses CLHEP
  even if the Geant4 extension library is not used.

* Xerces-C (optional, needed for GDML support and Geant4 bridge)
 
  * Ubuntu 20.04 provides version 3.2.2 (libxerces-c-dev)
  * Ubuntu 22.04 provides version 3.2.3 (libxerces-c-dev)

* ROOT 6.08.00, 6.12.04 or 6.16.00 (recommended): http://root.cern.ch
  Bayeux/geomtools requires you setup ROOT at least with support for:

  * minimal X11,
  * GDML,
  * OpenGL.

  You must install ROOT by yourself.

  On Ubuntu 18.04  or 20.04, we have identified the  following list of
  packages to be installed to satisfy ROOT dependencies (this list may
  be  incomplete): g++,  gcc,  binutils, libfreetype6-dev,  libgsl-dev
  gsl-bin,  libbz2-dev  ,  zlib1g-dev,  libreadline-dev,  libxml2-dev,
  libsqlite3-dev,  libssl-dev\  libx11-dev,  libxext-dev,  libxft-dev,
  libxpm-dev, libpng-dev,  libjpeg-dev, libgif-dev, libafterimage-dev,
  libtiff5-dev,      liblzma-dev,       liblz4-dev,      libfftw3-dev,
  libgraphviz-dev, libftgl-dev, libglew-dev, libpcre3-dev.

* Qt5 (optional)

  * Ubuntu 20.04 provides version 5.12.8 (libqt5core5a, libqt5gui5, libqt5widgets5,
    qt5-default, qtbase5-dev, qtbase5-dev-tools, libqt5svg5-dev)
  * Ubuntu 22.04 provides version 5.15.3
    
* BxDecay0 (>=1.1.0) : https://github.com/BxCppDev/bxdecay0

  You must  install BxDecay0 by  yourself. BxDecay0 depends on  GSL so
  you should  synchronize this dependency from  the Bayeux's dependency on
  GSL.

  **Remark**  : The  BxDecay0 C++  port of  the legacy  Fortran decay0
  program is now an independant  project which has been extracted from
  the ``Bayeux/genbb_help``  legacy module.  BxDecay0 is  now the only
  implementation of the Decay0 C++ port supported by Bayeux.

Install dependencies with BxInstallers
..........................................

We provide  a collection  of semi-automated installation  scripts from
the BxInstallers_ project. ``BxInstallers`` is mostly dedicated to the
Ubuntu  20/22.04  system. It  allows  to  build,  install and  setup  the
software dependency stack for Bayeux.

.. _BxInstallers: https://github.com/BxCppDev/BxInstallers


Install dependencies with LinuxBrew
...................................

We have  experienced that  the use  of Linuxbrew  is not  a definitive
robust solution  to solve the software  dependency problem.  Linuxbrew
regularly fails to provide a proper and stable environment to host and
use  Bayeux,  due   to  rapidly  changing  brew   formulas  and  their
dependencies from the homebrew core tap.  We have tried to provide the
proper formulas for Ubuntu Linux. However  we have decided to stop the
support of this tap (`bxcppdev/bxtap`_).

Note however that it is  perfectly possible to use system installation
of  some of  the  above  libraries if  your  OS distribution  provides
adequate support.

Install dependencies with Spack
...................................

Work under progress.


Additional Software Required
............................

* Bayeux/datatools requires the Qt5 library when the ``BAYEUX_WITH_QT_GUI``
  option is set (experimental).

  On  Ubuntu  18.04/20.04,  this   implies  the  installation  of  the
  following packages:

  .. code:: sh

     $ sudo apt-get install libqt5core5a libqt5gui5 libqt5svg5 \
	    libqt5svg5-dev libqt5widgets5  \
	    qt5-default
  ..
  
* Bayeux/geomtools also requires Gnuplot 4.0 or higher: http://www.gnuplot.info

  On Ubuntu 18.04/20.04, this implies the installation of the following packages:

  .. code:: sh

     $ sudo apt-get install gnuplot 
  ..

  Gnuplot uses by  default the ``gnuplot-qt`` interface.  You may want
  to use the ``gnuplot-x11`` package.

* Bayeux/datatools and Bayeux/geomtools uses the Readline library, if available:

  * http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html
  * http://askubuntu.com/questions/194523/how-do-i-install-gnu-readline

  On Ubuntu 20.04, this implies the installation of the following packages:

  .. code:: sh

     $ sudo apt-get install libreadline-dev
  ..

* Doxygen is  used to generate some
  documentation:

  On Ubuntu, this implies  the installation of the following
  packages:

  .. code:: sh

     $ sudo apt-get install doxygen
  ..

* pandoc (http://johnmacfarlane.net/pandoc/) is  useful to generate
  documentation in user friendly format (optional):

  On Ubuntu, this implies  the installation of the following
  packages:

  .. code:: sh

     $ sudo apt-get install pandoc
  ..

* docutils  (http://docutils.sourceforge.net/)  is also  useful  to
  generate documentation from ReST format in user friendly format (optional):

  On Ubuntu 20.04, this implies the installation of the following packages:

  .. code:: sh

     $ sudo apt-get install docutils-common python-docutils
     $ sudo apt-get install rst2pdf
  ..

Known Issues on Tested Platforms
--------------------------------


- Boost/Serialization library  from version 1.65 to  1.68 introduced a
  bug in  the implementation  of the singleton  template class.   As a
  consequence, only Boost 1.69 and above should be supported so far.
- Despite our efforts, Geant4 10.5 (no  MT build) is not supported yet
  since  the implementation  of hit  collections has  changed in  some
  undocumented way (as usual with  Geant4!) and now causes segfault in
  the Bayeux/mctools Geant4 extension module.
  
.. raw:: pdf

   PageBreak oneColumn

Installing Bayeux
=================

Bayeux provides a CMake_ based  build system. We'll assume for brevity
that you are using  a UNIX system on the command  line (i.e.  macOS or
Linux).  We'll also  assume  you have already setup all third party software
Bayeux depends on using the tool of your choice.

.. We'll also  assume that  you're going  to use  the Linuxbrew
   `bxcppdev/bxtap`_ tap to provide some required third party packages.

.. _`bxcppdev/bxtap`: https://github.com/BxCppDev/homebrew_bxtap
.. _CMake: http://www.cmake.org

Configuring the Installation
----------------------------
   
The directory in which this  ``README.rst`` file resides is called the
"source directory"  of Bayeux. Because  CMake generates many  files as
part of the configuration and  build process, we perform configuration
in a directory isolated from the  source directory. This enables us to
quickly clean  up in  the event  of issues,  and prevents  commital of
generated (and hence system dependent) files to the repository.

To configure  Bayeux under  Ubuntu 20.04, simply  do, from  the source
directory of Bayeux:

.. code:: sh

   $ mkdir Bayeux-build
   $ cd Bayeux-build/
   $ cmake \
	  -DCMAKE_INSTALL_PREFIX="${HOME}/Bayeux-install" \
	  -DBAYEUX_WITH_QT_GUI=ON \
	  -DROOT_DIR="`root-config --prefix`/share/root/cmake" \
          -DBxDecay0_DIR=`bxdecay0-config --cmakedir` \
	  -DBAYEUX_WITH_GEANT4_MODULE=ON \
	  -DGeant4_DIR="`geant4-config --prefix`" \
	  ..
..

You may also  use an arbitrary temporary build directory somewhere in
your filesystem:

.. code:: sh

   $ mkdir /tmp/Bayeux-build
   $ cd /tmp/Bayeux-build
   $ cmake \
	  -DCMAKE_INSTALL_PREFIX="${HOME}/Bayeux-install" \
	  -DBAYEUX_WITH_QT_GUI=ON \
	  -DROOT_DIR="`root-config --prefix`/share/root/cmake" \
          -DBxDecay0_DIR=`bxdecay0-config --cmakedir` \
	  -DBAYEUX_WITH_GEANT4_MODULE=ON \
	  -DGeant4_DIR="`geant4-config --prefix`" \
	  <path to the Bayeux source directory>
..

CMake Configuration Options
---------------------------

These options control the underlying CMake system, a full list can be
obtained from the CMake documentation, but in Bayeux you will only need
to deal with the following three in most cases:

``CMAKE_INSTALL_PREFIX``
  Path under which to install Bayeux. It should point to an empty,
  writable directory. It defaults to ``/usr/local`` so you will want
  to change this.

``CMAKE_BUILD_TYPE``
  Build type, e.g. ``Release``, ``Debug``. You will want this to be
  set  to ``Release``  in most  cases. ``Debug``  builds are  only
  needed if you  are needing to follow debugging  symbols into one
  of   Linuxbrew's  thid   party  binaries.    It  defaults   to
  ``Release``, so you will not need to change it in most cases.

``CMAKE_PREFIX_PATH``
  Path under which  Linuxbrew is installed and where  some of the
  third party software (dependencies) should be searched for.
  You can use the following to automatically locate Linuxbrew on your system:

  .. code:: sh

     $ cmake -DCMAKE_PREFIX_PATH=$(brew --prefix)
  ..

Note also  that you can  ask CMake to use  the Ninja_ build  system in
place of the legacy ``make`` command. Use the ``-GNinja`` switch with your
CMake command:

.. code:: sh

   $ cmake ... -GNinja ...
..

.. _Ninja: https://ninja-build.org/


.. raw:: pdf

   PageBreak oneColumn

Bayeux Configuration Options
----------------------------

These options control the core configuration of Bayeux.

``BAYEUX_CXX_STANDARD``
  Select the C++  Standard to compile against. Recognized values are:

     * ``11`` (default) : all features of the C++11 standard in GCC 4.9 (provided
       for forward compatibility)
     * ``14``  :  same  as  ``11``  plus at  least  one  C++14  feature
       (provided for forward compatibility)
     * ``17``  :  same  as  ``14``  plus at  least  one  C++17  feature
       (provided for forward compatibility)
     * ``20``  :  same  as  ``17``  plus at  least  one  C++20  feature
       (provided for forward compatibility)

``BAYEUX_COMPILER_ERROR_ON_WARNING``
  Turn warnings into errors. Default is ON.

``BAYEUX_WITH_IWYU_CHECK``
  Run include-what-you-use on Bayeux sources. Default is OFF.

``BAYEUX_WITH_DEVELOPER_TOOLS``
  Build and install additional tools for developers and *normal* users.
  Default is ON.

``BAYEUX_WITH_GEANT4_MODULE``
  Build the Bayeux/mctools Geant4 library extension module. Default is ON.

``BAYEUX_WITH_GEANT4_EXPERIMENTAL``
  Build the Bayeux/mctools Geant4 library extension module with
  experimental Geant4 support (>=10.5, experts only). Default is OFF.

``BAYEUX_WITH_QT_GUI``
  Build the Qt-based GUI basic components (experimental). Default is OFF.

.. removed : ``BAYEUX_WITH_QT_SVG``
..   Build the specific Qt SVG component. Default is OFF.

``BAYEUX_ENABLE_TESTING``
  Build unit testing system for Bayeux. Default is OFF.

``BAYEUX_WITH_DOCS``
  Build Bayeux documentation products. Default is ON.

``BAYEUX_WITH_DOCS_OCD``
  Build      *object      configuration     description*      (OCD)
  documentation. Default is OFF. Implies ``BAYEUX_WITH_DOCS``.

``BAYEUX_MINIMAL_BUILD``
  Build Bayeux core library only (datatools module). Default is OFF (experts only).

``BAYEUX_WITH_xxx``
  Build Bayeux library including up to the ``xxx`` module ( taking into account dependencies),
  where ``xxx`` is taken from:

  - ``CUTS``
  - ``MATERIALS``
  - ``MYGSL``
  - ``BRIO``
  - ``DPP``
  - ``GEOMTOOLS``
  - ``EMFIELD``
  - ``GENBB``
  - ``GENVTX``
  - ``MCTOOLS``

  Default is ON (forcing OFF is for experts only).

Dependency Options
----------------------------

``BOOST_ROOT``, ``Boost_ADDITIONAL_VERSIONS`` :
  Set the directory where Boost is installed and optionally allow a unsupported newer version of Boost
  (may not work).
 
  Example:

  .. code:: sh
 
     $ cmake ... -DBOOST_ROOT="/usr" -DBoost_ADDITIONAL_VERSIONS=1.74 ...
  ..  
 
``CAMP_DIR`` :
  Set the directory where CAMP's Cmake support is available.
 
  Example:

  .. code:: sh
 
     $ cmake ... -DCAMP_DIR="/usr/lib/camp/cmake" ...
  ..  
 

``CLHEP_ROOT_DIR`` :
  Set the directory where CLHEP header and library files are installed.
 
  Example:

  .. code:: sh

     $ cmake ... -DCLHEP_ROOT_DIR=$(clhep-config --prefix | tr -d '"') ...
  ..  

 
``BxDecay0_DIR`` :
   Set the directory where BxDecay0's CMake support is available.

  Example:

  .. code:: sh

     $ cmake ...  -DBxDecay0_DIR="$(bxdecay0-config --cmakedir)" ...
  ..  
 
 
``Qt5Core_DIR``, ``Qt5Gui_DIR``, ``Qt5Widgets_DIR``, ``Qt5Svg_DIR``:
   Set the directories where Qt5 libraries' CMake support is available.

  Example:

  .. code:: sh

     $ cmake ... -DBAYEUX_WITH_QT_GUI=ON \
                 -DQt5Core_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5Core" \ 
                 -DQt5Gui_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5Gui" \ 
                 -DQt5Widgets_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5Widgets" \ 
                 -DQt5Svg_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5Svg" ...
  ..  

``ROOT_DIR`` :
  Set the directory where ROOT's Cmake support is available.
 
  Example:

  .. code:: sh
 
     $ cmake ... -DROOT_DIR="$(root-config --prefix)/share/root/cmake" ...
  ..  
 

``Geant4_DIR`` :
  Set the directory where Geant4's Cmake support is available.
  
  Example:

  .. code:: sh
 
     $ cmake ... \
        -DBAYEUX_WITH_GEANT4_MODULE=ON \
        -DGeant4_DIR"$(geant4-config --prefix)/lib/Geant4-$(geant4-config --version | cut -d' ' -f2)" ...
  ..  
 
Building and Installing
-----------------------

Once  you have  generated the  build system  for Bayeux,  as described
earlier, you are ready to build.  Note that if you want to reconfigure
at  any  time, you  can  simply  run  ``ccmake``  again in  the  build
directory.

By default Bayeux  generates a Makefile based system, so  to build and
install Bayeux, simply run:

.. code:: sh

   $ make [-j4]
   $ make install
..

where ``-j4`` indicates  the number of processors to be  used to build
Bayeux.

If you  chose Ninja as the  build system, please replace  the ``make``
command above by ``ninja`` :

.. code:: sh

   $ ninja [-j4]
   $ ninja install
..


.. raw:: pdf

   PageBreak oneColumn

   
Running test
------------

In order  to run the  test programs  provided with the  various Bayeux
submodules,  you should  have activated  the ``BAYEUX_ENABLE_TESTING``
configuration option. From the build directory, simply run:

.. code:: sh

   $ make test
..

or

.. code:: sh

   $ ninja test
..


.. raw:: pdf

   PageBreak oneColumn
..


Setup
-----

In order  to setup Bayeux  on your system,  we recommend to  provide a
bayeux  activation  shell  function  from  your  Bash  startup  script
(typically ``~/.bashrc``):

.. code:: sh
 
   function do_bayeux_setup()
   {
      local _bayeux_install_dir="/path/to/bayeux/installation/dir"
      if [ -n "${BAYEUX_INSTALL_DIR}" ]; then
	  echo >&2 "[error] do_bayeux_setup: Bayeux is already setup!"
	  return 2
      fi     
      export BAYEUX_INSTALL_DIR="${_bayeux_install_dir}"
      export PATH="${BAYEUX_INSTALL_DIR}/bin:${PATH}"
      echo >&2 "[info] do_bayeux_setup: Bayeux $(bxquery --version) is now setup!"
      return 0
   }
   export -f do_bayeux_setup
   alias bayeux_setup='do_bayeux_setup'
..

When you need to use the Bayeux  software from a bare Bash shell, just
type:

.. code:: sh

   $ bayeux_setup
..

The  ``bxquery``  utility should  help  you  to locate  the  resources
provided by Bayeux:

.. code:: sh

   $ bxquery --help
..

Fell free  to provide a  ``do_bayeux_unsetup`` shell function  to come
back to the  initial state of the shell or  simply terminate the shell
when you are done with Bayeux.


Documentation
===============

Bayeux is built with *some* documentation, although incomplete:

* From the  installation directory, provided  the ``BAYEUX_WITH_DOCS``
  and ``BAYEUX_WITH_DOCS_OCD`` options have been enabled, one can find
  a set of  Doxygen generated HTML pages. The main  page is located in
  ``share/Bayeux-{Bayeux's
  version}/Documentation/API/html/index.html``   from   the   Bayeux's
  installation directory.
* The Bayeux  source code  provides some test  programs that  *may* be
  used as sample code. However, it is not their original purpose.
* Bayeux  modules  contains some  example  code  implemented as  small
  projects. See the source code for example in
  ``share/Bayeux-{Bayeux's version}/examples/``
  from the Bayeux's installation directory.
  
Troubleshooting
===============
WIP


Using Bayeux
============


A note on RPATHs
----------------

You should not use  the ``(DY)LD_LIBRARY_PATH`` variables because they
are  intended  for testing,  not  production  (see  the man  pages  of
ld/dyld).   Bayeux uses  **rpaths**  to provide  a  simple setup  that
allows  applications  to  be  run  directly  with  guaranteed  library
lookup. Morever, relative rpaths are  used that generally allow Bayeux
to be relocatable (albeit not tested).

However, these settings are platform dependent and CMake has only added
support for this gradually. In particular, see these references:

* Kitware Blog article on macOS RPATH handling (http://www.kitware.com/blog/home/post/510)
* Handling macOS RPATH on older CMake

  (http://www.mail-archive.com/cmake@cmake.org/msg47143.html)
* CMake's general RPATH handling (http://www.cmake.org/Wiki/CMake_RPATH_handling)

Note also  that if you  have ``(DY)LD_LIBRARY_PATH`` set, you  may see
startup errors if any of the  paths contains libraries used by Bayeux,
e.g. ROOT.  In general, you should never need to set the library path,
though many scientific software projects (badly mis)use it.


To do
=====

* Provide official example code for many classes.
* Migrate some  Boost classes to some  C++11 classes (smart
  pointers...).

.. raw:: pdf

   PageBreak oneColumn
..


Bayeux development group
========================

Current development staff:

* François Mauger (LPC Caen, Université de Caen, Normandie Université,
  project leader): all modules.

Other contributors:

* Ben Morgan (University of  Warwick): CMake support, logging features
  in datatools, other management  and integration tools, Doxygen based
  documentation support, Trac/SVN to GitHub migration.
* Jean    Hommet   (LPC    Caen):   initial    development   of    the
  Boost/Serialization features.
* Yves Lemière  (LPC Caen, Université de  Caen, Normandie Université):
  validation.
* Xavier  Garrido   (LAL  Orsay,  Université  Paris   Sud,  Université
  Paris-Saclay): all modules, validation.
* Guillaume  Oliviéro   (LPC  Caen,  Université  de   Caen,  Normandie
  Université): validation
* Arnaud Chapon (LPC Caen, Cerap): geometry, validation.
* Benoit Guillon (LPC Caen,  ENSICAEN): original implementation of the
  ``Bayeux/materials`` module.


Acknowledgements
================

The authors gratefully thank the following persons for their direct or
indirect contributions to the Bayeux library:

* Volodymyr  Tretyak  is  the  author of  the  original  *Genbb/Decay0*
  generator (written in  Fortran 77) from which  a significant portion
  of the Bayeux/genbb_help module is derived.
* Christian Pfligersdorffer  is the author of  the Boost/Serialization
  *portable  binary archive*  classes which  is supported  by the  I/O
  system of the Bayeux/datatools and Bayeux/brio modules.
* Nicolas Devillard and Rajarshi Guha  are the authors of the *Gnuplot
  pipe* library that is embedded in Bayeux/geomtools.
* Sylvette Lemagnen (Curator at the  Bayeux Museum) and Patrick Gomont
  (Mayor  of the  City  of  Bayeux) for  their  authorization for  the
  library's name and logo.

  Visit the Bayeux Tapestry at http://www.bayeuxmuseum.com/en/la_tapisserie_de_bayeux_en.html !

  .. image:: source/bxbayeux/logo/bayeux_tapestry_slice-1-small.png
     :align: center
     :width: 100%

.. end
