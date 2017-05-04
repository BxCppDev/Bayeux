=============================
geomtools ``examples/ex01``
=============================

Introduction
============

 * Description:

   This  example illustrates  how to  use the geometry manager to
   design a virtual geometry hierarchy based on the ``geomtools``
   library.
   Also it is shown how to use the ``bxgeomtools_inspector`` utility
   to visualize the setup and generate a GDML file usable by Geant4
   and/or browsable by ROOT.

 * Source files :

   * ``ex01.cxx`` : the main program

 * Configuration files :

   * ``config/manager.conf`` : the main configuration file of the geometry
     manager.
   * Geometry models files:

     * ``config/models/world.geom`` : the geometry model that represents
       the top volume (**"world"**).
     * ``config/models/lab.geom`` : the geometry model that represents the
       experimental area where the setup is installed.
     * ``config/models/vessel.geom`` : the geometry models that represent the
       vacuum chamber wherin the detectors and calibration sources are
       installed.
     * ``config/models/optical_module.geom``: the geometry models that represent
       the detection module.
     * ``config/models/shielding.geom`` : the geometry models that represent
       passive shielding walls on both side of the vessel.
     * ``config/models/source.geom`` : the geometry models that represent
       the calibration source.
     * ``config/models/pmt_hamamatsu_R5912MOD_polycone.data`` : the dimensions
       and shape of the photomultiplier tube used by the optical module.

   * ``config/mapping/categories.lis`` : the file that defines the
     list of *geometry categories* used to assign *geometry IDs* to the
     volumes in the hierarchy.
   * Geometry plugins:

     * ``config/plugins/materials_plugin.conf`` : the file that defines the
       *materials* plugin. At least one material plugin must be provided
       to export the geometry hierarchy in a GDML file.
     * ``config/plugins/materials_aliases.def`` : the file that defines the
       *materials* aliases used by this geometry setup.
     * ``config/plugins/mapping_plugins.conf`` : the file that defines some
       *geometry ID mapping* plugins. Mapping plugins are optional.
       A default *mapping* object is built from the rules exposed in the
       ``config/mapping/categories.lis`` file. Additionnal (and specialized)
       *mapping* objects may be optionally provided within plugins.

 * Built object(s) :

     * ``ex01`` : the example executable linked to the ``Bayeux`` library.
       It runs a geometry manager, loaf and build a virtual geometry setup
       and use a Gnuplot renderer for 3D visualization.

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the geomtools library
2. Make a copy of the example directory::

      shell> cp -a $(bxquery --exampledir)/geomtools/examples/ex01 /tmp/geomtools_ex01
      shell> cd /tmp/geomtools_ex01

3. Build and install the example::

      shell> export CONFIG_DIR=$(pwd)/config
      shell> mkdir _build.d
      shell> cd _build.d
      shell> cmake \
        -DCMAKE_INSTALL_PREFIX=.. \
        -DCMAKE_FIND_ROOT_PATH:PATH=$(bxquery --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

4. Run the example::

      shell> ./ex01

5. Run the ``bxgeomtools_inspector`` ::

      shell> bxgeomtools_inspector \
               --manager-config "${CONFIG_DIR}/manager.conf" --visu-view-3d

   It displays views of the setup using the ``geomtools`` Gnuplot viewer.

      .. image:: images/geomtools_ex01_setup_3d.jpg
         :width: 200
         :scale: 25 %
         :alt: The 3D view of the setup (file ``images/geomtools_ex01_setup_3d.jpg``)
         :align: center

      .. image:: images/geomtools_ex01_om_3d.jpg
         :width: 200
         :scale: 25 %
         :alt: The 3D view of the optical module (file ``images/geomtools_ex01_om_3d.jpg``)
         :align: center

6. Check the output ``geomtools`` data file:

     * ROOT display of the setup via the ``geomtools-ex01.gdml`` GDML file ::

         shell> root
         root [0] TGeoManager * geo = new TGeoManager("geo","geomtools examples/ex01 virtual setup");
         root [1] TGeoManager * g2 = geo->Import("geomtools-ex01-1.0.gdml");
         root [2] g2->SetVisOption(0);
         root [3] g2->SetVisLevel(100);
         root [4] g2->GetMasterVolume()->Draw("");
	 root [5] .q

      It displays views of the setup using the ROOT viewer.

      .. image:: images/geomtools_ex01_setup_root_3d.jpg
         :width: 200
         :scale: 25 %
         :alt: The 3D view of the setup (file ``images/geomtools_ex01_setup_root_3d.jpg``)
         :align: center

7. Clean::

      shell> rm ex01
      shell> rm geomtools-ex01.gdml
      shell> rm -fr _build.d
