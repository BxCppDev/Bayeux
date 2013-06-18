========================
genvtx ``examples/ex01``
========================

Introduction
============

 * Description:

   This  example illustrates  how to  use the *vertex generator manager*
   coupled to a *geometry manager* (from the ``geomtools`` library)
   to shoot random vertices.
   Also it is shown how to use the ``genvtx_production`` utility.

 * Source files :

   * ``ex01.cxx`` : the main program

 * Configuration files :

   * Geometry (from the ``config/geometry/`` directory) :

     * ``manager.conf``      : the main configuration file of the geometry manager.
     * ``categories.lis``    : the file that defines the
       list of *geometry categories* used to assign *geometry IDs* to the
       volumes in the hierarchy.
     * ``setup.geom``        : geometry models files.
     * ``geom_plugins.conf`` : geometry plugins (*material driver* plugin
       and *mapping* plugins).
     * ``materials.def``     : definition of materials for the *material driver* plugin

   * Vertex generation (from the ``config/vertex/`` directory) : :

    * ``manager.conf``      : the main configuration file of the vertex generator manager.
    * ``vertex_generators.def`` : the file that defines the available
       vertex generators.

 * Built object(s) :

    * ``ex01`` : the example executable linked to the ``genvtx`` library.

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the geomtools library
2. Make a copy of the example directory::

      shell> cp -a [geomtools install base directory]/share/geomtools/examples/ex01 /tmp/ex01
      shell> cd /tmp/ex01

3. Build and install the example::

      shell> mkdir __build
      shell> cd __build
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -Dgenvtx_DIR=$(genvtx-config --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

4. Run the example::

      shell> ./ex01

5. Run the ``geomtools_inspector`` to check the virtual geometry::

         shell> geomtools_inspector \
                  --manager-config config/geometry/manager.conf \
                  --with-visu --visu-view-3d


5. Run the ``genvtx_production``:

   * List the available vertex generators::

      shell> genvtx_production \
               --geometry-manager config/geometry/manager.conf \
               --vertex-generator-manager config/vertex/manager.conf \
               --list


   * Generate vertices::

      shell> genvtx_production \
               --geometry-manager config/geometry/manager.conf \
               --vertex-generator-manager config/vertex/manager.conf \
               --shoot \
	       --number-of-vertices 10000 \
	       --prng-seed 314159 \
	       --vertex-generator "all_mezzanine_pcb_bulk.vg" \
               --output-file "genvtx_ex01_vertices.txt" \
	       --visu --tiny


6. Check the output ``geomtools`` data file:

     * ROOT display of the setup via the ``geomtools-ex01.gdml`` GDML file ::

         shell> root
         root [0] TGeoManager * geo = new TGeoManager("geo","geomtools examples/ex01 virtual setup");
         root [1] TGeoManager * g2 = geo->Import("geomtools-ex01.gdml");
         root [2] g2->SetVisOption(0);
         root [3] g2->SetVisLevel(100);
         root [4] g2->GetMasterVolume()->Draw("");
	 root [5] .q

8. Clean::

      shell> rm ex01
      shell> rm geomtools-ex01.gdml
      shell> rm -fr __build


