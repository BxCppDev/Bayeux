============================================================
Bayeux C++ Library for Experimental Particle/Nuclear Physics
============================================================

:Authors: François Mauger; Xavier Garrido, Ben Morgan
:Date:    Today

.. contents::
   :depth: 3
..

About Bayeux
============

Bayeux provides a collection of C++ classes and functions designed
for the simulation, recording and analysis of data for experimental
particle and nuclear physics projects. This functionality is split
into several specialized submodules:

datatools :  Serializable data structures based on Boost.

brio :       A Boost over ROOT I/O serialization system (extends datatools).

mygsl :      C++ wrapper and extensions to the GNU Scientific Library.

cuts :       Generic tools for making arbitrary data selections.

materials :  Description of isotopes, elements and materials plus tools
	     for input to simulation applications (i.e. GDML/Geant4).

geomtools :  Generic tools for working with experiment geometries and
             provide input to simulation tools (i.e. GDML/Geant4).

genvtx :     Vertex random generator tools for input to
             simulation applications.

genbb_help : C++ wrapper and extensions to the Decay0/GENBB program
             by Vladimir Tretyak, for input to simulation applications.

emfield :    Electromagnetic field modelling and management.

mctools :    Tools and data models for Particle and Nuclear Physics
             Monte-Carlo simulation, wrapper engine for the Geant4 framework.

trackfit :   Track fitting in a NEMO-like detector.


Naming
------
Bayeux is named thus because it weaves together several threads of software
developed in Normandy ;-).

Licensing
---------
Please study the file ``LICENSE.txt`` for the distribution terms and
conditions of use of Bayeux.


Getting Help
============

If you have problems, questions, ideas or suggestions on Bayeux or
any of its submodules, please contact the SuperNEMO Software Working
Group via the main development website https://nemo.lpc-caen.in2p3.fr/.


Getting Bayeux
===============

You can obtain the Bayeux source code from the main LPC Caen Subversion
repository.

Preparing Your System for Cadfael Installation
==============================================

To install Cadfael, you first need the following sofware:

Prerequisite Software
---------------------

-  **UNIX Operating System**:

   -  Linux
   -  Mac OS X 10.7 (Lion) or higher

We expect Cadfael to work on most modern BSD based Unices or Linux
distributions derived from Fedora(RedHat) and Debian provided the
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

On Linux, you should install these through the package manager for your
distribution. Some older Linux systems (SL/CentOS, especially on
clusters) may not provide CMake 2.8. If this is the case, then you
should download the latest Linux *binary .sh* file from:

-  http://www.cmake.org/cmake/resources/software.html

and follow the instructions on that page to install it.

On Mac OS X, simply install CMake from the latest ``Darwin64`` dmg
bundle available from:

-  http://www.cmake.org/cmake/resources/software.html

To obtain the other tools, simply install the latest version of Xcode
from the Mac App Store. After installation, you should also install the
Xcode command line tools via going to the menu Xcode->Preferences,
clicking on the Downloads tab, and then installing Command Line Tools.

-  **Core Libraries Required**

   -  Boost 1.53.0 or higher: http://www.boost.org
       - with serialization, filesystem, system libraries.
   -  Camp 0.7.1 : https://github.com/tegesoft/camp
   -  GSL 1.15 or higher: http://www.gnu.org/s/gsl
   -  CLHEP 2.1.3.2 or higher: http://proj-clhep.web.cern.ch
   -  Geant4 9.6.0 or higher: http://geant4.cern.ch
       - with GDML support enabled
   -  ROOT 5.34.0 or higher: http://root.cern.ch

For ease of use, LPC Caen provide the Cadfael SDK which bundles these
libraries for easy use by Bayeux and clients of Bayeux. You can use this
bundle, or, at your discretion, your own, or system, installs of these
packages.

Known Issues on Tested Platforms
--------------------------------
None known at present.


Installing Bayeux
=================
WIP


Troubleshooting
===============
WIP


Using Bayeux
============
WIP


Developing Bayeux
=================
WIP

