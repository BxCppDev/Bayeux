======================
geomtools ``examples/ex01``
======================

Introduction
============

 * Description:

   This  example illustrates  how to  use the geometry manager to
   design a virtual geometry hierarchy based on the ``geomtools``
   library.
   Also it is shown how to use the ``geomtools_check_setup`` utility
   to visualize the setup and generate a GDML file usable by Geant4
   and/or browsable by ROOT.

 * Source files :

   * ``ex01.cxx`` : the main program

 * Configuration files :

   * ``config/manager.conf`` : the main configuration file of the geometry
     manager.
   * Geometry models files:

     * ``config/world.geom`` : the geometry model that represents
       the top volume (**"world"**).
     * ``config/lab.geom`` : the geometry model that represents the
       experimental area where the setup is installed.
     * ``config/vessel.geom`` : the geometry models that represent the
       vacuum chamber wherin the detectors and calibration sources are
       installed.
     * ``config/optical_module.geom``: the geometry models that represent
       the detection module.
     * ``config/shielding.geom`` : the geometry models that represent
       passive shielding walls on both side of the vessel.
     * ``config/source.geom`` : the geometry models that represent
       the calibration source.
     * ``config/pmt_hamamatsu_R5912MOD_polycone.data`` : the dimensions
       and shape of the photomultiplier tube used by the optical module.

   * ``config/categories.lis`` : the file that defines the
     list of *geometry categories* used to assign *geometry IDs* to the
     volumes in the hierarchy.
   * Geometry plugins:

     * ``config/materials_plugin.conf`` : the file that defines the
       *materials* plugin. At least one material plugin must be provided
       to export the geometry hierarchy in a GDML file.
     * ``config/mapping_plugins.conf`` : the file that defines some
       *geometry ID mapping* plugins. Mapping plugins are optional.
       A default *mapping* object is built from the rules exposed in the
       ``config/categories.lis`` file. Additionnal (and specialized)
       *mapping* objects may be optionaly provided within plugins.

 * Built object(s) :

     * ``ex01`` : the example executable linked to the ``geomtools`` library.

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
        -Dgeomtools_DIR=$(geomtools-config --prefix) \
        ..
      shell> make
      shell> make install
      shell> cd ..

4. Run the example::

      shell> ./ex01

5. Run the ``geomtools_check_setup`` ::

      shell> geomtools_check_setup \
               --manager-config config/manager.conf \
               --with-gdml \
               --with-visu --visu-view-3d

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
         root [1] TGeoManager * g2 = geo->Import("geomtools-ex01.gdml");
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

8. Clean::

      shell> rm ex01
      shell> rm geomtools-ex01.gdml
      shell> rm -fr __build


