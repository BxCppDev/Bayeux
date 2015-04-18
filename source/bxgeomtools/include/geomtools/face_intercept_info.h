/// \file geomtools/face_intercept_info.h
/* Author(s):     François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-03-16
 * Last modified: 2015-03-16
 *
 * License:
 *
 * Description:
 *
 *  The identifier of a face in a shape
 *
 * History:
 *
 */

// Standard library:
#include <iostream>
#include <string>

// This project:
#include <geomtools/face_identifier.h>
#include <geomtools/utils.h>

#ifndef GEOMTOOLS_FACE_INTERCEPT_INFO_H
#define GEOMTOOLS_FACE_INTERCEPT_INFO_H 1

namespace geomtools {

  //! \brief The face_intercept_info class hosts the parameters of
  //         the intercept of a curve on the surface of a shape.
  class face_intercept_info
  {
  public:

    //! Default constructor
    face_intercept_info();

    //! Reset
    void reset();

    //! Return the face identifier
    const face_identifier & get_face_id() const;

   //! Return the face identifier (mutable)
    face_identifier & grab_face_id();

    //! Set the face identifier
    void set_face_id(const face_identifier & fid_);

    //! Set the intercept/impact point
    void set_impact(const vector_3d & point_);

    //! Return the impact point
    const vector_3d & get_impact() const;

    //! Return the impact point (mutable)
    vector_3d & grab_impact();

    //! Check validity of the intercept
    bool is_valid() const;

    //! Intercept point exists
    bool is_ok() const;

    //! Print
    void print(std::ostream & out_, const std::string & indent_ = "") const;

  private:

    face_identifier _face_id_; //!< The identifier of the intercepted face
    vector_3d       _impact_;  //!< The impact point on the intercepted face

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_FACE_INTERCEPT_INFO_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
