// -*- mode: c++; -*-
/// \file geomtools/i_wires_drawer.h
/* Author(s):     F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-02-07
 * Last modified: 2015-02-07
 *
 * License:
 *
 * Description:
 *
 *   Base class for all shape drawer in wires mode
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_WIRES_DRAWER_H
#define GEOMTOOLS_I_WIRES_DRAWER_H 1

// Standard library:
#include <iostream>

// This project:
#include <geomtools/utils.h>

namespace geomtools {

  // Forward declaration:
  // class i_object_3d;

  //! \brief Base class for all shape drawer objects in wires mode
  class i_wires_drawer
  {
  public:
    //! Default constructor
    i_wires_drawer();

    //! Destructor
    virtual ~i_wires_drawer();

    //! Generation of wires data in a stream
    virtual void generate_wires(std::ostream & out_,
                                const geomtools::vector_3d & position_,
                                const geomtools::rotation_3d & rotation_) = 0;

    //! Generation of wires data in a stream
    void generate_wires_self(std::ostream & out_);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_WIRES_DRAWER_H
