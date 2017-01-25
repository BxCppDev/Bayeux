====================================================================
Bayeux/trunk installation report on (X)Ubuntu 16.04 LTS (64bits)
====================================================================

:Author: François Mauger, LPC Caen <mauger@lpccaen.in2p3.fr>
:Date:   2016-12-06

In  this  document  we  propose  an  installation  procedure  for  the
Bayeux_/trunk  library on  top  of CadfaelBrew_ on  Xubuntu
16.04 LTS (Xenial Xerus) for a 64-bits system.

Notes:

* Cadfaelbrew_ is only supported  on 64-bits systems.
* Two build  systems are supported :  GNU/make and Ninja_, on  top of
  which CMake is used to build Bayeux_.

.. contents::

.. raw:: latex

   \pagebreak

The target system
=================

* Architecture:

.. code:: sh

   $ uname -a
   Linux bayeux-laptop 4.4.0-34-generic #53-Ubuntu SMP Wed Jul 27 16:06:39 UTC 2016 x86_64 x86_64 x86_64 GNU/Linux
..

* Processors:

.. code:: sh

   $ cat /proc/cpuinfo | grep "model name"
   model name   : Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
   model name   : Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
   model name   : Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
   model name   : Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
..

* Linux version:

.. code:: sh

   $ cat /etc/lsb-release
   DISTRIB_ID=Ubuntu
   DISTRIB_RELEASE=16.04
   DISTRIB_CODENAME=xenial
   DISTRIB_DESCRIPTION="Ubuntu 16.04.1 LTS"
..

* Environment:

  The system must have a relatively *bare* environment. It means that even if a lot of software
  has been installed on the system (``/usr/bin``) or in some alternative locations (``/usr/local``,
  ``/opt``...), you should be able to run a shell with a lightweight ``PATH``, typically something
  like:

.. code:: sh

   $ echo $PATH
   /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
..

   In principle, you should not have the ``LD_LIBRARY_PATH`` environmental variable set:

.. code:: sh

   $ echo aaa${LD_LIBRARY_PATH}ZZZ
   aaaZZZ
..

Important notice:
You have to check carefully both environment variables above because it is frequent that some system
administrators use them to setup by default some third party software. The keyword here is
*by default* which means something you didn't ask for. Unfortunately, the excessive/improper
usage of these environs (mostly ``LD_LIBRARY_PATH``) may ends to conflict while building
Cadfael and/or Bayeux.


* Dependencies:

  It is be  mandatory (or maybe useful) to install additional system  packages to properly
  build Bayeux and  activate some of its features.  This is documented
  below.


.. raw:: latex

   \pagebreak

Setup of Cadfaelbrew
===============================

Links:

 * Cadfaelbrew_ repository (GitHub, public access)
 * Cadfael_ (SuperNEMO Wiki, private access)

.. _Cadfael: https://nemo.lpc-caen.in2p3.fr/wiki/Software/Cadfael
.. _Cadfaelbrew: https://github.com/SuperNEMO-DBD/brew


Please follow the instructions on the installation report at https://nemo.lpc-caen.in2p3.fr/browser/Bayeux/trunk/doc/InstallationReports/Cadfaelbrew/Xubuntu16.04-a/tagged/cadfaelbrew_xubuntu16.04_report-1.0.pdf

brew
------------

Once you have installed Cadfaelbrew_, you should be able to run a *brew* session:

.. code:: sh

   $ brewsh      # Enter a *brew shell*
   ...
   $ echo $PATH
   /opt/sw/SuperNEMO-DBD/Cadfaelbrew/bin:/usr/local/sbin:...
..

.. code:: sh

   $ which brew
   /opt/sw/SuperNEMO-DBD/Cadfaelbrew/bin/brew
..

This opens a  new shell with all environmental  variables activated to
setup all the software  tools managed through Cadfaelbrew_ (utilities,
compiler(s), Boost, Root, Geant4...).

You can check the location and version of core software utilities:

.. code:: sh

   $ which cmake
   /opt/sw/SuperNEMO-DBD/Cadfaelbrew/bin/cmake
   $ cmake --version
   cmake version 3.6.1
..

.. code:: sh

   $ which g++
   /usr/bin/g++
   $ g++ --version
   g++ (Ubuntu 5.4.0-6ubuntu1~16.04.1) 5.4.0 20160609
   ...
..

.. code:: sh

   $ which doxygen
   /usr/bin/doxygen
   $ doxygen --version
   1.8.11
..

Ninja
-------------

Ninja_ is  a build  system which  can be used  in place  of (GNU)make.
Install Ninja_ through ``brew`` if it was not already done before (you
must setup the brew environment for that):

.. _Ninja: https://ninja-build.org/

.. code:: sh

   $ brewsh
   $ brew install ninja
   ...
..

Then you can check your Ninja version:

.. code:: sh

   $ which ninja
   /opt/sw/SuperNEMO-DBD/Cadfaelbrew/bin/ninja
   $ ninja --version
   1.7.1
   $ exit
..

.. raw:: latex

   \pagebreak


Qt5
-------------

Qt5 is used for the Qt-based GUI components implemented in Bayeux (optional component).
For now we use the system install of Qt5 (5.5.1 on Ubuntu 16.04):

.. code:: sh

   $ sudo apt-get install  \
	  libqt5core5a \
	  libqt5gui5 \
	  libqt5svg5 \
	  libqt5svg5-dev \
	  libqt5widgets5 \
	  qtbase5-dev \
	  qtbase5-dev-tools \
	  qt5-default

..

Brew is able to install a recent Qt5 (Qt5.6.0) but it seems to be broken
within Bayeux. Please do not use it as long as it is not fixed.

.. .. code:: sh

..   $ brew install qt5-base

..


Configuration and build of Bayeux/trunk
=================================================

Links:

 * Bayeux_ (SuperNEMO Wiki, private access)

.. _Bayeux: https://nemo.lpc-caen.in2p3.fr/wiki/Software/Bayeux

System dependencies
---------------------------

Install dependencies and useful utilities:

.. code:: sh

   $ sudo apt-get install gnuplot gnuplot-doc gnuplot-mode
   $ sudo apt-get install libreadline-dev readline-common
   $ sudo apt-get install pandoc pandoc-data
   $ sudo apt-get install python-docutils rst2pdf
..

See above for Qt5 components.


Working directory
---------------------------

Set the software base directory where there is enough storage capacity
to host  Bayeux (> 1  GB). Here we  use a simple  environment variable
``SW_WORK_DIR``  which   points  to   a  specific  directory   on  the
filesystem:

.. code:: sh

   $ export SW_WORK_DIR=/opt/sw
..

You may adapt this base directory to your own system, for example:

.. code:: sh

   $ export SW_WORK_DIR=${HOME}/Software
..

Then create a few working directories:

.. code:: sh

   $ mkdir -p ${SW_WORK_DIR}
   $ mkdir -p ${SW_WORK_DIR}/Bayeux         # base working directory for Bayeux
   $ mkdir -p ${SW_WORK_DIR}/Bayeux/Source  # hosts the source directories
   $ mkdir -p ${SW_WORK_DIR}/Bayeux/Binary  # hosts the build/installation directories
..

Download Bayeux
---------------------

Download Bayeux/trunk source files:

.. $ export BX_SOURCE_BASE_DIR="${HOME}/Documents/Private/Software/Bayeux/Source"

.. code:: sh

   $ export BX_SOURCE_BASE_DIR="${SW_WORK_DIR}/Bayeux/Source"
   $ export BX_DEV_SOURCE_DIR=${BX_SOURCE_BASE_DIR}/Bayeux-trunk
   $ cd ${BX_SOURCE_BASE_DIR}
   $ svn co https://nemo.lpc-caen.in2p3.fr/svn/Bayeux/trunk Bayeux-trunk
   $ cd Bayeux-trunk
   $ pwd
   /home/mauger/Documents/Private/Software/Bayeux/Source/Bayeux-trunk
   $ LANG=C svn info
   Path: .
   Working Copy Root Path: /home/mauger/Documents/Private/Software/Bayeux/Source/Bayeux-trunk
   URL: https://nemo.lpc-caen.in2p3.fr/svn/Bayeux/trunk
   Relative URL: ^/Bayeux/trunk
   Repository Root: https://nemo.lpc-caen.in2p3.fr/svn
   Repository UUID: 3e0f96b8-c9f3-44f3-abf0-77131c94f4b4
   Revision: 17933
   Node Kind: directory
   Schedule: normal
   Last Changed Author: mauger
   Last Changed Rev: 17924
   Last Changed Date: 2016-06-27 17:58:43 +0200 (lun., 27 juin 2016)
..

.. raw:: latex

   \pagebreak

Configure Bayeux
--------------------------

  1. Make sure Cadfaelbrew is setup on  your system. If you follow the
     Cadfaelbrew installation  report available from the  Cadfael wiki
     page, you just have to invoke:

.. code:: sh

   $ brewsh
..

  2. Create a build directory and cd in it:

.. code:: sh

   $ export BX_DEV_BIN_DIR="${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk"
   $ export BX_DEV_BUILD_DIR=${BX_DEV_BIN_DIR}/Build-gcc-cxx11-ninja-Linux-x86_64
   $ mkdir -p ${BX_DEV_BUILD_DIR}
   $ cd ${BX_DEV_BUILD_DIR}
   $ pwd
   /opt/sw/Bayeux/Binary/Bayeux-trunk/Build-gcc-cxx11-ninja-Linux-x86_64
..

  3. Configure the Bayeux build with CMake and using Ninja and GCC :

.. code:: sh

   $ export BX_DEV_INSTALL_DIR="${BX_DEV_BIN_DIR}/Install-gcc-cxx11-Linux-x86_64"
   $ cmake \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCMAKE_INSTALL_PREFIX:FILEPATH="${BX_DEV_INSTALL_DIR}" \
    -DBAYEUX_CXX_STANDARD="11" \
    -DBAYEUX_WITH_IWYU_CHECK=ON \
    -DBAYEUX_WITH_DEVELOPER_TOOLS=ON \
    -DBAYEUX_WITH_LAHAGUE=OFF \
    -DBAYEUX_WITH_GEANT4_MODULE=ON \
    -DBAYEUX_WITH_MCNP_MODULE=OFF \
    -DBAYEUX_WITH_QT_GUI=ON \
    -DBAYEUX_ENABLE_TESTING=ON \
    -DBAYEUX_WITH_DOCS=ON \
    -DBAYEUX_WITH_DOCS_OCD=ON \
    -GNinja \
    ${BX_DEV_SOURCE_DIR}
..

Build
-----------------

Using 4 processors to go faster (depends on your machine):

.. code:: sh

   $ time ninja -j4
   ...
   real	17m7.958s
   user	62m46.852s
   sys	3m19.396s
..

.. raw:: latex

   \pagebreak

Quick check after build
---------------------------------

After the build step, Bayeux uses  the following hierarchy on the file
system:

.. code:: sh

   $ LANG=C tree -L 1 BuildProducts/
   BuildProducts/
   |-- bin/
   |-- include/
   |-- lib/
   `-- share/
..

Particularly, the shared libraries are:

.. code:: sh

   $ LANG=C tree -F BuildProducts/lib/
   BuildProducts/lib/
   |-- cmake/
   |   `-- Bayeux-3.0.0/
   |       |-- BayeuxConfig.cmake
   |       |-- BayeuxConfigVersion.cmake
   |       |-- BayeuxDocs.cmake
   |       `-- BayeuxTargets.cmake
   |-- libBXCatch.a
   |-- libBayeux.so*
   |-- libBayeux_mctools_geant4.so*
   `-- libBayeux_mctools_mcnp.so*
..

Executable are in:

.. code:: sh

   $ LANG=C tree -L 1 -F BuildProducts/bin/
   BuildProducts/bin/
   |-- bxdpp_processing*
   |-- bxextract_table_of_objects*
   |-- bxg4_production*
   |-- bxg4_seeds*
   |-- bxgenbb_inspector*
   |-- bxgenbb_mkskelcfg*
   |-- bxgenvtx_mkskelcfg*
   |-- bxgenvtx_production*
   |-- bxgeomtools_inspector*
   |-- bxgeomtools_mkskelcfg*
   |-- bxmaterials_diagnose*
   |-- bxmaterials_inspector*
   |-- bxmctools_g4_mkskelcfg*
   |-- bxocd_make_doc*
   |-- bxocd_manual*
   |-- bxocd_sort_classnames.py*
   |-- bxquery*
   |-- bxtests/
   `-- bxvariant_inspector*
..

These  directories  and  files  will be  copied  in  the  installation
directory (but  ``bxtests/`` which contains test  programs usable only
at build stage).

..
    .. raw:: latex

..  \pagebreak

Test programs
---------------------------------

Before to do the final installation, we run the test programs:

.. code:: sh

   $ ninja test
   [1/1] Running tests...
   Test project /opt/sw/Bayeux/Binary/Bayeux-trunk/Build-gcc-cxx11-ninja-Linux-x86_64
           Start   1: datatools-test_reflection_0
     1/326 Test   #1: datatools-test_reflection_0 .......   Passed    0.10 sec
   ...
	   Start 343: bxbayeux-test_bayeux
   343/343 Test #343: bxbayeux-test_bayeux ..............   Passed    0.07 sec

   100% tests passed, 0 tests failed out of 326

   Total Test time (real) =  68.23 sec
..

.. raw:: latex

   \pagebreak


Installation
====================

Run:

.. code:: sh

   $ ninja install
   ...
..

Check installation
-----------------------

Browse the installation directory:

.. code:: sh

   $ LANG=C tree -L 3 -F ${BX_DEV_INSTALL_DIR}
   /opt/sw/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64
   |-- bin/
   |   |-- bxdpp_processing*
   |   |-- bxextract_table_of_objects*
   |   |-- bxg4_production*
   |   |-- bxg4_seeds*
   |   |-- bxgenbb_inspector*
   |   |-- bxgenbb_mkskelcfg*
   |   |-- bxgenvtx_mkskelcfg*
   |   |-- bxgenvtx_production*
   |   |-- bxgeomtools_inspector*
   |   |-- bxgeomtools_mkskelcfg*
   |   |-- bxmaterials_diagnose*
   |   |-- bxmaterials_inspector*
   |   |-- bxmctools_g4_mkskelcfg*
   |   |-- bxocd_make_doc*
   |   |-- bxocd_manual*
   |   |-- bxocd_sort_classnames.py*
   |   |-- bxquery*
   |   `-- bxvariant_inspector*
   |-- include/
   |   `-- bayeux/
   |       |-- bayeux.h
   |       |-- bayeux_config.h
   |       |-- brio/
   |       |-- cuts/
   |       |-- datatools/
   |       |-- dpp/
   |       |-- emfield/
   |       |-- genbb_help/
   |       |-- genvtx/
   |       |-- geomtools/
   |       |-- lahague/
   |       |-- materials/
   |       |-- mctools/
   |       |-- mygsl/
   |       |-- reloc.h
   |       `-- version.h
   |-- lib/
   |   |-- cmake/
   |   |   `-- Bayeux-3.0.0/
   |   |-- libBayeux.so
   |   |-- libBayeux_mctools_geant4.so
   |   `-- libBayeux_mctools_mcnp.so
   `-- share/
       `-- Bayeux-3.0.0/
	   |-- Documentation/
	   |-- examples/
	   `-- resources/
..

.. raw:: latex

   \pagebreak
..

Suggestions for a Bash setup (see below)
----------------------------------------------------

 1. Define convenient environment variables:

.. code:: sh

   $ export SW_WORK_DIR=/opt/sw
   $ export BX_DEV_INSTALL_DIR=\
	  "${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64"
..

 2. The only configuration you need now is:

.. code:: sh

   $ export PATH=${BX_DEV_INSTALL_DIR}/bin:${PATH}
..

    There is no need to update the ``LD_LIBRARY_PATH`` environment variable because Bayeux
    uses RPATH. So you should **NOT** use the following:

.. code:: sh

   $ export LD_LIBRARY_PATH=${BX_DEV_INSTALL_DIR}/lib:${LD_LIBRARY_PATH}
..

 3. After setting ``PATH`` as shown above, you can check where some of the
    executable are installed:

.. code:: sh

   $ which bxquery
   /opt/sw/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64/bin/bxquery
..

    Check Bayeux/datatools' OCD tool (Object Configuration Documentation):

.. code:: sh

      $ which bxocd_manual
	  /opt/sw/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64/bin/bxocd_manual
      $ bxocd_manual --action list
      List of registered class IDs :
      cuts::accept_cut
      cuts::and_cut
      ...
      mygsl::histogram_pool
..

    Check geometry tools; cd in the Bayeux/geomtools example #01:

.. code:: sh

   $ cd /opt/sw/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64/share/Bayeux-3.0.0/examples/geomtools/ex01
   $ export CONFIG_DIR=$(pwd)/config
..

   Run the geometry inspector:

.. code:: sh

   $ bxgeomtools_inspector --manager-config ${CONFIG_DIR}/manager.conf

     G E O M T O O L S    I N S P E C T O R
     Version 5.1.0

     Copyright (C) 2009-2015
     Francois Mauger, Xavier Garrido, Benoit Guillon,
     Ben Morgan and Arnaud Chapon

     immediate help: type "help"
     quit:           type "quit"
     support:        Gnuplot display
     support:        Root display from GDML

   geomtools>
..

   Test session:

.. code::

   geomtools> help
   ...
   geomtools> display --help
   ...
   geomtools> display
   ...
   geomtools> list_of_logicals
   ...
   geomtools> display optical_module.model.log
   ...
   geomtools> list_of_gids --with-category "optical_module.gc"
   List of available GIDs :
   [2020:0.0] as 'optical_module.gc'       [2020:0.1] as 'optical_module.gc'
   [2020:1.0] as 'optical_module.gc'       [2020:1.1] as 'optical_module.gc'
   geomtools> display [2020:0.1]

   Press [Enter] to continue...

   geomtools>  export_gdml bxgeomtools_test.gdml
   GDML file 'bxgeomtools_test.gdml' has been generated !
   geomtools> quit
..

.. raw:: latex

   \pagebreak

Setup your environment for Bayeux
==================================

Here we explicitely *load/setup* the Bayeux environment from a Bash shell
with a dedicated function defined in my ``~/.bashrc`` startup file:

.. code:: sh

   # The base directory of all the software (convenient path variable):
   export SW_WORK_DIR=/opt/sw
   export BXSW_BASE_DIR=${SW_WORK_DIR}/Bayeux
   export BX_DEV_BIN_DIR=${BXSW_BASE_DIR}/Binary/Bayeux-trunk
   export BX_DEV_BUILD_DIR=${BX_DEV_BIN_DIR}/Build-gcc-cxx11-ninja-Linux-x86_64

   # The Bayeux/trunk setup function:
   function do_bayeux_trunk_setup()
   {
    if [ -z "${CADFAELBREW_INSTALL_DIR}" ]; then
      echo "ERROR: Cadfaelbrew is not setup ! Please run 'brewsh'!" >&2
      return 1
    fi
    if [ -n "${BX_DEV_INSTALL_DIR}" ]; then
      echo "ERROR: Bayeux/trunk is already setup !" >&2
      return 1
    fi
    export BX_DEV_INSTALL_DIR=${BX_DEV_BIN_DIR}/Install-gcc-cxx11-Linux-x86_64
    export PATH=${BX_DEV_INSTALL_DIR}/bin:${PATH}
    echo "NOTICE: Bayeux/trunk is now setup !" >&2
    return 0;
   }
   export -f do_bayeux_trunk_setup

   # Special alias:
   alias bayeux_dev_setup="do_bayeux_trunk_setup"
..

When one wants to use pieces of software from Bayeux, one runs:

.. code:: sh

   $ brewsh
   $ bayeux_dev_setup
..

Then all executable are usable from the Bayeux installation directory:

.. code:: sh

   $ which bxocd_manual
   ...
   $ which bxgeomtools_inspector
   ...
   $ which bxg4_production
   ...
..

as well as package management utilities (CMake scripts...).

.. raw:: latex

   \pagebreak

Update the source code from the Bayeux/trunk
============================================

1. Activate the Cadfaelbrew environment:

.. code:: sh

   $ brewsh
..

2. Cd in the Bayeux/trunk source directory:

.. code:: sh

   $ cd ${HOME}/Documents/Software/Bayeux/Source/Bayeux-trunk

..

3. Update the source code:

.. code:: sh

   $ svn up
..

4. Cd in the Bayeux/trunk build directory:

.. code:: sh

   $ export BX_DEV_BIN_DIR="${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk"
   $ cd ${BX_DEV_BIN_DIR}/Build-gcc-cxx11-ninja-Linux-x86_64
..

5. You may need to clean the build directory:

.. code:: sh

   $ ninja clean
..

   and even to completely delete and rebuild it from scratch:

.. code:: sh

   $ cd ${BX_DEV_BIN_DIR}
   $ rm -fr Build-gcc-cxx11-ninja-Linux-x86_64
   $ mkdir Build-gcc-cxx11-ninja-Linux-x86_64
   $ cd Build-gcc-cxx11-ninja-Linux-x86_64
..

   then reconfigure (see above).

6. You may need to delete the install tree:

.. code:: sh

   $ rm -fr ${BX_DEV_BIN_DIR}/Install-gcc-cxx11-Linux-x86_64
..

7. Rebuild, test and (re)install:

.. code:: sh

   $ ninja -j4
   $ ninja test
   $ ninja install
..

.. raw:: latex

   \pagebreak

Appendices
===================================================

Alternative: build Bayeux with GNU make
------------------------------------------

a. Build dir:

.. code:: sh

   $ export BX_DEV_BIN_DIR="${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk"
   $ mkdir -p ${BX_DEV_BIN_DIR}/Build-gcc-cxx11-gnumake-Linux-x86_64
   $ cd ${BX_DEV_BIN_DIR}/Build-gcc-cxx11-gnumake-Linux-x86_64
..

b. Configure Bayeux with CMake and GNU make (default build system):

.. code:: sh

   $ brewsh
   $ export BX_DEV_INSTALL_DIR="${BX_DEV_BIN_DIR}/Install-gcc-cxx11-Linux-x86_64"
   $ cmake \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCMAKE_INSTALL_PREFIX:FILEPATH="${BX_DEV_INSTALL_DIR}" \
    -DBAYEUX_WITH_IWYU_CHECK=ON \
    -DBAYEUX_WITH_DEVELOPER_TOOLS=ON \
    -DBAYEUX_WITH_LAHAGUE=ON \
    -DBAYEUX_WITH_GEANT4_MODULE=ON \
    -DBAYEUX_WITH_MCNP_MODULE=ON \
    -DBAYEUX_WITH_QT_GUI=ON \
    -DBAYEUX_ENABLE_TESTING=ON \
    -DBAYEUX_WITH_DOCS=ON \
    -DBAYEUX_WITH_DOCS_OCD=ON \
    ${SW_WORK_DIR}/Bayeux/Source/Bayeux-trunk
..

c. Build, test and install:

.. code:: sh

   $ make -j4
   $ make test
   $ make install
..

.. raw:: latex

   \pagebreak
