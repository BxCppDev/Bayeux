// -*- mode: c++; -*-
/// \file geomtools/geomtools.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-28
 * Last modified: 2010-10-28
 *
 * License:
 *
 * Description:
 *  The main header file from the 'geomtools' package.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_GEOMTOOLS_H
#define GEOMTOOLS_GEOMTOOLS_H 1

// Library's version:
#include <geomtools/geomtools_config.h>

// Basic utilities:
#include <geomtools/clhep.h> /** Interface to CLHEP class material:
                              *   - 2D and 3D vector,
                              *   - rotation matrix,
                              *   - system of units,
                              *   - physical constants
                              */

#include <geomtools/units.h> /** The 'datatools' package provides the tools to manipulate units (from CLHEP) */

#include <geomtools/utils.h> /** Basic functions and constants */

// Handling of specific properties:
#include <geomtools/color.h>
#include <geomtools/visibility.h>
#include <geomtools/sensitive.h>
#include <geomtools/material.h>

// Miscellaneous:
#include <geomtools/intersection.h>  /* Data structure that describes the intersecting point between
                                      * some surface and a segment */
#include <geomtools/i_stackable.h>   /* Interface for solids that may be stacked with others */

// Interfaces for 1D, 2D and 3D geometrical objects:
#include <geomtools/i_object_3d.h>  /* 3D-object main interface */
#include <geomtools/i_shape_1d.h>   /* 1D-shape interface */
#include <geomtools/i_shape_2d.h>   /* 2D-shape interface */
#include <geomtools/i_shape_3d.h>   /* 3D-shape interface */

// Logical and physical volume (using some similar approach than in GEANT4)
#include <geomtools/logical_volume.h>   /* Logical volume (ala GEANT4) */
#include <geomtools/physical_volume.h>  /* Physical volume (ala GEANT4) */

// Useful 3D solids (GDML/GEANT compatible):
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/tube.h>
#include <geomtools/sphere.h>
#include <geomtools/polycone.h>
#include <geomtools/polyhedra.h>
#include <geomtools/tessellation.h>
//#include <geomtools/hexagon_box.h>

// Useful 3D composite solids (GDML/GEANT compatible):
#include <geomtools/i_composite_shape_3d.h> /* Interface for solid shapes made of
                                             * a logical combination of 2 shapes
                                             */
#include <geomtools/subtraction_3d.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/union_3d.h>

// 1D or 2D useful shapes:
#include <geomtools/polyline_3d.h>
#include <geomtools/disk.h>
#include <geomtools/circle.h>
#include <geomtools/rectangle.h>
#include <geomtools/helix_3d.h>
#include <geomtools/line_3d.h>
#include <geomtools/regular_polygon.h>

// Placements:
#include <geomtools/i_placement.h>              /* Interface for virtual placement objects */
#include <geomtools/placement.h>                /* Basic (simple) unique placement with
                                                 * one translation and one rotation
                                                 */
#include <geomtools/multiple_placement.h>       /* Placement object that handles several
                                                 * arbitrary placements for several 3D-objects
                                                 */
#include <geomtools/regular_grid_placement.h>   /* Placement object that handles several
                                                 * placements for several 3D-objects
                                                 * using a regular 2D-grid pattern
                                                 */
#include <geomtools/regular_linear_placement.h>  /* Placement object that handles several
                                                  * placements for several 3D-objects
                                                  * using a regular 1D-grid pattern
                                                  */

// Geometry models:
#include <geomtools/i_model.h>       /* the main interface for virtual geometry model objects */
#include <geomtools/i_boxed_model.h> // maybe obsolete thanks to the new 'i_stackable' concept
#include <geomtools/model_with_internal_items_tools.h> /* Tools to place several geometry model objects
                                                        * within a given geometry mother model
                                                        */

// The model factory:
#include <geomtools/model_factory.h> /* A manager that can dynamically build a full tree of geometry models
                                      * using directives from properties files (ASCII)
                                      */

// Some useful pre-defined models:
#include <geomtools/rotated_boxed_model.h>
#include <geomtools/simple_boxed_model.h>
#include <geomtools/simple_shaped_model.h>
#include <geomtools/stacked_model.h>
#include <geomtools/surrounded_boxed_model.h>
#include <geomtools/multiple_items_model.h>
#include <geomtools/replicated_boxed_model.h>
#include <geomtools/cylindric_extrusion_boxed_model.h>
#include <geomtools/simple_world_model.h>

// IDs and mapping:
#include <geomtools/mapping_utils.h>
#include <geomtools/geom_id.h>
#include <geomtools/geom_info.h>
#include <geomtools/geom_map.h>
#include <geomtools/mapping.h>
#include <geomtools/id_mgr.h>
#include <geomtools/address_set.h>
#include <geomtools/id_selector.h>

// Locators:
#include <geomtools/i_locator.h>        // Geometry locator interface
#include <geomtools/smart_id_locator.h> // A smart geometry locator

// GDML:
#include <geomtools/gdml_writer.h>    // GDML streaming primitives
#include <geomtools/gdml_export.h>    // GDML export manager

// Gnuplot renderer:
#include <geomtools/gnuplot_draw.h>   // Gnuplot streaming primitives
#include <geomtools/gnuplot_drawer.h> // Gnuplot renderer manager
#include <geomtools/gnuplot_i.h>      // On the fly gnuplot rendering utilities

#endif // GEOMTOOLS_GEOMTOOLS_H
