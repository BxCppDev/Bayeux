===================================================================
Installation report for Cadfaelbrew on Xubuntu Linux 14.04 (LTS)
===================================================================

:Author: F.Mauger
:Version: 0.1
:Contact: mauger@lpccaen.in2p3.fr
:Date: 2016-03-06

This note  explains the Cadfaelbrew installation  procedure on Xubuntu
Linux 14.04 (LTS) (64-bits). Cadfaelbrew is a package manager, derived
from the  Linux/Homebrew projects.  It allows  you to  install various
software *formulas*.  Thanks to the generic ``brew`` utility, you will
be able to  install the *Cadfael* formula which  includes Boost, CAMP,
CLHEP, GSL, ROOT, Geant4...  that is  all the software you need to run
Bayeux and the  SuperNEMO software.  Other formulas can  also be added
(Bayeux, Falaise...).  This is out of the scope of this document which
concentrates on the installation of the basic layer of Cadfaelbrew and
Cadfael formulas.

Cadfaelbrew is hosted at https://github.com/SuperNEMO-DBD/.

Ben Morgan (Ben.Morgan@warwick.ac.uk) is  the developper and maintener
of Cadfaelbrew for the SuperNEMO collaboration.

.. contents::

Preparation
===========

   a. Requirements:

      * A working network connection
      * Bash shell

   b. The working directory:

      In this  report, the  ``/data3`` directory will  be used  as the
      base working  directory to build  and install the  software. You
      are free to  choose any other location on  your system, provided
      there is enough space on it (~10 GB).

.. code:: sh

   bash$ mkdir -p /data3/sw/Cadfaelbrew
   bash$ cd /data3/sw/Cadfaelbrew
..

   c. Install mandatory packages:

.. code:: sh

   bash$ LANG=C sudo apt-get install build-essential curl git m4 \
	  ruby texinfo libbz2-dev libcurl4-openssl-dev \
	  libexpat-dev libncurses-dev zlib1g-dev
..


Download the Cadfael installer
================================================================================

The Cadfael installer utility  is available from the ``SuperNEMO-DBD``
``git``  repository  (GitHub).  You  must  have  a recent  version  of
``git`` installed  on your  system. The  installer will  first install
Cadfaelbrew before the Cadfael software itself.

   a. Clone the default Git branch to your local filesystem:

.. code:: sh

   bash$ LANG=C git clone https://github.com/SuperNEMO-DBD/cadfael-installer.git \
            cadfael-installer.git
   Cloning into 'cadfael-installer.git'...
   remote: Counting objects: 295, done.
   remote: Total 295 (delta 0), reused 0 (delta 0), pack-reused 295
   Receiving objects: 100% (295/295), 55.80 KiB | 0 bytes/s, done.
   Resolving deltas: 100% (156/156), done.
   Checking connectivity... done.
..

   b. Explore what you have donwloaded:

.. code:: sh

   bash$ LANG=C tree -L 1 -F cadfael-installer.git/
   cadfael-installer.git/
   |-- LICENSE
   |-- README.md
   |-- cadfael-installer*
   `-- containers/
..

Prepare the installation
==============================================

The following  steps are  not mandatory. It  depends on  the available
storage  on  your  filesystem.    By  default,  Cadfaelbrew  uses  the
``~/.cache/Homebrew`` directory  as the download cache  directory.  If
your home directory has not enough space left, you will need to define
the ``HOMEBREW_CACHE``  environmental variable to another  location of
your filesystem.

Also Cadfaelbrew uses the ``/tmp`` directory as the temporary location
to build the  software.  If the disk partition  hosting this directory
has   not  enough   space  left,   you   will  need   to  define   the
``HOMEBREW_TEMP`` environmental variable.

Examples:

   a. Here  we create  a  directory that  will be  used  as the  cache
      directory to  download source tarballs.  This  directory must be
      large  enough to  endure at  least  500 MB  of downloaded  files
      (source tarballs, patch files, Geant4 datasets...).

.. code:: sh

   bash$ mkdir -p /data3/var/cache/Homebrew
   bash$ export HOMEBREW_CACHE=/data3/var/cache/Homebrew
..

   b. Here we  create a directory that  will be used as  the temporary
      build  directory for  all  software  packages (formulas).   This
      directory  must be  large  enough to  endure at  least  4 GB  of
      temporary files (mainly because compiling GCC uses a huge amount
      of disk space).

.. code:: sh

   bash$ mkdir -p /data3/var/tmp
   bash$ export HOMEBREW_TEMP=/data3/var/tmp
..

.. raw:: latex

   \pagebreak
..


Build and install Cadfaelbrew in C++11 mode
=====================================================

The  ``cadfael-installer``   script  is   provided  to   automate  the
installation of Cadfaelbrew and also some software packages (formulas)
used by the SuperNEMO software (example: tagged version of Bayeux...).

a. You can print some basic online help with:

.. code:: sh

   bash$ cd cadfael-installer.git/
   bash$ ./cadfael-installer -h
..

   There is only a very small set of options for now.

b. Here we request a bare installation (``-b``) of Cadfaelbrew without
   installing specific formulas for  SuperNEMO software (like Bayeux).
   We build the software with C++11 (``-e``).  The installation prefix
   is set with the ``-p`` switch:

   ``/data3/sw/Cadfaelbrew/supernemo/cxx11``.

   Run:

.. code:: sh

   bash$ cd cadfael-installer.git/
   bash$ ./cadfael-installer -b -e -p /data3/sw/Cadfaelbrew/supernemo/cxx11
..

   At  the end  of the  process, the  Cadfaelbrew environment  will be
   installed in :

   ``/data3/sw/Cadfaelbrew/supernemo/cxx11/Cadfael.git/``

   You will find the ``brew`` script in the ``bin/`` subdirectory.


Install the ``cadfael`` formula
====================================

Now  Cadfaelbrew is  available, we  can  install the  formula for  the
Cadfael software:

.. code:: sh

   bash$ /data3/sw/Cadfaelbrew/supernemo/cxx11/Cadfael.git/bin/brew \
     install cadfael
..

This is a  rather long process, particularly because  the GCC compiler
(version 4.9) is built and installed (~35 min on this system).  Geant4
and Root  are also  rather long  to build.  All  the software  will be
installed   in  ``/data3/sw/Cadfaelbrew/supernemo/cxx11/Cadfael.git``.
It takes approximatively 3 GB.

The following report gives you an idea of the time needed to build the
``cadfael`` formula on a system with four i7-3540M processors @ 3 GHz:

.. raw:: latex

   \tiny
..

.. include:: logs/cadfael_formula_summary_0.log
   :code: sh
..

.. raw:: latex

   \normalsize
..

.. raw:: latex

   \pagebreak
..

Setup Cadfaelbrew for your environment
=====================================================

To activate Cadfaelbrew in your environement, you have two possibilities:

a. You can  define a specific Bash function in your ``~/.bashrc`` startup file:

.. code:: sh

   function do_cadfaelbrew_setup()
   {
      if [ -n "${CADFAELBREW_INSTALL_DIR}" ]; then
	  echo "ERROR: Cadfaelbrew is already setup !" >&2
	  return 1
      fi
      export CADFAELBREW_INSTALL_DIR=\
        "/data3/sw/Cadfaelbrew/supernemo/cxx11/Cadfael.git"
      export PATH="${CADFAELBREW_INSTALL_DIR}/bin:${PATH}"
      export MANPATH="${CADFAELBREW_INSTALL_DIR}/share/man:${MANPATH}"
      export INFOPATH="${CADFAELBREW_INSTALL_DIR}/share/info:${INFOPATH}"
      echo "NOTICE: Cadfaelbrew is now setup !" >&2
      return
   }
   export -f do_cadfaelbrew_setup
..

   You can activate thus Cadfaelbrew from your shell with :

.. code:: sh

   bash$ do_cadfaelbrew_setup
..

b. Alternatively you can use the ``brew.sh`` script provided by Cadfaelbrew.
   In your ``~/.bashrc`` startup file, define:

.. code:: sh

   alias brewsh='/data3/sw/Cadfaelbrew/supernemo/cxx11/Cadfael.git/bin/brew sh'
..

   Then, each time you need to use Cadfaelbrew or some software managed by it, you
   just type in your shell:

.. code:: sh

   bash$ brewsh
..

   This will start a dedicated shell with all Cadfaelbrew software activated.
   To leave this environment, type:

.. code:: sh

   bash$ exit
..


Additional operations after installation of the ``cadfael`` formula
======================================================================

Geant4
----------

At the time  I have installed this version of  Cadfaelbrew, the Geant4
version  shipped with  Cadfaelbrew was  not including  by default  the
Geant4  OpenGL/X11 visualization  library  module on  Linux.  Ben  has
added  an option  (``--with-opengl-x11``) and  I was  able to  rebuild
Geant4  with   this  feature   explicitely  activated.    This  allows
interactive Geant4 session with visualization.


To rebuild Geant4 with brew:

.. code:: sh

   bash$ export HOMEBREW_CACHE=/data3/var/cache/Homebrew
   bash$ export HOMEBREW_TEMP=/data3/var/tmp
   bash$ brewsh
   bash$ brew reinstall geant4 --with-opengl-x11
..

Unfortunately, this operation rebuilds Geant4 from scratch, so it is rather long.
