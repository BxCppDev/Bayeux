/// \file geomtools/i_shape_1d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2012-04-10
 *
 * License:
 *
 * Description:
 *  Interface for 1D shaped volumes (lines)
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_SHAPE_1D_H
#define GEOMTOOLS_I_SHAPE_1D_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/ocd_macros.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>

namespace geomtools {

  /// \brief The abstract base class for all 1D curves/shapes
  class i_shape_1d : public i_object_3d
  {
  public:

    /// Return the dimension of the object
    int get_dimensional() const override;

    /// Check if the 1D shape knows its number of associated paths
    virtual bool has_number_of_paths() const;

    /// Return the number of paths
    virtual unsigned int get_number_of_paths() const;

    /// Check if the 1D shape known its length
    virtual bool has_length(uint32_t flags_ = PATH_ALL_BITS) const;

    /// Return the length of the 1D shape
    virtual double get_length(uint32_t flags_ = PATH_ALL_BITS) const;

    /// Default constructor
    i_shape_1d();

    /// Constructor
    i_shape_1d(double tolerance_);

    /// Constructor
    i_shape_1d(double tolerance_, double angular_tolerance_);

    /// Destructor
    ~i_shape_1d() override;

    /// Check is a given point belongs to the path of the 1D shape
    virtual bool is_on_curve(const vector_3d &,
                             double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Return the tangent direction at some position on the 1D shape's path
    virtual vector_3d get_direction_on_curve(const vector_3d & position_) const = 0;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

    // Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // end of namespace geomtools

/*
// Explicit class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::i_shape_1d, 0)
*/

#endif // GEOMTOOLS_I_SHAPE_1D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
