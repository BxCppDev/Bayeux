/// \file geomtools/materials_utils.h
/* Author(s):  François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-03-10
 * Last modified: 2013-03-10
 *
 * License:
 *
 * Description:
 *
 *   Utilities for materials
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_MATERIALS_UTILS_H
#define GEOMTOOLS_MATERIALS_UTILS_H 1

// Standard library:
#include <iostream>

namespace materials {
  class manager;
}

namespace geomtools {

  class gdml_writer;

  void export_gdml (const materials::manager &, gdml_writer & gw_);

  void export_gdml (const materials::manager &, std::ostream & out_);


} // end of namespace geomtools

#endif // GEOMTOOLS_MATERIALS_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
