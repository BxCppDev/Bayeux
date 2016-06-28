===================================================================
Installation report for CadfaelBrew on Xubuntu Linux 14.04 (LTS)
===================================================================

:Author: F.Mauger
:Version: 0.2
:Contact: mauger@lpccaen.in2p3.fr
:Date: 2016-06-28

This note  explains the CadfaelBrew installation  procedure on Xubuntu
Linux 14.04 (LTS) (64-bits). CadfaelBrew is a package manager, derived
from the  Linux/Homebrew projects.  It allows  you to  install various
software *formulas*.  Thanks to the generic ``brew`` utility, you will
be able to  install the *Cadfael* formula which  includes Boost, CAMP,
CLHEP, ROOT, Geant4...  that is  all the software you need to run
Bayeux and the  SuperNEMO software.  Other formulas can  also be added
(Bayeux, Falaise...).  This is out of the scope of this document which
concentrates on the installation of the basic layer of CadfaelBrew and
Cadfael formulas.

CadfaelBrew is hosted at https://github.com/SuperNEMO-DBD/.

Ben Morgan (Ben.Morgan@warwick.ac.uk) is  the developper and maintener
of CadfaelBrew for the SuperNEMO collaboration.

.. contents::

Preparation
===========

   a. Requirements:

      * A working network connection
      * Bash shell

   b. The working directory:

      In this  report, the ``/data3/sw/SuperNEMO-DBD``  directory will
      be used as  the base working directory to build  and install the
      software.  You are  free to  choose any  other location  on your
      system, provided there is enough space on it (~10 GB).

.. code:: sh

   $ export SNSW_BASE_DIR="/data3/sw/SuperNEMO-DBD"
   $ mkdir -p ${SNSW_BASE_DIR}
   $ cd ${SNSW_BASE_DIR}
   $ git clone https://github.com/SuperNEMO-DBD/brew.git ./CadfaelBrew
   $ export PATH="${SNSW_BASE_DIR}/CadfaelBrew/bin:${PATH}"
   $ export MANPATH="${SNSW_BASE_DIR}/CadfaelBrew/share/man:${MANPATH}"
   $ export INFOPATH="${SNSW_BASE_DIR}/CadfaelBrew/share/info:${INFOPATH}"
..

   c. Install mandatory packages:

.. code:: sh

   bash$ LANG=C sudo apt-get install \
	  build-essential \
	  curl \
	  git \
	  m4 \
	  ruby \
	  texinfo \
	  libbz2-dev \
	  libcurl4-openssl-dev \
	  libexpat-dev \
	  libncurses-dev \
	  zlib1g-dev
..

Prepare the installation
==============================================

The following  steps are  not mandatory. It  depends on  the available
storage  on  your  filesystem.    By  default,  CadfaelBrew  uses  the
``~/.cache/Homebrew`` directory  as the download cache  directory.  If
your home directory has not enough space left, you will need to define
the ``HOMEBREW_CACHE``  environmental variable to another  location of
your filesystem.

Also CadfaelBrew uses the ``/tmp`` directory as the temporary location
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


Build and install CadfaelBrew base software
=====================================================

The  ``cadfael-bootstrap`` formula  is   provided  to   automate  the
installation of CadfaelBrew core software packages (formulas) needed
to build Bayeux and the SuperNEMO software.

a. Brew it!

.. code:: sh

   bash$ brew cadfael-bootstrap
   ...
..

This is a  rather long process, particularly because  the GCC compiler
(version 4.9) is built and installed (~35 min on this system).  Geant4
and Root  are also  rather long  to build.  All  the software  will be
installed   in  ``/data3/sw/CadfaelBrew``.
The full installation uses approximatively 3 GB.

Install  formulas
====================================

Now  CadfaelBrew is  setup, we  can  install useful formulas
on top of which Bayeux will be installed:

.. code:: sh

   bash$ brew sh --cc=gcc-4.9
   bash$ export HOMEBREW_CACHE=/data3/var/cache/Homebrew
   bash$ export HOMEBREW_TEMP=/data3/var/tmp
   bash$ brew install supernemo-dbd/cadfael/boost
   bash$ brew install supernemo-dbd/cadfael/camp
   bash$ brew install supernemo-dbd/cadfael/clhep
   bash$ brew install supernemo-dbd/cadfael/xerces-c
   bash$ brew install supernemo-dbd/cadfael/geant4 --with-opengl-x11
   bash$ brew install supernemo-dbd/cadfael/root5
..

We do not install qt5 here. We will use Ubuntu Qt5 system install.

.. raw:: latex

   \pagebreak
..

Setup CadfaelBrew for your environment
=====================================================

To activate CadfaelBrew in your environement, you have two possibilities:

a. You can use the ``brew.sh`` script provided by CadfaelBrew.
   In your ``~/.bashrc`` startup file, define:

.. code:: sh

   alias brewsh='/data3/sw/SuperNEMO-DBD/CadfaelBrew/bin/brew sh --cc=gcc-4.9'
..

   Then, each time you need to use CadfaelBrew or some software managed by it, you
   just type in your shell:

.. code:: sh

   bash$ brewsh
   bash$ # ... you can now work with brewed software ...
..

   This will start a dedicated shell with all CadfaelBrew software activated.
   To leave this environment, type:

.. code:: sh

   bash$ exit
..


b. Alternatively, you can define a specific Bash function in your ``~/.bashrc`` startup file:

.. code:: sh

   function do_cadfaelbrew_setup()
   {
      if [ -n "${CADFAELBREW_INSTALL_DIR}" ]; then
	  echo "WARNING: CadfaelBrew is already setup !" >&2
	  return 1
      fi
      export CADFAELBREW_INSTALL_DIR="/data3/sw/CadfaelBrew"
      export MANPATH="${CADFAELBREW_INSTALL_DIR}/share/man:${MANPATH}"
      export INFOPATH="${CADFAELBREW_INSTALL_DIR}/share/info:${INFOPATH}"
      mkdir -p /data3/var/cache/Homebrew
      export HOMEBREW_CACHE=/data3/var/cache/Homebrew
      mkdir -p /data3/var/tmp
      export HOMEBREW_TEMP=/data3/var/tmp
      ${CADFAELBREW_INSTALL_DIR}/bin/brew sh --cc=gcc-4.9
      echo "NOTICE: CadfaelBrew is now setup !" >&2
      return
   }
   export -f do_cadfaelbrew_setup
..

   You can thus activate a CadfaelBrew shell :

.. code:: sh

   bash$ do_cadfaelbrew_setup
..

   To leave this environment, type:

.. code:: sh

   bash$ exit
..
