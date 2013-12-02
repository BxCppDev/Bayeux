======================
geomtools ``examples/ex01``
======================

Introduction
============

 * Description:

   This  example illustrates  how to  use the event generator manager.
   Also it is shown how to use the ``genbb_inspector`` utility.

 * Source files :

   * ``ex01.cxx`` : the main program

 * Configuration files :

   * ``config/manager.conf`` : the main configuration file of the geometry
     manager.
   * Event generators' configuration files :

     * ``config/backgrounds.conf`` : some event generators
     * ``config/calibrations.conf`` : some event generators
     * ``config/dbd.conf`` : some event generators
     * ``config/misc.conf.conf`` : some event generators
     * ``config/electron_energy_spectrum_0.data`` : data file
       which contains the tabulated energy spectrum used by an electron generator

 * Built object(s) :

     * ``ex01`` : the example executable linked to the ``Bayeux`` library.

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory::

      shell> cp -a $(bxquery --exampledir)/genbb_help/examples/ex01 /tmp/genbb_help_ex01
      shell> cd /tmp/genbb_help_ex01

3. Build and install the example::

      shell> mkdir __build
      shell> cd __build
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
	-DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

4. Run the example::

      shell> ./ex01

5. Run the ``genbb_inspector`` ::

      shell> genbb_inspector \
               --configuration config/manager.conf \
               --action list

      shell> genbb_inspector \
               --configuration config/manager.conf \
               --action shoot \
               --generator "Co60" \
               --prng-seed 314159 \
               --number-of-events 10000 \
               --output-file "histos_Co60.root"

6. Check the output file:

   Output histograms browsable from ROOT via the ``histos_Co60.root`` file ::

      shell> root histos_Co60.root
      root [1] TBrowser b;
      root [2] .q


8. Clean::

      shell> rm ex01
      shell> rm genbb_help-ex01.gdml
      shell> rm -fr __build
