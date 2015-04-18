/// \file geomtools/intercept_info.h
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

// This project:
#include <geomtools/face_identifier.h>

#ifndef GEOMTOOLS_INTERCEPT_INFO_H
#define GEOMTOOLS_INTERCEPT_INFO_H 1

namespace geomtools {

  //! \brief The intercept_data_type class hosts the parameters of the intercept of a curve on the surface of a shape.
  class intercept_info
  {
  public:

    //! Return the face identifier
    const face_identifier & get_face_id() const;

    //! Set the face identifier
    void set_face_id(const face_identifier & fid_);

    //! Return the index of the sub-shape that hosts the intercept/impact point
    int get_shape_index() const;

    //! Set the index of the sub-shape that hosts the intercept/impact point
    void set_shape_index(int si_);

    //! Return the index of the face that hosts the intercept/impact point
    int get_face() const;

    //! Set the index of the face that hosts the intercept/impact point
    void set_face(int face_);

    //! Set the intercept/impact point
    void set_impact(const vector_3d & point_);

    //! Set intercept data
    void set(int shape_index_, int face_, const vector_3d & point_);

    //! Return the impact point
    const vector_3d & get_impact() const;

    //! Reset
    void reset();

    //! Default constructor
    intercept_info();

    //! Check validity of the intercept
    bool is_valid() const;

    //! Intercept point exists
    bool is_ok() const;

    //! Print
    void print(std::ostream & out_, const std::string & indent_ = "") const;

  private:

    face_identifier _face_id_; //!< The identifier of the intercepted face
    int32_t   _shape_index_; //!< The index of the sub-shape for composite shapes only (default is 0)
    int32_t   _face_;        //!< The index of the impact face on the shape
    vector_3d _impact_;      //!< The impact point on the surface

  };

  //! @deprecated Deprecated type alias (@see intercept_data_type)
  typedef intercept_info  intercept_data_type;

} // end of namespace geomtools

#endif // G
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
EOMTOOLS_INTERCEPT_INFO_H
