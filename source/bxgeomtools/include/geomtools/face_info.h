// \file geomtools/face_info.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-03-17
 * Last modified: 2015-03-17
 *
 * License:
 *
 * Description:
 *
 *  Face information: 2D surface and positioning
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_FACE_INFO_H
#define GEOMTOOLS_FACE_INFO_H 1

// Standard library:
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/i_tree_dump.h>

// This project:
#include <geomtools/placement.h>
#include <geomtools/face_identifier.h>
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  //! \brief Information about the nature and positioning
  //!        of a face of the solid
  class face_info : public datatools::i_tree_dumpable
  {
  public:

    //! Default constructor
    face_info();

    //! Destructor
    ~face_info() override;

    //! Check the validity
    bool is_valid() const;

    //! Check if a face identifier is available
    bool has_identifier() const;

    //! Check if a face is available
    bool has_face() const;

    //! Check if a positioning is available
    bool has_positioning() const;

    //! Check if a label is available
    bool has_label() const;

    //! Set the 2D face handle
    void set_face_ref(i_shape_2d *);

    //! Set the 2D face handle
    void set_face(const face_handle_type &);

    //! Set the 2D face with explicit type
    template <class Shape2D>
    Shape2D & add_face()
    {
      Shape2D * face_ptr = new Shape2D;
      _face_.reset(face_ptr);
      return *face_ptr;
    }

    //! Reset the face
    void reset_face();

    //! Return the face
    const i_shape_2d & get_face_ref() const;

    //! Return the face (mutable)
    i_shape_2d & grab_face_ref();

    //! Set the face identifier
    void set_identifier(const face_identifier &);

    //! Return the face identifier
    const face_identifier & get_identifier() const;

    //! Return the face identifier (mutable)
    face_identifier & grab_identifier();

    //! Reset the face identifier
    void reset_identifier();

    //! Return the face positioning
    const placement & get_positioning() const;

    //! Return the face positioning (mutable)
    placement & grab_positioning();

    //! Force face positioning to identity
    void set_identity_positioning();

    //! Reset face positioning
    void reset_positioning();

    //! Return the face label
    const std::string & get_label() const;

    //! Set face label
    void set_label(const std::string &);

    //! Reset face label
    void reset_label();

    //! Check the explicit type of the face
    template <class Shape2D>
    bool is_a() const
    {
      // std::cerr << "DEVEL: face_info::is_a<>: " << "BEGIN" << std::endl;
      try {
        dynamic_cast<const Shape2D &>(_face_.get());
        return true;
      }
      catch (...) {
      }
      // std::cerr << "DEVEL: face_info::is_a<>: " << "END" << std::endl;
      return false;
    }

    //! Return a the explicitely typed face
    template <class Shape2D>
    const Shape2D & get_face() const
    {
      return dynamic_cast<const Shape2D &>(_face_.get());
    }

    //! Return a the explicitely typed face (mutable)
    template <class Shape2D>
    Shape2D & grab_face()
    {
      return dynamic_cast<Shape2D &>(_face_.grab());
    }

    //! Reset
    void reset();

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  private:

    face_identifier  _identifier_;  //!< Face identifier
    face_handle_type _face_;        //!< Handle to a 2D surface shape
    placement        _positioning_; //!< Placement of the face
    std::string      _label_;       //!< Face label

  };

  //! Type alias for a collection of face information objects
  typedef std::vector<face_info> face_info_collection_type;

  //! Smart print
  void print_face_infos(const face_info_collection_type &,
                        std::ostream & out_,
                        const std::string & title_ = "",
                        const std::string & indent_ = "");


} // end of namespace geomtools

#endif // GEOMTOOLS_FACE_INFO_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
