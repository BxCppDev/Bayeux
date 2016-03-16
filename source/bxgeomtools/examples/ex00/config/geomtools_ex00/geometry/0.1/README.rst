Virtual geometry setup ``geomtools_ex00`` (version ``0.1``)
==============================================================================

This   README   file   has    been   originally   generated   by   the
``bxgeomtools_mkskelcfg`` script to create a skeleton directory with a
set of geometry configuration files.

Files
-----

 * ``README.rst`` : This file.
 * ``manager.conf`` : The main configuration file for the geometry manager.
 * Folders:

 * ``mapping`` : Contains the files that describe geometry categories.

     * ``lab_categories.lis`` : Contains the definitions of geometry categories
       used by the automated numbering scheme for the mapping of geometry volumes
       through geometry identifiers (GIDs).
     * ``det_categories.lis`` : Contains the definitions of other geometry categories
       used by the automated numbering scheme for the mapping of geometry volumes
       through geometry identifiers (GIDs).
     * ``mapping_rules.def`` : Contains the definitions of mapping rules
       used by the automated numbering scheme for the mapping of daughter
       geometry volumes positionned is the "world" geometry model.
     * ``mapping_rules_2.def`` : Contains the definitions of mapping rules
       used by the automated numbering scheme for the mapping of daughter
       geometry volumes positionned is some other geometry models.

   * ``models`` : Contains the files that describe geometry models.

     * ``world.geom`` : The definition of the top-level geometry volume (world) with
       its daughter physical volumes.
     * ``simple_shaped_models.geom`` : The definitions of some geometry models with
       very simple shapes.
     * ``replica_models.geom`` : The definitions of some geometry models using replicated
       placement techniques.
     * ``assembly_models.geom`` : The  definitions of some geometry models using special
       assembly algorithms.
     * ``meshed_models.geom`` : The  definitions of some geometry models using special
     mesh placement algorithms.
     * ``*.stl`` : ASCII files using the STL (STereo Lithography) format to describe complex
       tesselated shapes. The files have been obtained by exporting
       the descriptions/models of real mechanical parts from the CATIA software framework.
     * ``shapes.defs`` : The definitions of some 3D shapes that can be instantiated by a shape factory
       and used within geometry model.


   * ``plugins`` : Contains the files that describe plugins embedded in the geometry manager.

     * ``mappings.conf`` : The definitions of specific plugins for dedicated geometry mapping.
     * ``materials.conf`` : The definition of a plugin dedicated to the description
       of the materials of which the geometry volumes are made of.
     * ``material_aliases.def`` : The definition of a material aliases used by the material plugin.

Geometry inspector
------------------

Run the Bayeux/geomtools inspector program: ::

  $ cd {ex00 source dir}
  $ export SETUP_CONFIG_DIR=$(pwd)/config
  $ bxgeomtools_inspector \
      --manager-config "${SETUP_CONFIG_DIR}/geomtools_ex00/geometry/0.1/manager.conf"
  geomtools> help
  ...
  geomtools> display
  ...
  geomtools> quit

Clean
-----
In case you have used the Bayeux/geomtools inspector program to generate a GDML file: ::

  $ rm geomtools_ex00-0.1.gdml
