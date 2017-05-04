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

    * ``ex01`` : the example executable linked to the ``Bayeux`` library.

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the Bayeux library
2. Make a copy of the example directory:

.. code:: sh

      shell> cp -a $(bxquery --exampledir)/genvtx/examples/ex01 /tmp/genvtx_ex01
      shell> cd /tmp/genvtx_ex01
..

3. Build and install the example:

.. code:: sh

      shell> mkdir _build.d
      shell> cd _build.d
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=../_install.d \
	-DBayeux_DIR=$(bxquery --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..
..

4. Run the example:

.. code:: sh

      shell> ./_install.d/ex01
..

5. Run the ``bxgeomtools_inspector`` to check the virtual geometry:

.. code:: sh

   shell> bxgeomtools_inspector \
                  --manager-config config/geometry/manager.conf \
                  --with-visu --visu-view-3d
..


5. Run the ``bxgenvtx_production``:

   * List the available vertex generators:

.. code:: sh

   shell> bxgenvtx_production \
               --geometry-manager config/geometry/manager.conf \
               --vertex-generator-manager config/vertex/manager.conf \
               --list
..


   * Generate vertices:

.. code:: sh

   shell> bxgenvtx_production \
               --geometry-manager config/geometry/manager.conf \
               --vertex-generator-manager config/vertex/manager.conf \
               --shoot \
	       --number-of-vertices 10000 \
	       --prng-seed 314159 \
	       --vertex-generator "all_mezzanine_pcb_bulk.vg" \
               --output-file "genvtx_ex01_vertices.txt" \
	       --visu
..

   * Generate vertices and serialize a 'display data' object containing
     some vertexes:

.. code:: sh

   shell> bxgenvtx_production \
               --geometry-manager config/geometry/manager.conf \
               --vertex-generator-manager config/vertex/manager.conf \
               --shoot \
	       --number-of-vertices 10000 \
	       --prng-seed 314159 \
	       --vertex-generator "all_mezzanine_pcb_bulk.vg" \
               --output-file "genvtx_ex01_vertices.txt" \
	       --visu-max-counts 100 \
	       --visu-spot-color "magenta" \
	       --visu-output-file "vertices-dd-magenta.data.gz"
..

 .. code:: sh

    shell> bxgenvtx_production \
               --geometry-manager config/geometry/manager.conf \
               --vertex-generator-manager config/vertex/manager.conf \
               --shoot \
	       --number-of-vertices 10000 \
	       --prng-seed 314159 \
	       --vertex-generator "all_mezzanine_pcb_bulk.vg" \
               --output-file "genvtx_ex01_vertices.txt" \
	       --visu-max-counts 30 \
	       --visu-spot-color "red" \
	       --visu-output-file "vertices-dd-red.data.gz"
..

   * Redisplay geometry plus vertexes:

.. code:: sh

   shell> bxgeomtools_inspector \
	  --manager-config config/geometry/manager.conf
   geomtools> ldd -n "vtx-red" -i "vertices-dd-red.data.gz"
   geomtools> ldd -n "vtx-magenta" -i "vertices-dd-magenta.data.gz"
   geomtools> pdd
   List of embedded display data :
   vtx-magenta
   vtx-red

   geomtools> d [1200:6.1]

   Press [Enter] to continue...
   geomtools> d -xy world

   Press [Enter] to continue...
   geomtools> d -xy -nodd world
   geomtools> cdd
   geomtools> q
..


7. Clean:

.. code:: sh

   shell> rm -fr _install.d
   shell> rm -fr _build.d
..
