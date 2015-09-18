====================================================================
Bayeux/trunk installation report on (X)Ubuntu 14.04 LTS (64bits)
====================================================================

* Author: Francois Mauger, LPC Caen <mauger@lpccaen.in2p3.fr>
* Date:   2015-09-17
* Last update: 2015-09-17

The target system
=================

Architecture: ::

  $ uname -a
  Linux mauger-laptop 3.13.0-55-generic #94-Ubuntu SMP Thu Jun 18 00:27:10 UTC 2015 x86_64 x86_64 x86_64 GNU/Linux

Processors: ::

  $ cat /proc/cpuinfo | grep "model name"
  model name	: Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
  model name	: Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
  model name	: Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz
  model name	: Intel(R) Core(TM) i7-3540M CPU @ 3.00GHz

Linux version: ::

  $ cat /etc/lsb-release
  DISTRIB_ID=Ubuntu
  DISTRIB_RELEASE=14.04
  DISTRIB_CODENAME=trusty
  DISTRIB_DESCRIPTION="Ubuntu 14.04.3 LTS"


GCC version: ::

  $ g++ --version | head -1
  g++ (Ubuntu 4.8.4-2ubuntu1~14.04) 4.8.4


Installation of Bayeux (trunk)
===============================

Install dependencies: ::

  $ sudo apt-get install libreadline-dev
  $ sudo apt-get install gnuplot gnuplot-doc gnuplot-mode
  $ sudo apt-get install libqt4-dev libqt4-dev-bin libqt4-gui
  $ sudo apt-get install libreadline-dev readline-common
  $ sudo apt-get install pandoc pandoc-data
  $ sudo apt-get install doxygen

Set the software base directory where there is enough storage capacity to host
Bayeux (> 1 GB). I choose here to use a simple environment variable ``SW_WORK_DIR``: ::

  $ export SW_WORK_DIR=/data/sw

You should adapt this base directory to your own system, for example: ::

  $ export SW_WORK_DIR=${HOME}/Software

Then create a few working directories: ::

  $ mkdir -p ${SW_WORK_DIR}
  $ mkdir ${SW_WORK_DIR}/Bayeux          # This one is the base working directory for Bayeux
  $ mkdir ${SW_WORK_DIR}/Bayeux/Source   # This one will host the source code
  $ mkdir ${SW_WORK_DIR}/Bayeux/Binary   # This one will host the build and installation directories

Download Bayeux/trunk source files: ::

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
  Revision: 15637
  Node Kind: directory
  Schedule: normal
  Last Changed Author: mauger
  Last Changed Rev: 15637
  Last Changed Date: 2015-01-20 09:28:20 +0100 (Tue, 20 Jan 2015)

Configure Bayeux:

  1. Make sure Cadfael is setup on your system. If you follow the Cadfael installation report
     available from the Cadfael wiki page (https://nemo.lpc-caen.in2p3.fr/wiki/Software/Cadfael)
     you just have to invoke: ::

      $ do_cadfael_dev_setup

  2. Create a build directory and cd in it: ::

      $ mkdir -p ${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Build-Linux-x86_64
      $ cd ${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Build-Linux-x86_64

  3. Invoke CMake to configure Bayeux: ::
      $ cmake \
         -DCMAKE_BUILD_TYPE:STRING=Release \
         -DCMAKE_INSTALL_PREFIX:PATH=${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Install-Linux-x86_64 \
         -DCMAKE_FIND_ROOT_PATH:PATH=${SW_WORK_DIR}/Cadfael/Binary/Cadfael-trunk/Install-Linux-x86_64 \
         -DBayeux_FORCE_CXX_ALL_WARNINGS=ON \
         -DBayeux_BUILD_GEANT4_MODULE=ON \
         -DBayeux_BUILD_DOCS=ON \
         -DBayeux_BUILD_OCD_DOCS=ON \
         -DBayeux_BUILD_DEVELOPER_TOOLS=ON \
         -DBayeux_BUILD_QT_GUI=ON \
         -DBayeux_ENABLE_TESTING=ON \
         -DBayeux_WITH_EXAMPLES=ON \
         ${SW_WORK_DIR}/Bayeux/Source/Bayeux-trunk

Build (using 4 processors to go faster): ::

  $ make -j4
  ...

Quick check after build
=========================

After the build step, Bayeux uses the following hierarchy on the file system: ::

  $ LANG=C tree -L 1 BuildProducts/
  BuildProducts/
  |-- bin
  |-- include
  |-- lib
  `-- share

Particularly, the shared libraries are: ::

  $ LANG=C tree -L 2 BuildProducts/lib/
  BuildProducts/lib/
  `-- x86_64-linux-gnu
      |-- cmake
      |-- libBayeux.so
      `-- libBayeux_mctools_geant4.so

Executable are in: ::

  $ LANG=C tree -L 1 BuildProducts/bin/
  BuildProducts/bin/
  |-- bxdpp_processing
  |-- bxg4_production
  |-- bxgenbb_inspector
  |-- bxgenbb_mkskelcfg
  |-- bxgenvtx_mkskelcfg
  |-- bxgenvtx_production
  |-- bxgeomtools_inspector
  |-- bxgeomtools_mkskelcfg
  |-- bxmaterials_inspector
  |-- bxmctools_g4_mkskelcfg
  |-- bxocd_make_doc
  |-- bxocd_manual
  |-- bxocd_sort_classnames.py
  |-- bxquery
  `-- bxtests

These directories and files will be copied in the installation directory.

Test programs
=========================

Before to do the final installation, we run the test programs: ::

  $ make test
  ...
  100% tests passed, 0 tests failed out of 298
  Total Test time (real) =  25.90 sec

Installation
====================

Simple run: ::

  $ make install

Check installation
========================

Browse the installation directory: ::

  $ LANG=C tree -L 3 -F ${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Install-Linux-x86_64
  /data/sw/Bayeux/Binary/Bayeux-trunk/Install-Linux-x86_64
  |-- bin/
  |   |-- bxdpp_processing
  |   |-- bxg4_production
  |   |-- bxgenbb_inspector
  |   |-- bxgenbb_mkskelcfg
  |   |-- bxgenvtx_mkskelcfg
  |   |-- bxgenvtx_production
  |   |-- bxgeomtools_inspector
  |   |-- bxgeomtools_mkskelcfg
  |   |-- bxmaterials_inspector
  |   |-- bxmctools_g4_mkskelcfg
  |   |-- bxocd_make_doc
  |   |-- bxocd_manual
  |   |-- bxocd_sort_classnames.py
  |   `-- bxquery
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
  |   `-- x86_64-linux-gnu/
  |       |-- cmake/
  |       |-- libBayeux.so
  |       `-- libBayeux_mctools_geant4.so
  `-- share/
      `-- Bayeux-2.0.0/
          |-- Documentation/
          |-- examples/
          `-- resources/

Suggestions for a Bash setup (see below):

 1. Define convenient environment variables: ::

     $ export SW_WORK_DIR=/data/sw
     $ export BAYEUX_INSTALL_DIR=${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Install-Linux-x86_64

 2. The only configuration you need now is: ::

     $ export PATH=${BAYEUX_INSTALL_DIR}/bin:${PATH}

    There is no need to update the ``LD_LIBRARY_PATH`` environment variable because Bayeux
    uses RPATH. So you **should NOT** use the following: ::

     $ export LD_LIBRARY_PATH=${BAYEUX_INSTALL_DIR}/lib:${LD_LIBRARY_PATH}

 3. After setting ``PATH`` as shown above, you can check where some of the
    executable are installed: ::

      $ which bxquery
      /data/sw/Bayeux/Binary/Bayeux-trunk/Install-Linux-x86_64/bin/bxquery

    Check datatools' OCD tool: ::

      $ which bxocd_manual
      /data/sw/Bayeux/Binary/Bayeux-trunk/Install-Linux-x86_64/bin/bxocd_manual
      $ bxocd_manual --action list
      List of registered class IDs :
      cuts::accept_cut
      cuts::and_cut
      ...
      mygsl::histogram_pool

    Check geometry tools; cd in the Bayeux/geomtools example #01: ::

      $ cd ${SW_WORK_DIR}/Bayeux/Source/Bayeux-trunk/source/bxgeomtools/examples/ex01
      $ export CONFIG_DIR=$(pwd)/config
      $ bxgeomtools_inspector --manager-config config/manager.conf

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
      $

Conclusion:

 * No problem for compiling, running tests and examples.


Setup your environment for Bayeux
==================================

I prefer here to explicitely *load/setup* the Bayeux environment from my Bash shell
with a dedicated function defined in my ``~/.bashrc`` startup file: ::

  # The base directory of all the software (convenient path variable):
  export SW_WORK_DIR=/data/sw

  # The Bayeux/trunk setup function:
  function do_bayeux_trunk_setup()
  {
    do_cadfael_dev_setup # Automatically load the Cadfael dependency
    if [ -n "${BAYEUX_INSTALL_DIR}" ]; then
        echo "ERROR: Bayeux/trunk is already setup !" >&2
        return 1
    fi
    export BAYEUX_INSTALL_DIR=${SW_WORK_DIR}/Bayeux/Binary/Bayeux-trunk/Install-Linux-x86_64
    export PATH=${BAYEUX_INSTALL_DIR}/bin:${PATH}
    echo "NOTICE: Bayeux/trunk is now setup !" >&2
    return;
  }
  export -f do_bayeux_trunk_setup

  # Special alias:
  alias do_bayeux_dev_setup="do_bayeux_trunk_setup"

When I want to use pieces of software from Bayeux, I run::

  $ do_bayeux_dev_setup

Then all executable are usable from the Bayeux installation directory: ::

  $ which bxocd_manual
  ...
  $ which bxgeomtools_inspector
  ...
  $ which bxg4_production
  ...
