================================================================
Bayeux C++ Library for Experimental Particle and Nuclear Physics
================================================================

.. image:: source/bxbayeux/logo/logo_bayeux_240x200_transparent.png
   :width: 200pt

:Authors: The BxCppDev_ group
:Date:    2017-10-20
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

genbb_help
  C++ port and  extensions to the Genbb/Decay0 program by
  Vladimir Tretyak, providing input to simulation applications.

genvtx
  Vertex  random  generator tools providing input to  simulation
  applications.

mctools
  Utilities for particle and nuclear physics simulation with
  an optional Geant4 interface.

lahague
  Utilities for dosimetry and radiation protection.


.. raw:: pdf

   PageBreak oneColumn

Bayeux's roots
--------------

Original work on Bayeux was  initiated at the `Laboratoire de Physique
Corpusculaire de  Caen`_ (CNRS/IN2P3)  in the  framework of  the NEMO3
double-beta  decay  experiment and  the  R&D  for its  successor:  the
SuperNEMO experiment.  Bayeux  is developped by a  group of physicists
working in Nuclear  and Particle Physics. It  provides generic classes
and tools that can be used in  many different contexts.  It is now the
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
(UMR6534,  CNRS/IN2P3,  ENSICAEN, Normandie Université).

.. _`Laboratoire de Physique Corpusculaire de Caen`: http://www.lpc-caen.in2p3.fr/


Licensing
---------

Please study the  file ``LICENSE.txt`` for the  distribution terms and
conditions of use of Bayeux.

Bayeux contains some code extracted  from :

* the *Kitware System* library (OSI-approved BSD License),
* the *BinReloc* library (Do  What The Fuck You Want To Public License, Version  2),
* the *EOS portable archive* library (MIT License).

Vladimir Tretyak has kindly accepted that the C++ wrapper and C++ port
of the  original Genbb/Decay0 Fortran  library is published  under the
GPL 3 within Bayeux.

Some parts of code or  management/build scripts are released under the
MIT License.

Most of Bayeux's C++ source code is  released under the GNU General Public
License 3.0.


Supported environments
----------------------

Bayeux  is expected  to work  on modern  Linux distributions  based on
Debian  or Fedora,  as Linux  is our  main development  and production
environment.  Other  UNIX-like OSes  (Linux, BSD, macOS)  **may** work
with some work of adapting.

Bayeux now uses the C++11 standard  by default so this implies the use
of a modern C++ compiler.

Using  the `bxcppdev/bxtap`_  `Linuxbrew`_ *tap*  provided by  the
BxCppDev_  group  should help  you  to  guarantee a  suitable  working
environment on your system.

Releases
--------

* Latest release: Bayeux-3.1.0



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

For example, to download Bayeux version 3.1.0, you may use, assuming a
Linux system:

.. code:: sh

   $ cd ${HOME}
   $ mkdir -p ${HOME}/BxCppDev
   $ cd ${HOME}/BxCppDev
   $ wget https://github.com/BxCppDev/Bayeux/archive/Bayeux-3.1.0.tar.gz
   $ mkdir Bayeux-3.1.0 && tar xvzf Bayeux-3.1.0.tar.gz -C Bayeux-3.1.0 --strip-components 1
   $ cd Bayeux-3.1.0/
..


The  ``${HOME}/BxCppDev/Bayeux-3.1.0/``   source  directory   is  thus
created.

You  can  now create  a  dedicated  directory  to build  Bayeux  3.1.0
following  the guidelines  in the  *Installing Bayeux*  section below.
Note that  different versions  of Bayeux  may have  slightly different
build/installation  procedures,  so  you  should  read  carefully  the
``README.rst`` file supplied with the source code.


If you  want to use  the development version (possibly  unstable), use
Git:

.. code:: sh

   $ cd ${HOME}
   $ mkdir -p ${HOME}/BxCppDev
   $ cd ${HOME}/BxCppDev
   $ git clone https://github.com/BxCppDev/Bayeux Bayeux.git
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
distributions  derived from  Fedora(RedHat)  and  Debian provided  the
software listed below is installed. However, we can only fully support
and test the following at present:

-  Ubuntu 12.04LTS, 14.04LTS, 16.04LTS (current development system):
   http://www.ubuntu.com
-  Scientific Linux 5/6.X/7.X: http://www.scientificlinux.org

If you have problems on systems other than these, please contact us,
though support will be on a best effort basis.

Core Software Required
......................

* CMake 3.3.0 or higher: http://www.cmake.org
* C/C++ compiler supporting at least C++11 standard
  (GNU/Clang/Intel)

On Linux,  you should  install these through  the package  manager for
your distribution. Some older  Linux systems (SL/CentOS, especially on
clusters) may  not provide CMake  3.3. If this  is the case,  then you
should download the latest Linux *binary .sh* file from:

  http://www.cmake.org/cmake/resources/software.html

and follow the instructions on that page to install it.

On macOS, simply install CMake from the latest ``Darwin64`` dmg
bundle available from:

  http://www.cmake.org/cmake/resources/software.html

To obtain the other tools, simply  install the latest version of Xcode
from the  Mac App Store.  After installation, you should  also install
the Xcode command line tools via going to the menu Xcode->Preferences,
clicking on the Downloads tab, and then installing Command Line Tools.

Core Libraries Required
.......................

* Boost 1.63.0 or higher: http://www.boost.org
  with filesystem, system, serialization, iostreams, program_options, regex
  and thread libraries.
* Camp 0.8.0 : https://github.com/tegesoft/camp
* GSL 2.4 or higher: http://www.gnu.org/s/gsl
* CLHEP 2.1.3.1 or higher: http://proj-clhep.web.cern.ch
* Geant4 9.6.0 or higher: http://geant4.cern.ch
  with GDML support enabled (through the XercesC library)
* ROOT 6.08: http://root.cern.ch
  Bayeux/geomtools requires you setup ROOT at least with support for:

  * minimal X11,
  * GDML,
  * OpenGL.


For ease  of use,  the BxCppDev  group provides  the `bxcppdev/bxtap`_
Linuxbrew tap  for easy use  by Bayeux, Bayeux companion  software and
clients of Bayeux.  It is **strongly** advised to use this bundle.  It
will  provide,  for  Linux  and macOS  systems,  an  uniform  software
environment with  a selected set  of blessed software,  including the
C++ compiler if needed.

Additional Software Required
............................

* Bayeux/datatools requires the Qt5 library when the ``BAYEUX_WITH_QT_GUI``
  option is set (experimental).

  On Ubuntu 16.04, this implies the installation of the following packages:

  .. code:: sh

     $ sudo apt-get install libqt5core5a libqt5gui5 libqt5svg5 \
	    libqt5svg5-dev libqt5widgets5 qtbase5-dev qtbase5-dev-tools \
	    qt5-default
  ..

* Bayeux/geomtools also requires Gnuplot 4.0 or higher: http://www.gnuplot.info

  On Ubuntu 16.04, this implies the installation of the following packages:

  .. code:: sh

     $ sudo apt-get install gnuplot-x11 gnuplot-doc gnuplot-mode
  ..

* Bayeux/datatools and Bayeux/geomtools uses the Readline library, if available:

  * http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html
  * http://askubuntu.com/questions/194523/how-do-i-install-gnu-readline

  On Ubuntu 16.04, this implies the installation of the following packages:

  .. code:: sh

     $ sudo apt-get install libreadline6-dev readline-common
  ..

* pandoc (http://johnmacfarlane.net/pandoc/) is  useful to generate
  documentation in user friendly format:

  On Ubuntu 16.04, this implies  the installation of the following
  packages:

  .. code:: sh

     $ sudo apt-get install pandoc pandoc-data
  ..

* docutils  (http://docutils.sourceforge.net/)  is also  useful  to
  generate documentation from ReST format in user friendly format:

  On Ubuntu 16.04, this implies the installation of the following packages:

  .. code:: sh

     $ sudo apt-get install docutils-common docutils-doc python-docutils
     $ sudo apt-get install rst2pdf
  ..

Known Issues on Tested Platforms
--------------------------------
None known at present.


.. raw:: pdf

   PageBreak oneColumn

Installing Bayeux
=================

Bayeux provides a CMake_ based  build system. We'll assume for brevity
that you are using  a UNIX system on the command  line (i.e.  macOS or
Linux).   We'll also  assume that  you're going  to use  the Linuxbrew
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

To configure Bayeux, simply do, from the source directory of Bayeux:

.. code:: sh

   $ mkdir Bayeux-build
   $ cd Bayeux-build/
   $ cmake -DCMAKE_INSTALL_PREFIX=<where you want to install> \
	   -DCMAKE_PREFIX_PATH=<path to your Linuxbrew install> \
	   ..
..

You may also  use an arbitrary temporary build  directory somewhere in
your filesystem:

.. code:: sh

   $ mkdir /tmp/Bayeux-build
   $ cd /tmp/Bayeux-build
   $ cmake -DCMAKE_INSTALL_PREFIX=<where you want to install> \
	   -DCMAKE_PREFIX_PATH=<path to your Linuxbrew install> \
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

``CMAKE_PREFIX_PATH``
  Path under which  Linuxbrew is installed and where  some of the
  third party software (dependencies) should be searched for.
  You can use the following to automatically locate Linuxbrew on your system:

  .. code:: sh

     $ cmake -DCMAKE_PREFIX_PATH=$(brew --prefix)


``CMAKE_BUILD_TYPE``
  Build type, e.g. ``Release``, ``Debug``. You will want this to be
  set  to ``Release``  in most  cases. ``Debug``  builds are  only
  needed if you  are needing to follow debugging  symbols into one
  of   Linuxbrew's  thid   party  binaries.    It  defaults   to
  ``Release``, so you will not need to change it in most cases.

Note also  that you can  ask CMake to use  the Ninja_ build  system in
place of the legacy make command. Use the ``-GNinja`` switch with your
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

``BAYEUX_COMPILER_ERROR_ON_WARNING``
  Turn warnings into errors. Default is ON.

``BAYEUX_WITH_IWYU_CHECK``
  Run include-what-you-use on Bayeux sources. Default is OFF.

``BAYEUX_WITH_DEVELOPER_TOOLS``
  Build and install additional tools for developers and *normal* users.
  Default is ON.

``BAYEUX_WITH_GEANT4_MODULE``
  Build the Bayeux/mctools Geant4 library extension module. Default is ON.

``BAYEUX_WITH_MCNP_MODULE``
  Build the Bayeux/mctools MCNP library extension module (experimental). Default is OFF.

``BAYEUX_WITH_LAHAGUE``
  Build the Bayeux/lahague library module. Default is OFF.

``BAYEUX_WITH_QT_GUI``
  Build the Qt-based GUI components (experimental). Default is OFF.

``BAYEUX_ENABLE_TESTING``
  Build unit testing system for Bayeux. Default is OFF.

``BAYEUX_WITH_DOCS``
  Build Bayeux documentation products. Default is ON.

``BAYEUX_WITH_DOCS_OCD``
  Build      *object      configuration     description*      (OCD)
  documentation. Default is OFF. Implies ``BAYEUX_WITH_DOCS``.


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


.. raw:: pdf

   PageBreak oneColumn
..


Documentation
===============
WIP


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

* Migrate some deprecated Boost classes to some C++11 classes (smart pointers...)
* Implement support for radioactive decays  using ENSDF files from Geant4 in
  the Bayeux/genbb_help module.
* Implement  the  Bayeux/mctools  MCNP extension  library  module  and
  companion tools.


.. raw:: pdf

   PageBreak oneColumn
..


Bayeux development group
========================

Current development staff:

* Xavier Garrido (LAL Orsay, Université Paris Sud, Université Paris-Saclay): all modules, validation.
* Jean Hommet (LPC Caen): initial development of the Boost/Serialization features.
* Yves Lemière (LPC Caen, Université de Caen, Normandie Université): validation.
* François Mauger (LPC Caen, Université de Caen, Normandie Université, project leader): all modules.
* Guillaume Oliviéro (LPC Caen, Université de Caen, Normandie Université): validation

Other contributors:

* Arnaud Chapon (LPC Caen, Cerap): geometry, validation.
* Benoit Guillon (LPC Caen, ENSICAEN): original implementation of the ``Bayeux/materials`` module.
* Ben Morgan (University of Warwick): CMake support, logging features in datatools,
  other management and integration tools, Doxygen based documentation support,
  Trac/SVN to GitHub migration.


Acknowledgements
================

The authors gratefully thank the following persons for their direct or
indirect contributions to the Bayeux library:

* Vladimir  Tretyak  is  the  author of  the  original  *Genbb/Decay0*
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
