=========================
cuts ``examples/manager``
=========================

Introduction
============

 * Description :

   This example illustrates the use of the ``cuts::cut_manager`` class
   with some *cut* classes on an arbitrary user data type.

 * Source files :

   * ``data.h`` and ``data.cc`` : sample user data class header file
   * ``color_cut.h`` and  ``color_cut.cc``: a cut class that selects
     ``data`` object with a specific color
   * ``range_cut.h`` and ``range_cut.cc``: a cut class that selects
     ``data`` object with a coordinate in a specific range
   * ``sphere_cut.h`` and ``sphere_cut.cc`` : a cut class that
     selects ``data`` object with a position in the volume of a specific sphere
   * ``ex_manager.cxx`` : the main program
   * ``ex_manager.gp`` : Gnuplot macro for plotting results

 * Configuration files :

   * ``config/manager.conf`` : The cut manager main configuration file
   * ``config/cuts.def`` : The definition and configuration parameters
     of the cuts

 * Build method : CMake

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory:

.. code:: sh

   shell> cp -a $(bxquery --exampledir)/cuts/manager /tmp/cuts_ex_manager
   shell> cd /tmp/cuts_ex_manager
..

3. Build and install the example:

.. code:: sh

   shell> mkdir _build.d
   shell> cd _build.d
   shell> cmake \
	  -DCMAKE_INSTALL_PREFIX=../_install.d \
	  -DBayeux_DIR:PATH=$(bxquery --cmakedir) \
	  ..
   shell> make
   shell> make install
   shell> cd ..
..

4. Run the example:

.. code:: sh

   shell> ./_install.d/ex_manager
   shell> gnuplot ex_manager.gp
..

5. Check the output file:

   * ``ex_manager.orig.data`` : 10 % of the original set of randomized
     colored data points
   * ``ex_manager.selected.data`` : Final set of selected colored
     data points

6. Clean:

.. code:: sh

   shell> rm -f ex_manager.orig.data
   shell> rm -f ex_manager.selected.data
   shell> rm -fr _install.d
   shell> rm -fr _build.d
..
