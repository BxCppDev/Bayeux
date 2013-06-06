======================
geomtools ``examples/ex01``
======================

Introduction
============

 * Description:

   This  example illustrates  how to  use the mctools Geant4 engine to simulate
   radioactive decays from a given location in a virtual geometry and track the
   Monte-Carlo particles.

   It is first shown how to use the ``geomtools_check_setup`` utility
   to visualize the setup and generate a GDML file usable by Geant4
   and browsable by ROOT.

   The event and vertex generators are also tested in a standalone mode
   thanks to the ``genbb_inspector`` and ``genvtx_production`` programs
   respectively.

   Finally the ``g4_production`` program is used to run a Geant4 based
   Monte-Carlo simulation and produce an output simulated data file.

 * Source files :

   * ``ex01.cxx`` : the main program

 * Configuration files :

   * Geometry :

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

   * Event generation:

     * ``config/event_generator/manager.conf`` : The main configuration file for the event
       generator manager (``genbb::manager`` class).
     * ``config/event_generator/backgrounds.conf`` : The definitions of some event generators.
     * ``config/event_generator/calibrations.conf`` : The definitions of some event generators.
     * ``config/event_generator/dbd.conf`` : The definitions of some event generators.
     * ``config/event_generator/misc.conf`` : The definitions of some event generators.

   * Vertex generation:

     * ``config/vertex_generator/manager.conf`` : The main configuration file for the vertex
       generator manager (``gencvtx::manager`` class).
     * ``config/vertex_generator/vertex_generators.conf`` :  The definitions of some vertex generators.

   * Geant4 engine:

     * ``config/manager.conf`` : The main configuration file for the Geant4 simulation engine.
     * ``config/dlls.conf`` : The configuration file for the dynamic library loader.
     * ``config/service_manager.conf`` : The configuration file for the service manager.
     * ``config/services.conf`` : The configuration definitions of some services.
     * ``config/module_manager.conf`` : The configuration file for the module manager.
     * ``config/modules.conf`` : The configuration definitions of some modules.
     * ``config/step_hit_processor_factory.conf`` : The definitions of the *step MC hit processors*
       associated to the sensitive detectors in the geometry.
     * ``config/g4vis.mac`` : A Geant4 visualization macro.



 * Built object(s) : none

 * Build method: CMake.

Quick start
===========

1. Build, install and setup the ``mctools`` library.
2. Make a copy of the example directory::

      shell> cp -a [geomtools install base directory]/share/geomtools/examples/ex01 /tmp/ex01
      shell> cd /tmp/ex01

3. Geometry:

   a. Run the ``geomtools_check_setup`` to check the virtual geometry::

         shell> geomtools_check_setup \
                  --manager-config config/geometry/manager.conf \
                  --with-visu --visu-view-3d \
                  --with-gdml --gdml-to-root-display

   b. ROOT display of the setup via the ``mctools_ex01-1.0.gdml`` GDML file ::

         shell> root
         root [0] TGeoManager * geo = new TGeoManager("geo","mctools examples/ex01 virtual setup");
         root [1] TGeoManager * g2 = geo->Import("mctools_ex01-1.0.gdml");
         root [2] g2->SetVisOption(0);
         root [3] g2->SetVisLevel(100);
         root [4] g2->GetMasterVolume()->Draw("");
	 root [5] .q

4. Event generation:

    a. Show the list of available generators::

         shell> genbb_inspector \
                  --configuration "config/event_generator/manager.conf" \
                  --action "list"

    b. Shoot some primary events from one event generator::

         shell> genbb_inspector \
                  --configuration "config/event_generator/manager.conf" \
                  --action "shoot" \
                  --generator "Co60" \
                  --prng-seed 314159 \
                  --number-of-events 1000 \
                  --output-file "histos_Co60.root"

    c. Display histograms associated to the event kinematics::

         shell> root histos_Co60.root
         root [1] TBrowser b; // then use the GUI to display the histograms
         root [2] .q


5. Vertex generation:

    a. Show the list of available generators::

         shell> genvtx_production \
	         --geometry-manager "config/geometry/manager.conf" \
                 --vertex-generator-manager "config/vertex_generator/manager.conf" \
		 --list


    b. Shoot some random vertex generators and visualize them::

         shell> genvtx_production \
                 --geometry-manager "config/geometry/manager.conf" \
                 --vertex-generator-manager "config/vertex_generator/manager.conf" \
                 --shoot \
	         --number-of-vertices 10000 \
	         --prng-seed 314159 \
	         --vertex-generator "vessel_inner_surface.vg" \
                 --output-file "mctools_ex01_vertices.txt" \
	         --visu --tiny

6. Geant4 simulation:

   a. Run the simulation::

       shell> g4_production \
                --logging-priority "debug" \
		--number-of-events 1000 \
            	--number-of-events-modulo 100 \
            	--interactive \
		--g4-visu \
            	--config "config/manager.conf" \
		--vertex-generator-name "vessel_inner_surface.vg" \
		--vertex-generator-seed 0 \
		--event-generator-name "Co60" \
		--event-generator-seed 0 \
		--shpf-seed 0 \
		--g4-manager-seed 0 \
                --output-prng-seeds-file "prng_seeds.save" \
                --output-prng-states-file "prng_states.save" \
                --output-data-file "mctools_ex01_Co60_inner_vessel.brio"



8. Clean::

      shell> rm mctools_ex01-1.0.gdml
      shell> rm mctools_ex01_vertices.txt
      shell> rm geomtools_check_setup.C
      shell> rm histos_Co60.root
      shell> rm prng_seeds.save
      shell> rm prng_seeds.save.~backup~



