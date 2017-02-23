=============================================
 Installation of Cadfaelbrew on MacOS 10.9.5
=============================================

:Author: F.Mauger, C.Augier
:Date:   2017-02-20

This note reports how we have built and installed Cadfaelbrew
on a MacOS 10.9.5 system for the EDELWEISS-DM experiment.

Links:

  * https://github.com/SuperNEMO-DBD/brew.git

Preparation
-----------

1. Clone the Cadfaelbrew Git repository from GitHub:

   a. Prepare a working directory:

.. code:: sh

   $ mkdir -p ${HOME}/softs/EDELWEISS
   $ cd ${HOME}/softs/EDELWEISS
..

   b. Download the CadfaelBrew sources:

.. code:: sh

   $ git clone https://github.com/SuperNEMO-DBD/brew.git CadfaelBrew
..

   c. Activate the Cadfael ``brew`` command:

.. code:: sh

   $ export PATH="${HOME}/softs/EDELWEISS/CadfaelBrew/bin:${PATH}"
..

2. Configuration of the LPC Caen proxy for curl (specific to LPC Caen):

   This is a special setup to process the installation from behind
   a network proxy:

.. code:: sh

   $ export http_proxy=http://caeproxy.in2p3.fr:3128/
   $ export https_proxy=https://caeproxy.in2p3.fr:p3128/
   ..

3. Installation of XQuartz 2.7.11:

.. code:: sh

   $ brew cask reinstall xquartz
   Password: ***
   ...
..

4. Booststrap Cadfaelbrew:

.. code:: sh

   $ brew update
   $ brew cadfael-bootstrap
   ...
..

Installation
-------------

1. Install core packages:

.. code:: sh

   $ brew install supernemo-dbd/cadfael/boost
   $ brew install supernemo-dbd/cadfael/camp
   $ brew install supernemo-dbd/cadfael/clhep
   $ brew install supernemo-dbd/cadfael/xerces-c
..

2. Geant4.9:

   Cannot access the ``http://geant4.cern.ch/support/source/geant4.9.6.p04.tar.gz`` tarball. It seems the base URL has changed to:

   ``http://geant4.web.cern.ch/geant4/support/source/``

   Workaround: edit the Geant4 formula :

   ``./Library/Taps/supernemo-dbd/homebrew-cadfael/Formula/geant4.rb``.

   a. change the URL to:

.. code:: ruby

   stable do
     url "http://geant4.web.cern.ch/geant4/support/source/geant4.9.6.p04.tar.gz"
..

   b. Inhibit automatic download/installation of Geant4 data files:

.. code:: ruby

   args << "-DGEANT4_INSTALL_DATA=OFF"
   # args << "-DGEANT4_INSTALL_DATA_TIMEOUT=86400" if build.with? "notimeout"
..

   c. Installation:

.. code:: sh

   $ brew install supernemo-dbd/cadfael/geant4
..

   d. Manual installation of Geant4 data files:

.. code:: sh

   $ mkdir -p ${HOME}/softs/EDELWEISS/CadfaelBrew/Cellar/geant4/9.6.4_4/share/Geant4-9.6.4/data
   $ cd  ${HOME}/softs/EDELWEISS/CadfaelBrew/Cellar/geant4/9.6.4_4/share/Geant4-9.6.4/data
   $ wget http://geant4.web.cern.ch/geant4/support/source/G4NDL.4.2.tar.gz
   $ wget http://geant4.web.cern.ch/geant4/support/source/G4EMLOW.6.32.tar.gz
   $ wget http://geant4.web.cern.ch/geant4/support/source/G4PhotonEvaporation.2.3.tar.gz
   $ wget http://geant4.web.cern.ch/geant4/support/source/G4RadioactiveDecay.3.6.tar.gz
   $ wget http://geant4.web.cern.ch/geant4/support/source/G4NEUTRONXS.1.2.tar.gz
   $ wget http://geant4.web.cern.ch/geant4/support/source/G4PII.1.3.tar.gz
   $ wget http://geant4.web.cern.ch/geant4/support/source/RealSurface.1.0.tar.gz
   $ wget http://geant4.web.cern.ch/geant4/support/source/G4SAIDDATA.1.1.tar.gz
   $ tar xzf G4NDL.4.2.tar.gz
   $ tar xzf G4NEUTRONXS.1.2.tar.gz
   $ tar xzf G4PII.1.3.tar.gz
   $ tar xzf G4PhotonEvaporation.2.3.tar.gz
   $ tar xzf G4RadioactiveDecay.3.6.tar.gz
   $ tar xzf G4SAIDDATA.1.1.tar.gz
   $ tar xzf RealSurface.1.0.tar.gz
   $ rm -f *.tar.gz     # <=== optional
..

3. Root5:

.. code:: sh

   $ brew install xz
   $ brew install pcre
   $ brew install supernemo-dbd/cadfael/root5
..

4. Additional software:

.. code:: sh

   $ brew install gnuplot --with-cairo --with-wxmac
..

Setup
-----

In your ``.bashrc`` :

.. code:: sh

   echo >&2 "[info] Type 'brewsh' to enter Cadfaelbrew shell..."
   export CADFAELBREW_HOME_DIR="${HOME}/softs/EDELWEISS/CadfaelBrew"
   function do_cadfaelbrew_shell()
   {
     if [ -n "${CADFAELBREW_INSTALL_DIR}" ]; then
       echo >&2 "[warning] do_cadfaelbrew_shell: Cadfaelbrew is already setup !"
       return 1
     fi
     export CADFAELBREW_INSTALL_DIR="${HOME}/softs/EDELWEISS/CadfaelBrew"
     export PATH="${CADFAELBREW_INSTALL_DIR}/bin:${PATH}"
     export MANPATH="${CADFAELBREW_INSTALL_DIR}/share/man:${MANPATH}"
     export INFOPATH="${CADFAELBREW_INSTALL_DIR}/share/info:${INFOPATH}"
     echo >&2 "[info] do_cadfaelbrew_shell: Entering Cadfaelbrew shell... (type 'exit' or [Ctrl-D] to quit)"
     ${CADFAELBREW_INSTALL_DIR}/bin/brew sh
     unset CADFAELBREW_INSTALL_DIR
     echo >&2 "[info] do_cadfaelbrew_shell: Cadfaelbrew shell is terminated."
     return 0
   }
   export -f do_cadfaelbrew_shell
   alias brewsh="do_cadfaelbrew_shell"
..
