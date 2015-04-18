/* i_find_intercept.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-02-27
 * Last modified: 2015-02-27
 *
 * License:
 *
 * Description:
 *
 *  Interface for an object for which the find_intercept method
 *  is provided (2D and 3D shapes)
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_FIND_INTERCEPT_H
#define GEOMTOOLS_I_FIND_INTERCEPT_H 1

// This project:
#include <geomtools/utils.h>
#include <geomtools/face_identifier.h>
#include <geomtools/face_intercept_info.h>

namespace geomtools {

  // Forward declaration:
  class placement;

  /// \brief Abstract interface for all objects for which we can compute the intercept
  ///        with a ray emitted from a point
  class i_find_intercept
  {
  public:

    /// Default constructor
    i_find_intercept();

    /// Destructor
    virtual ~i_find_intercept();

    /// Find intercept on some surface from a segment/ray emitted from a point
    ///
    /// This method implies we work in the local coordinate system of the target object
    /// @arg a_from the position from which the ray is emitted
    /// @arg a_direction the direction of the emitted ray
    /// @arg a_intercept the data structure that contains the result of the
    ///      intercept algorithm (impact point)
    /// @return true if a valid intercept/impact has been found
    virtual bool find_intercept(const vector_3d & a_from,
                                const vector_3d & a_direction,
                                face_intercept_info & a_intercept,
                                double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Find intercept on some surface from a segment/ray emitted from a point
    ///
    /// This method implies we work in the local coordinate system of the target object
    /// @arg a_from the position from which the ray is emitted
    /// @arg a_direction the direction of the emitted ray
    /// @arg a_intercept the data structure that contains the result of the
    ///      intercept algorithm (impact point)
    /// @return true if a valid intercept/impact has been found
    bool find_intercept_self(const vector_3d & a_from,
                             const vector_3d & a_direction,
                             face_intercept_info & a_intercept,
                             double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Find intercept on some surface from a segment/ray emitted from a point
    ///
    /// This method implies we work in the global coordinate system of the target object
    /// @arg a_from the position from which the ray is emitted
    /// @arg a_direction the direction of the emitted ray
    /// @arg a_placement the positioning of the target object on which we search
    ///      the intercept
    /// @arg a_intercept the data structure that contains the result of the
    ///      intercept algorithm (impact point)
    /// @return true if a valid intercept/impact has been found
    bool find_intercept(const vector_3d & a_from,
                        const vector_3d & a_direction,
                        const placement & a_placement,
                        face_intercept_info & a_intercept,
                        double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_FIND_INTERCEPT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
