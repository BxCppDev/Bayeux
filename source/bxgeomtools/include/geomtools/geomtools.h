// -*- mode: c++; -*- 
/* geomtools.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

/*! \mainpage The geomtools package documentation
 *
 * \section intro_section Introduction
 *
 * \subsection pres_subsection Presentation
 *  The \b geomtools package provides some general purpose utility classes
 *  for geometry. It is based on the CLHEP 2D and 3D vectors and rotation 
 *  matrices. It provides some serialization facilities for vector and rotation classes
 *  based on Boost/Serialization (through the \b datatools package).
 *  Some basic 1D, 2D and 3D shapes are implemented with useful features. 
 *
 *  Most parts of the library are compatible with CLHEP and GEANT4.
 *
 *  More, it is possible to build a geometry model in the way the GEANT4
 *  library does with GDLM exporting capability. Some tools are available to:
 *  - dynamically build a tree of geometry models that describes a full 3D-setup from
 *    pure ASCII files
 *  - automate the placement of solid with respect to others thanks to useful automated
 *    placement algorithms
 *  - identify some or all 3D-object of a geometry, assigning to them a
 *    unique \e geometry \e ID
 *  - enable the automated \e mapping between any 3D-object (placement, shapes)
 *    and the associated geometry ID
 *  - display part of the geometry setup using a simple graphics renderer based on gnuplot
 *  - export a geometry setup to GEANT4 using the GDML language
 *  - extends the library by providing new geometry model classes with auto-registration 
 *    in a \e models \e factory
 *
 *  This package depends on:
 *  - the \b datatools library
 *    (https://nemo.lpc-caen.in2p3.fr/wiki/datatools/)  
 *  - the \b CLHEP C++ library (version 2.0 and above)
 *
 * \section contents_section Contents
 *  
 * Many \t \b test_XXXXX.cxx sample programs are available in the \b \t programs directory.
 *
 * The main \b \t include/geomtools/geomtools.h header file contains a few informations about
 * the package's components.
 *
 * \section build_section Installation instructions
 *
 *  1- Configure:
 *
 *     shell> ./pkgtools.d/pkgtool configure
 *
 *  2- Build:
 * 
 *     shell> ./pkgtools.d/pkgtool build
 *
 *  3- Build binaries:
 *
 *     shell> ./pkgtools.d/pkgtool build bin
 *
 *  4- Build test binaries:
 *
 *     shell> ./pkgtools.d/pkgtool build bin_test
 *
 *  5- Build documentation:
 *
 *     shell> ./pkgtools.d/pkgtool doc
 *
 *  6- Installation:
 *
 *     shell> ./pkgtools.d/pkgtool install
 *
 * \section geninfo_section General informations
 *
 *  Wiki repository:
 *   - https://nemo.lpc-caen.in2p3.fr/wiki/geomtools/
 *
 *  SVN repository:
 *   - Base repository: https://nemo.lpc-caen.in2p3.fr/svn/geomtools/
 *   - Development version: https://nemo.lpc-caen.in2p3.fr/svn/geomtools/trunk
 *   - Releases: https://nemo.lpc-caen.in2p3.fr/svn/geomtools/tags/geomtools-X.Y
 *
 *  Author:
 *   - Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 *  Mainteners:
 *   - Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef __geomtools__geomtools_h
#define __geomtools__geomtools_h 1

// Library's version:
#include <geomtools/geomtools_version.h>

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

#endif // __geomtools_geomtools_h

// end of geomtools.h
