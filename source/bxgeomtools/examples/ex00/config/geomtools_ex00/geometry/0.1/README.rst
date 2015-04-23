Virtual geometry setup ``geomtools_ex00`` (version ``0.1``)
==============================================================================

This   README   file  has   been   automatically   generated  by   the
``bxgeomtools_mkskelcfg`` script to describe a skeleton directory with a
set of geometry configuration  files.  From this sample directory,
you can now build your own virtual geometry setup.

Files
-----

 * ``README.rst`` : This file.
 * ``manager.conf`` : The main configuration file for the geometry manager.
 * Folders:

   * ``gids`` : Contains the files that describe geometry categories.

     * ``lab_categories.lis`` : Contains the definition of geometry categories
       used by the automated numbering scheme for the mapping of geometry volumes
       through geometry identifiers (GIDs).
     * ``det_categories.lis`` : Contains the definition of other geometry categories
       used by the automated numbering scheme for the mapping of geometry volumes
       through geometry identifiers (GIDs).

   * ``models`` : Contains the files that describe geometry models.

     * ``source.geom`` : The definition of a source geometry model.
     * ``detector.geom`` : The definition of a detector geometry model.
     * ``world.geom`` : The definition of the top-level geometry volume (world) and
       its daughter physical volumes (source and detectors).
     * ``geom_models.lis`` : The list of files of geometry models (alternative setup
       property in the ``manager.conf`` file).

   * ``plugins`` : Contains the files that describe plugins for the geometry manager.

     * ``mappings.conf`` : The definition of plugins for dedicated geometry mapping.
     * ``materials.conf`` : The definition of a plugin dedicated to the description
       of the materials of which the geometry volumes are made of.
     * ``material_aliases.def`` : The definition of a material aliases used by the material plugin.

Geometry inspector
------------------

Run the Bayeux/geomtools inspector program: ::

  $ cd {ex00 source dir}
  $ SETUP_CONFIG_DIR=$(pwd)/config \
    bxgeomtools_inspector \
      --manager-config "${SETUP_CONFIG_DIR}/geomtools_ex00/geometry/0.1/manager.conf"
