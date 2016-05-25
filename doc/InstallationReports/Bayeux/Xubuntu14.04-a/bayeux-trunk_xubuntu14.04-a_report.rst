====================================================================
Bayeux/trunk installation report on (X)Ubuntu 14.04 LTS (64bits)
====================================================================

:Author: François Mauger, LPC Caen <mauger@lpccaen.in2p3.fr>
:Date:   2016-05-25

In  this  document  we  propose  an  installation  procedure  for  the
Bayeux_/trunk  library on  top  of Cadfaelbrew_  (2016.01) on  Xubuntu
14.04 LTS (Trusty Tahr) for a system (64-bits).  By default, the build
of Bayeux is done using the C++11 standard.

Notes:

 * Cadfaelbrew_ is only supported  on 64-bits systems. This constrains
   Bayeux_ installation to such architectures.
 * In a near future (spring 2016), C++11 will become the standard used
   by default within Bayeux and C++98 will not be supported anymore.
 * The  (X)ubuntu  16.04  LTS  (Xenial Xerus)  will  become  the  main
   supported Ubuntu flavor. This will imply a few changes in process of
   Cadfaelbrew and Bayeux installation.
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
   Linux bayeux-laptop 3.13.0-74-generic #118-Ubuntu SMP ... x86_64 GNU/Linux
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
   DISTRIB_RELEASE=14.04
   DISTRIB_CODENAME=trusty
   DISTRIB_DESCRIPTION="Ubuntu 14.04.3 LTS"
..

* Environment:

  The system must have a relatively *bare* environment. It means that even if a lot of software
  has been installed on the system (``/usr/bin``) or in some alternative locations (``/usr/local``,
  ``/opt``...), you should be able to run a shell with a lightweight ``PATH``, typically something
  like:

.. code:: sh

   $ echo $PATH
   /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games
..

   In principle, you should not have the ``LD_LIBRARY_PATH`` environmental variable set:

.. code:: sh

   $ echo aaa${LD_LIBRARY_PATH}ZZZ
   aaaZZZ
..

* Dependencies:

  It may be  useful to install additional system  packages to properly
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
.. _Cadfaelbrew: https://github.com/SuperNEMO-DBD/cadfaelbrew


Please follow the instructions on the installation report at https://nemo.lpc-caen.in2p3.fr/browser/Bayeux/trunk/doc/InstallationReports/Cadfaelbrew/Xubuntu14.04-a/tagged/cadfaelbrew_xubuntu14.04-a_report-0.1.pdf

brew
------------

Once you have installed Cadfaelbrew_, you should be able to run a *brew* session:

.. code:: sh

   $ brewsh
..

Note that, in this example, the ``brewsh`` is an alias for:

.. code:: sh

   $ /data3/sw/Cadfaelbrew/supernemo/cxx11/Cadfael.git/bin/brew sh --cc=gcc-4.9
..

where the ``brew`` utility has been installed by Cadfaelbrew_ on some arbitrary path on the system.

This opens a new shell with all environmental variables activated to setup all the software tools
managed through Cadfaelbrew_.


Alternatively you can use a dedicated setup function, but you should favor the first method above:

.. code:: sh

   $ do_cadfaelbrew_setup
   NOTICE: Cadfaelbrew is now setup !
..

You can check the location and version of core software utilities:

.. code:: sh

   $ which cmake
   /path/to/Cadfaelbrew/install/supernemo/cxx11/Cadfael.git/bin/cmake
..

.. code:: sh

   $ cmake --version
   cmake version 3.4.0
..

.. code:: sh

   $ g++ --version
   g++ (Homebrew gcc49 4.9.2_2) 4.9.2
..

.. code:: sh

   $ doxygen --version
   1.8.10
..

Ninja
-------------

Ninja_ is  a build  system which  can be used  in place  of (GNU)make.
Install Ninja_ through ``brew`` if it  was not already done before (you
must setup the brew environment for that):

.. _Ninja: https://ninja-build.org/

.. code:: sh

   $ brewsh
   $ brew install ninja
   ...
..

Then you can check your Ninja version:

.. code:: sh

   $ ninja --version
   1.6.0
   $ exit
..

.. raw:: latex

   \pagebreak


Qt5
-------------

Qt5 is used for the QS-based GUI components implemented in Bayeux (optional component).

For now we use the system install of Qt5 (5.2.1 on Ubuntu 14.04):

.. code:: sh

   $ sudo apt-get install libqt5core5a
	  libqt5gui5
	  libqt5svg5
	  libqt5svg5-dev
	  libqt5svg5-private-dev
	  libqt5widgets5
..

Brew is able to install a recent Qt5 (Qt5.6.0) but this is still broken wihtin Bayeux.
Please do not use it as long as it is not fixed.

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

Install dependencies:

.. code:: sh

   $ sudo apt-get install gnuplot gnuplot-doc gnuplot-mode
   $ sudo apt-get install libreadline-dev readline-common
   $ sudo apt-get install pandoc pandoc-data
   $ sudo apt-get install python-docutils
..

See above for Qt5 components.


Working directory
---------------------------

Set the software base directory where there is enough storage capacity
to host  Bayeux (> 1  GB). Here we  use a simple  environment variable
``SW_WORK_DIR``  which   points  to   a  specific  directory   on  the
filesystem:

.. code:: sh

   $ export SW_WORK_DIR=/data/sw
..

You should adapt this base directory to your own system, for example:

.. code:: sh

   $ export SW_WORK_DIR=${HOME}/Software
..

Then create a few working directories:

.. code:: sh

   $ mkdir -p ${SW_WORK_DIR}
   $ mkdir ${SW_WORK_DIR}/Bayeux         # base working directory for Bayeux
   $ mkdir ${SW_WORK_DIR}/Bayeux/Source  # hosts the source code
   $ mkdir ${SW_WORK_DIR}/Bayeux/Binary  # hosts the build/installation directories
..

Download Bayeux
---------------------

Download Bayeux/trunk source files:

.. code:: sh

   $ cd ${SW_WORK_DIR}/Bayeux/Source
   $ svn co https://nemo.lpc-caen.in2p3.fr/svn/Bayeux/trunk Bayeux-trunk
   $ cd Bayeux-trunk
   $ LANG=C svn info
   Path: .
   Working Copy Root Path: /data/sw/Bayeux/Source/Bayeux-trunk
   URL: https://nemo.lpc-caen.in2p3.fr/svn/Bayeux/trunk
   Relative URL: ^/Bayeux/trunk
   Repository Root: https://nemo.lpc-caen.in2p3.fr/svn
   Repository UUID: 3e0f96b8-c9f3-44f3-abf0-77131c94f4b4
   Revision: 17214
   Node Kind: directory
   Schedule: normal
   Last Changed Author: mauger
   Last Changed Rev: 17210
   Last Changed Date: 2016-03-04 23:36:04 +0100 (Fri, 04 Mar 2016)
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

or :

.. code:: sh

      $ do_cadfaelbrew_setup
..

  2. Create a build directory and cd in it:

.. code:: sh

   $ BX_DEV_BIN_DIR="${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk"
   $ mkdir -p ${BX_DEV_BIN_DIR}/Build-gcc-cxx11-ninja-Linux-x86_64
   $ cd ${BX_DEV_BIN_DIR}/Build-gcc-cxx11-ninja-Linux-x86_64
..

  3. Configure the Bayeux build with CMake and using Ninja and GCC :

.. code:: sh

   $ BX11_DEV_INSTALL_DIR="${BX_DEV_BIN_DIR}/Install-gcc-cxx11-Linux-x86_64"
   $ cmake \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCMAKE_INSTALL_PREFIX:FILEPATH="${BX11_DEV_INSTALL_DIR}" \
    -DBAYEUX_WITH_IWYU_CHECK=ON \
    -DBAYEUX_WITH_DEVELOPER_TOOLS=ON \
    -DBAYEUX_WITH_LAHAGUE=ON \
    -DBAYEUX_WITH_GEANT4_MODULE=ON \
    -DBAYEUX_WITH_MCNP_MODULE=ON \
    -DBAYEUX_WITH_QT_GUI=ON \
    -DBAYEUX_ENABLE_TESTING=ON \
    -DBAYEUX_WITH_DOCS=ON \
    -DBAYEUX_WITH_DOCS_OCD=ON \
    -GNinja \
    ${SW_WORK_DIR}/Bayeux/Source/Bayeux-trunk
..

..    -DQt5Core_DIR:FILEPATH="$(brew --prefix)/Cellar/qt5-base/5.6.0/lib/cmake/Qt5Core" \
..    -DQt5Gui_DIR:FILEPATH="$(brew --prefix)/Cellar/qt5-base/5.6.0/lib/cmake/Qt5Gui" \
..    -DQt5Widgets_DIR:FILEPATH="$(brew --prefix)/Cellar/qt5-base/5.6.0/lib/cmake/Qt5Widgets" \
..    -DQt5Svg_DIR:FILEPATH="$(brew --prefix)/Cellar/qt5-base/5.6.0/lib/cmake/Qt5Svg" \

Build
-----------------

Using 4 processors to go faster (depends on your machine):

.. code:: sh

   $ time ninja -j4
   ...
   real 12m6.886s
   user 43m4.932s
   sys  2m24.929s
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
   |   `-- Bayeux-2.1.0/
   |       |-- BayeuxConfig.cmake
   |       |-- BayeuxConfigVersion.cmake
   |       |-- BayeuxDocs.cmake
   |       `-- BayeuxTargets.cmake
   |-- libBayeux.so*
   |-- libBayeux_mctools_geant4.so*
   `-- libBayeux_mctools_mcnp.so*
..

Executable are in:

.. code:: sh

   $ LANG=C tree -L 1 -F BuildProducts/bin/
   BuildProducts/bin/
   |-- bxdpp_processing*
   |-- bxg4_production*
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
   `-- bxtests/
..

These  directories  and  files  will be  copied  in  the  installation
directory.

..
    .. raw:: latex

..  \pagebreak

Test programs
---------------------------------

Before to do the final installation, we run the test programs:

.. code:: sh

   $ ninja test
   [1/1] Running tests...
   Test project /data/sw/Bayeux/Binary/Bayeux-trunk/Build-gcc-cxx11-ninja-Linux-x86_64
           Start   1: datatools-test_reflection_0
     1/303 Test   #1: datatools-test_reflection_0 .......   Passed    0.28 sec
   ...
           Start 309: bayeux-test_bayeux
   309/309 Test #309: bayeux-test_bayeux ................   Passed    0.13 sec

   100% tests passed, 0 tests failed out of 309

   Total Test time (real) =  60.47 sec
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

   $ LANG=C tree -L 3 -F \
     ${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64
   /data/sw/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64
   |-- bin/
   |   |-- bxdpp_processing*
   |   |-- bxg4_production*
   |   |-- bxgenbb_inspector*
   |   |-- bxgenbb_mkskelcfg*
   |   |-- bxgenvtx_mkskelcfg*
   |   |-- bxgenvtx_production*
   |   |-- bxgeomtools_inspector*
   |   |-- bxgeomtools_mkskelcfg*
   |   |-- bxmaterials_inspector*
   |   |-- bxmctools_g4_mkskelcfg*
   |   |-- bxocd_make_doc*
   |   |-- bxocd_manual*
   |   |-- bxocd_sort_classnames.py*
   |   `-- bxquery*
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
   |       |-- materials/
   |       |-- mctools/
   |       |-- mygsl/
   |       |-- qt/
   |       |-- reloc.h
   |       `-- version.h
   |-- lib/
   |   |-- cmake/
   |   |   `-- Bayeux-2.1.0/
   |   |-- libBayeux.so
   |   `-- libBayeux_mctools_geant4.so
   `-- share/
       `-- Bayeux-2.1.0/
           |-- Documentation/
           |-- examples/
           `-- resources/
..

.. raw:: latex

   \pagebreak
..

Suggestions for a Bash setup (see below)
----------------------------------------------------

 1. Define convenient environmental variables:

.. code:: sh

   $ export SW_WORK_DIR=/data/sw
   $ export BX11_DEV_INSTALL_DIR=\
       "${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64"
..

 2. The only configuration you need now is:

.. code:: sh

   $ export PATH=${BX11_DEV_INSTALL_DIR}/bin:${PATH}
..

    There is no need to update the ``LD_LIBRARY_PATH`` environment variable because Bayeux
    uses RPATH. So you **should NOT** use the following:

.. code:: sh

   $ export LD_LIBRARY_PATH=${BX11_DEV_INSTALL_DIR}/lib:${LD_LIBRARY_PATH}
..

 3. After setting ``PATH`` as shown above, you can check where some of the
    executable are installed:

.. code:: sh

   $ which bxquery
   /data/sw/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64/bin/bxquery
..

    Check datatools' OCD tool:

.. code:: sh

      $ which bxocd_manual
      /data/sw/Bayeux/Binary/Bayeux-trunk/Install-gcc-cxx11-Linux-x86_64/bin/bxocd_manual
      $ bxocd_manual --action list
      List of registered class IDs :
      cuts::accept_cut
      cuts::and_cut
      ...
      mygsl::histogram_pool
..

    Check geometry tools; cd in the Bayeux/geomtools example #01:

.. code:: sh

      $ cd ${SW_WORK_DIR}/Bayeux/Source/Bayeux-trunk/source/bxgeomtools/examples/ex01
      $ export CONFIG_DIR=$(pwd)/config
      $ bxgeomtools_inspector --manager-config config/manager.conf
..

.. code::

        G E O M T O O L S    I N S P E C T O R
        Version 5.0.0

        Copyright (C) 2009-2015
        Francois Mauger, Xavier Garrido, Benoit Guillon,
        Ben Morgan and Arnaud Chapon

        immediate help: type "help"
        quit:           type "quit"
        support:        Gnuplot display
        support:        Root display from GDML

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
      geomtools> list_of_gids --with-category optical_module.gc
      List of available GIDs :
        [2020:0.0] as 'optical_module.gc'       [2020:0.1] as 'optical_module.gc'
        [2020:1.0] as 'optical_module.gc'       [2020:1.1] as 'optical_module.gc'
      geomtools> display [2020:0.1]

      Press [Enter] to continue...

      geomtools>  export_gdml bxgeomtools_test.gdml
      GDML file 'bxgeomtools_test.gdml' has been generated !
      geomtools> quit
..

Conclusion:

 * No problem for compiling, running tests and examples.


.. raw:: latex

   \pagebreak

Setup your environment for Bayeux
==================================

Here we explicitely *load/setup* the Bayeux environment from a Bash shell
with a dedicated function defined in my ``~/.bashrc`` startup file:

.. code:: sh

   # The base directory of all the software (convenient path variable):
   export SW_WORK_DIR=/data/sw
   export BX_DEV_BIN_DIR="${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk"

   # The Bayeux/trunk setup function:
   function do_bayeux_trunk_cxx11_setup()
   {
    do_cadfaelbrew_setup # Automatically load the Cadfaelbrew dependency
    if [ -n "${BX11_DEV_INSTALL_DIR}" ]; then
      echo "ERROR: Bayeux/trunk is already setup !" >&2
      return 1
    fi
    export BX11_DEV_INSTALL_DIR=${BX_DEV_BIN_DIR}/Install-gcc-cxx11-Linux-x86_64
    export PATH=${BX11_DEV_INSTALL_DIR}/bin:${PATH}
    echo "NOTICE: Bayeux/trunk is now setup !" >&2
    return;
   }
   export -f do_bayeux_trunk_cxx11_setup

   # Special alias:
   alias do_bayeux_dev11_setup="do_bayeux_trunk_cxx11_setup"
   alias do_bayeux_dev_setup="do_bayeux_trunk_cxx11_setup"
..

When one wants to use pieces of software from Bayeux, one runs:

.. code:: sh

   $ do_bayeux_dev_setup
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

.. raw:: latex

   \pagebreak

Update the source code from the Bayeux/trunk
============================================

1. Activate the Cadfaelbrew environment:

.. code:: sh

   $ do_cadfaelbrew_setup
..

   or enter a ``brew`` shell (recommended):

.. code:: sh

   $ brewsh
..

2. Cd in the Bayeux/trunk source directory:

.. code:: sh

   $ cd ${SW_WORK_DIR}/Bayeux/Source/Bayeux-trunk

..

3. Update the source code:

.. code:: sh

   $ svn up
..

4. Cd in the Bayeux/trunk build directory:

.. code:: sh

   $ BX_DEV_BIN_DIR="${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk"
   $ cd ${BX_DEV_BIN_DIR}/Build-gcc-cxx11-ninja-Linux-x86_64
..

5. You may need to clean the build directory:

.. code:: sh

   $ ninja -clean
..

   and even to completely delete it to rebuild from scratch:

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

7. Rebuild, test and install:

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

   $ BX_DEV_BIN_DIR="${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk"
   $ mkdir -p ${BX_DEV_BIN_DIR}/Build-gcc-cxx11-gnumake-Linux-x86_64
   $ cd ${BX_DEV_BIN_DIR}/Build-gcc-cxx11-gnumake-Linux-x86_64
..

b. Configure Bayeux with CMake and GNU make (default build system):

.. code:: sh

   $ brewsh
   $ BX11_DEV_INSTALL_DIR="${BX_DEV_BIN_DIR}/Install-gcc-cxx11-Linux-x86_64"
   $ cmake \
    -DCMAKE_BUILD_TYPE:STRING="Release" \
    -DCMAKE_INSTALL_PREFIX:FILEPATH="${BX11_DEV_INSTALL_DIR}" \
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

   $ time make -j4
   ...
   $ make test
   $ make install
..

.. raw:: latex

   \pagebreak
