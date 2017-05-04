/// \file geomtools/i_polygon.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-03-22
 * Last modified: 2015-03-22
 *
 * License:
 *
 * Description:
 *
 *   Polygon interface
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_POLYGON_H
#define GEOMTOOLS_I_POLYGON_H 1

// Standard library:
#include <vector>

// This project:
#include <geomtools/utils.h>

namespace geomtools {

  /// \brief Polygon 2D shape
  class i_polygon
  {
  public:

    /// Build an ordered collection of vertexes
    virtual unsigned int compute_vertexes(vertex_col_type & vertexes_) const = 0;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_POLYGON_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
