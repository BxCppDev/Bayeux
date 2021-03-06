=========================
mctools ``examples/ex01``
=========================

Introduction
============

 * Description:

   This example illustrates how to use the mctools Geant4 engine (from
   the ``mctools_g4``  library) to  simulate Co-60  radioactive decays
   from  a  given  location  in  a  virtual  geometry  and  track  the
   Monte-Carlo particles.

   It is first shown how  to use the ``bxgeomtools_inspector`` utility
   to visualize  the setup and generate  a GDML file usable  by Geant4
   and browsable by ROOT.

   The event  and vertex  generators are also  tested in  a standalone
   mode      thanks     to      the     ``bxgenbb_inspector``      and
   ``bxgenvtx_production`` programs respectively.

   The  ``bxg4_production`` program  is  used to  run  a Geant4  based
   Monte-Carlo simulation  and produce  an output simulated  data file
   both  in interactive  mode (with  online Geant4  visualization) and
   non-interactive mode.

   A  sample program  is provided  to browse  (ASCII mode)  the output
   simulated data  files which  contains plain  object records  of the
   ``mctools::simulated_data`` class.

   Finally the ``bxdpp_processing`` program is  used to run the Geant4
   simulation through a data  processing pipeline, using the dedicated
   data processing module class ``mctools::g4::simulation_module``.

 * Source files :

   * ``ex01_inspector.h``  and ``ex01_inspector.cc``  : the  simulated
     data inspector class.

   * ``ex01_read_plain_simdata.cxx``  : the  program  to browse  plain
     simulated   data    archive   files    as   generated    by   the
     ``g4_production`` program.

   * ``ex01_read_pipeline_simdata.cxx``  :   the  program   to  browse
     simulated   data    archive   files    as   generated    by   the
     ``dpp_processing`` program.

 * Configuration files :

   * Geometry :

     * ``config/geometry/manager.conf`` : the main configuration file of the geometry
       manager.
     * Geometry models files:

       + ``config/geometry/world.geom`` : the geometry model that represents
         the top volume (*world*).
       + ``config/geometry/lab.geom`` : the geometry model that represents the
         experimental area where the setup is installed.
       + ``config/geometry/vessel.geom`` : the geometry models that represent the
         vacuum chamber wherin the detectors and calibration sources are
         installed.
       + ``config/geometry/optical_module.geom``: the geometry models that represent
         the detection module (sensitive volume).
       + ``config/geometry/probe.geom``: the geometry model that represents
         a detection probe (sensitive volume).
       + ``config/geometry/shielding.geom`` : the geometry models that represent
         passive shielding walls on both side of the vessel.
       + ``config/geometry/source.geom`` : the geometry models that represent
         the calibration source.
       + ``config/geometry/pmt_hamamatsu_R5912MOD_polycone.data`` : the dimensions
         and shape of the photomultiplier tube used by the optical module.

     * ``config/geometry/categories.lis`` : the file that defines the
       list of *geometry categories* used to assign *geometry IDs* to the
       volumes in the hierarchy.
     * Geometry plugins:

       + ``config/geometry/materials_plugin.conf`` : the file that defines the
         *materials* plugin. At least one material plugin must be provided
         to export the geometry hierarchy in a GDML file.
       + ``config/geometry/mapping_plugins.conf`` : the file that defines some
         *geometry ID mapping* plugins. Mapping plugins are optional.
         A default *mapping* object is built from the rules exposed in the
         ``config/geometry/categories.lis`` file. Additionnal (and specialized) *mapping*
         objects may be optionaly provided within plugins.
       + ``config/geometry/magnetic_field_plugin.conf`` : the file that defines
         the *electro magnetic fields* plugin. One such plugin is provided, it relies on
         the following files:

         - ``config/geometry/magnetic_field_manager.conf`` : the main configuration file for the electromagnetic field manager.
         - ``config/geometry/magnetic_fields.conf`` : the definitions of the magnetic field modelized in this setup.

   * Event generation:

     * ``config/event_generator/manager.conf`` : The main configuration file for the event
       generator manager (``genbb::manager`` class).
     * ``config/event_generator/backgrounds.conf`` : The definitions of some event generators.
     * ``config/event_generator/calibrations.conf`` : The definitions of some event generators.
     * ``config/event_generator/dbd.conf`` : The definitions of some event generators.
     * ``config/event_generator/misc.conf`` : The definitions of some event generators.

   * Vertex generation:

     * ``config/vertex_generator/manager.conf`` : The main configuration file for the vertex
       generator manager (``genvtx::manager`` class).
     * ``config/vertex_generator/vertex_generators.conf`` :  The definitions of some vertex generators.

   * Geant4 simulation:

     * Plain simulation (``g4_processing``):

       * ``config/simulation/manager.conf`` : The main configuration file for the Geant4 simulation engine.
       * ``config/simulation/step_hit_processor_factory.conf`` : The definitions of the *step MC hit processors*
         associated to the sensitive detectors in the geometry.
       * ``config/simulation/geant4_visualization.mac`` : A Geant4 visualization macro (for interactive mode only).

     * Simulation through the data processing pipeline (``dpp_processing``):

       * ``config/pipeline/dlls.conf`` : the list of shared libraries to be dynamically loaded.
       * ``config/pipeline/module_manager.conf`` : the main configuration file of
         the *data processing module manager* embeded in the ``dpp_processing`` program.
       * ``config/pipeline/service_manager.conf`` : the main configuration of the *service manager* embeded in the
         ``dpp_processing``program and used by the *module manager*.
       * ``config/pipeline/services.conf`` :  the definitions of the *services* used by the *data processing modules*.
         Here we define the *Geometry service* which wraps the *geometry manager*
         initialized from the ``config/geometry/manager.conf`` file (see above).
       * ``config/pipeline/modules.conf`` : the definitions of the *data processing modules* used along the pipeline
         Here we use only one *simulation module* which wraps the *Geant4 simulation manager* initialized
         from the ``config/simulation/manager.conf`` file.

 * Built object(s) :

     * ``ex01_read_plain_simdata`` : the executable linked against the ``mctools_g4`` library
       and other Boost I/O libraries (``datatools_bio``, ``geomtools_bio`` and ``mctools_bio``).

     * ``ex01_read_pipeline_simdata`` : another executable linked against the ``mctools_g4`` library
       and other Boost I/O libraries (``datatools_bio``, ``geomtools_bio`` and ``mctools_bio``).

 * Build method: CMake.


Quick start
===========

1. Build, install and setup the ``mctools`` library.
2. Make a copy of the example directory::

      $ cp -a [mctools install base directory]/share/mctools/examples/ex01 /tmp/ex01
      $ cd /tmp/ex01

3. Build and install the example program::

      $ mkdir _build.d
      $ cd _build.d
      $ cmake \
        -DCMAKE_INSTALL_PREFIX=../_install.d \
        -DBayeux_DIR=$(bxquery --cmakedir) \
        ..
      $ make
      $ make install
      $ cd ..

4. Standalone Geometry:

   a. Run the ``bxgeomtools_inspector`` to check the virtual geometry::

         $ export CONFIG_DIR="$(pwd)/config"
         $ bxgeomtools_inspector \
                  --manager-config "${CONFIG_DIR}/geometry/manager.conf" \
                  --with-visu --visu-view-3d
         geomtools> help
         geomtools> display -3d world
         geomtools> display [1000:0]
         geomtools> display [2020:0.0]
         geomtools> export_gdml
         geomtools> quit

      It displays views of the setup using the ``geomtools`` Gnuplot viewer :

       * The 3D view of the setup:

         .. image:: images/ex01_geometry_1.jpg
            :width: 200
            :scale: 25 %
            :alt: The 3D view of the setup (file ``images/ex01_geometry_1.jpg``)
            :align: center

       * The XY-view (top) of the setup:

         .. image:: images/ex01_geometry_2.jpg
            :width: 200
            :scale: 25 %
            :alt: The 2D view of the setup (file ``images/ex01_geometry_2.jpg``)
            :align: center

   b. ROOT display of the setup via the ``mctools_ex01-1.0.gdml`` GDML file: ::

         $ root
         root [0] TGeoManager * geo = new TGeoManager("geo","mctools examples/ex01 virtual setup");
         root [1] TGeoManager * g2 = geo->Import("mctools_ex01-1.0.gdml");
         root [2] g2->SetVisOption(0);
         root [3] g2->SetVisLevel(100);
         root [4] g2->GetMasterVolume()->Draw("");
         root [5] .q

      It displays a 3D view of the setup using the ROOT visualization tool:

      .. image:: images/ex01_geometry_3.jpg
         :width: 200
         :scale: 25 %
         :alt: The OpenGL 3D view of the setup from ROOT (file ``images/ex01_geometry_3.jpg``)
         :align: center

5. Standalone event generation:

    a. Show the list of available generators::

         $ bxgenbb_inspector \
                  --configuration "${CONFIG_DIR}/event_generator/manager.conf" \
                  --action "list"

    b. Shoot some primary events from one event generator::

         $ bxgenbb_inspector \
                  --configuration "${CONFIG_DIR}/event_generator/manager.conf" \
                  --action "shoot" \
                  --generator "Co60" \
                  --prng-seed 314159 \
                  --number-of-events 1000 \
                  --output-file "histos_Co60.root"

    c. Display histograms associated to the event kinematics::

         $ root histos_Co60.root
         root [1] TBrowser b; // then use the GUI to display the histograms
         root [2] .q


       It displays some histograms related to the kinematics of the Co-60 decay:

       * The first prompt electron energy spectrum in the Co-60 decay:
          .. image:: images/ex01_genbb_Co60_prompt_beta_energy_1.jpg
             :width: 200
             :scale: 25 %
             :alt: The first prompt electron energy spectrum in the Co-60 decay (file ``images/ex01_genbb_Co60_prompt_beta_energy_1.jpg``)
             :align: center

       * The first prompt gamma energy spectrum in the Co-60 decay:

          .. image:: images/ex01_genbb_Co60_prompt_gamma_energy_0.jpg
             :width: 200
             :scale: 25 %
             :alt: The first prompt gamma energy spectrum in the Co-60 decay (file ``images/ex01_genbb_Co60_prompt_gamma_energy_0.jpg``)
             :align: center

       * The second prompt gamma energy spectrum in the Co-60 decay:

          .. image:: images/ex01_genbb_Co60_prompt_gamma_energy_1.jpg
             :width: 200
             :scale: 25 %
             :alt: The second prompt gamma energy spectrum in the Co-60 decay (file ``images/ex01_genbb_Co60_prompt_gamma_energy_1.jpg``)
             :align: center

6. Standalone vertex generation:

    a. Show the list of available generators::

         $ bxgenvtx_production \
                 --geometry-manager "${CONFIG_DIR}/geometry/manager.conf" \
                 --vertex-generator-manager "${CONFIG_DIR}/vertex_generator/manager.conf" \
                 --list


    b. Shoot some random vertex generators and visualize them::

         $ bxgenvtx_production \
                 --geometry-manager "${CONFIG_DIR}/geometry/manager.conf" \
                 --vertex-generator-manager "${CONFIG_DIR}/vertex_generator/manager.conf" \
                 --shoot \
                 --number-of-vertices 10000 \
                 --prng-seed 314159 \
                 --vertex-generator "vessel_inner_surface.vg" \
                 --output-file "mctools_ex01_vertices.txt" \
                 --visu --tiny

      It displays  a 3D view  of the setup  with the positions  of the
      generated vertexes on the inner surface of the vacuum vessel :

      .. image:: images/ex01_vertex_generator_vessel_inner_surface.jpg
         :width: 200
         :scale: 25 %
         :alt: The generated vertexes on the surface of the vacuum chamber inner volume (file ``images/ex01_vertex_generator_vessel_inner_surface.jpg``)
         :align: center


    c. Another random vertex generators::

         $ bxgenvtx_production \
                 --geometry-manager "${CONFIG_DIR}/geometry/manager.conf" \
                 --vertex-generator-manager "${CONFIG_DIR}/vertex_generator/manager.conf" \
                 --shoot \
                 --number-of-vertices 10000 \
                 --prng-seed 314159 \
                 --vertex-generator "all_probe_rings_inner_surface.vg" \
                 --output-file "mctools_ex01_vertices.txt" \
                 --visu --tiny

       It displays the generated vertexes  on the inner surface of the
       probe rings:

        .. image:: images/ex01_vertex_generator_probe_rings_inner_surface.jpg
           :width: 200
           :scale: 25 %
           :alt: The generated vertexes on the inner surface of the probe rings (file ``images/ex01_vertex_generator_probe_rings_inner_surface.jpg``)
           :align: center

    d. Yet another random vertex generators::

         $ bxgenvtx_production \
                 --geometry-manager "${CONFIG_DIR}/geometry/manager.conf" \
                 --vertex-generator-manager "${CONFIG_DIR}/vertex_generator/manager.conf" \
                 --shoot \
                 --number-of-vertices 100 \
                 --prng-seed 314159 \
                 --vertex-generator "source_0_bulk.vg" \
                 --output-file "mctools_ex01_vertices.txt" \
                 --visu --tiny

       It  displays the  generated vertexes  from the  bulk of  source
       number 0 (on the right of the set of 3 sources):

        .. image:: images/ex01_vertex_generator_source_0_bulk.jpg
           :width: 200
           :scale: 25 %
           :alt: The generated vertexes from the bulk of source 0 (file ``images/ex01_vertex_generator_source_0_bulk.jpg``)
           :align: center

7. Geant4 simulation:

    a. Run the  simulation through  a Geant4 interactive  session with
       visualization.   Here  we  activate the  ``"all_visu"``  output
       profile  that stores  the  true hits  from  various volumes  of
       interest in the geometry (scintillator blocks, the inner volume
       of the vessel and sources): ::

         $ bxg4_production \
                --logging-priority "warning" \
                --number-of-events-modulo 1 \
                --interactive \
                --g4-visu \
                --config "${CONFIG_DIR}/simulation/manager.conf" \
                --vertex-generator-name "source_0_bulk.vg" \
                --vertex-generator-seed 0 \
                --event-generator-name "Co60" \
                --event-generator-seed 0 \
                --shpf-seed 0 \
                --g4-manager-seed 0 \
                --output-profiles "all_visu" \
                --output-prng-seeds-file "prng_seeds.save" \
                --output-prng-states-file "prng_states.save" \
                --output-data-file "mctools_ex01_Co60_source_0_bulk.xml" \
                --g4-macro "${CONFIG_DIR}/simulation/geant4_visualization.macro"

       We can  also activate the ``"full_truth"``  output profile that
       stores the true hits from all volumes in the geometry: ::

         $ bxg4_production \
                --logging-priority "warning" \
                --number-of-events-modulo 1 \
                --interactive \
                --g4-visu \
                --config "${CONFIG_DIR}/simulation/manager.conf" \
                --vertex-generator-name "source_0_bulk.vg" \
                --vertex-generator-seed 0 \
                --event-generator-name "Co60" \
                --event-generator-seed 0 \
                --shpf-seed 0 \
                --g4-manager-seed 0 \
                --output-profiles "full_truth" \
                --output-prng-seeds-file "prng_seeds.save" \
                --output-prng-states-file "prng_states.save" \
                --output-data-file "mctools_ex01_Co60_source_0_bulk.xml" \
                --g4-macro "${CONFIG_DIR}/simulation/geant4_visualization.macro"

      From the Geant4 interactive session::

         Idle> /vis/viewer/set/viewpointThetaPhi -60 45
         Idle> /run/beamOn 1
         Idle> exit

      It displays the virtual geometry setup using the Geant4 visualization driver.

      .. image:: images/ex01_g4_production_0.jpg
         :width: 200
         :scale: 25 %
         :alt: The Geant4 visualization of a Co-60 decay with two gamma rays and an electron emitted from a source film (file ``images/ex01_g4_production_0.jpg``)
         :align: center

      Then browse the output plain simulated data file: ::

         $ ls -l mctools_ex01_Co60_source_0_bulk.xml
         $ ./ex01_read_plain_simdata \
                 --logging-priority "notice" \
                 --interactive \
                 --with-visualization \
                 --input-file "mctools_ex01_Co60_source_0_bulk.xml"

      It displays a 3D view of simulated decays:

      .. image:: images/ex01_inspector_0.jpg
         :width: 200
         :scale: 25 %
         :alt: The Geant4 visualization of a Co-60 decay with two gamma rays and an electron emitted from a source film (file ``images/ex01_g4_production_0.jpg``)
         :align: center


    b. Run the simulation in non-interactive mode::

         $ bxg4_production \
                --logging-priority "warning" \
                --number-of-events 10 \
                --number-of-events-modulo 2 \
                --batch \
                --config "${CONFIG_DIR}/simulation/manager.conf" \
                --vertex-generator-name "source_0_bulk.vg" \
                --vertex-generator-seed 0 \
                --event-generator-name "Co60" \
                --event-generator-seed 0 \
                --shpf-seed 0 \
                --g4-manager-seed 0 \
                --output-profiles "all_visu" \
                --output-prng-seeds-file "prng_seeds.save" \
                --output-prng-states-file "prng_states.save" \
                --output-data-file "mctools_ex01_Co60_source_0_bulk.data.gz"

       Then browse the output plain simulated data file ::

         $ ls -l mctools_ex01_Co60_source_0_bulk.data.gz
         $ ./ex01_read_plain_simdata \
                 --logging-priority "notice" \
                 --interactive \
                 --with-visualization \
                 --input-file "mctools_ex01_Co60_source_0_bulk.data.gz"

    c. Run the geant4 simulation through the data processing pipeline::

         $ bxdpp_processing \
          --logging-priority "notice" \
          --dlls-config "${CONFIG_DIR}/pipeline/dlls.conf" \
          --module-manager-config "${CONFIG_DIR}/pipeline/module_manager.conf" \
          --max-records 1000 \
          --modulo 100 \
          --module "Co60@source_0_bulk" \
          --output-file "mctools_ex01_Co60_source_0_bulk.dpp.brio"


       The output data file uses the Brio format and stores the
       simulated data within ``datatools::things`` object records::

         $ ls -l mctools_ex01_Co60_source_0_bulk.dpp.brio
         $ ./ex01_read_pipeline_simdata \
                 --logging-priority "notice" \
                 --interactive \
                 --with-visualization \
                 --dump-data-record \
                 --dump-simulated-data \
                 --dump-hits \
                 --input-file "mctools_ex01_Co60_source_0_bulk.dpp.brio"

8. Clean::

      $ rm -f ex01_read_pipeline_simdata
      $ rm -f ex01_read_plain_simdata
      $ rm -f geomtools_inspector.C
      $ rm -f histos_Co60.root
      $ rm -f mctools_ex01-1.0.gdml
      $ rm -f mctools_ex01_Co60_source_0_bulk.data.gz
      $ rm -f mctools_ex01_Co60_source_0_bulk.dpp.brio
      $ rm -f mctools_ex01_Co60_source_0_bulk.xml
      $ rm -f mctools-ex01_README.html
      $ rm -f mctools_ex01_vertices.txt
      $ rm -f prng_seeds.save*
      $ rm -f prng_states.save*
      $ rm -fr lib
      $ rm -fr _build.d
