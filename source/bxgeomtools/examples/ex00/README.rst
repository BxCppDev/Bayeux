=============================
geomtools ``examples/ex00``
=============================

Introduction
============

 * Description:

   This  example illustrates how to create a virtual geometry setup
   using primitives provided by the Bayeux/geomtools library and
   used by the geometry manager to design a virtual geometry hierarchy.

   Also it is shown how to use the ``bxgeomtools_inspector`` utility
   to visualize the setup and generate a GDML file usable by Geant4
   and/or browsable through Gnuplot or ROOT.


 * Configuration files :

   * ``config/geomtools_ex00/geometry/0.1/`` : the main configuration
     directory that hosts all files used to store configuration parameters
     describing the setup. Among them the ``manager.conf`` is the
     top-level configuration file used by a geometry manager.

     Note: this directory has been originally created using the
     ``bxgeomtools_mkskelcfg`` script: ::

      $ bxgeomtools_mkskelcfg \
          --setup-label "geomtools_ex00" \
          --setup-version "0.1" \
          --config-base-dir "config"

     Then the resulting base configuration directory has been filled
     with other configuration files.


Quick start
===========

1. Build, install and setup the Bayeux/geomtools library
2. Make a copy of the example directory and move to this duplicated directory::

      $ cp -a $(bxquery --exampledir)/geomtools/examples/ex00 /tmp/geomtools_ex00
      $ cd /tmp/geomtools_ex00

3. Run the ``bxgeomtools_inspector`` ::

      $ export SETUP_CONFIG_DIR=$(pwd)/config
      $ bxgeomtools_inspector \
         --manager-config ${SETUP_CONFIG_DIR}/geomtools_ex00/geometry/0.1/manager.conf

   where the ``SETUP_CONFIG_DIR`` environment variable represents the base source directory
   of the ``ex00`` example.

Documentation
=============

You can generate documentation HTML pages about the geomtools primitive classes
using the following command: ::

      $ bxocd_make_doc -n geomtools -B
