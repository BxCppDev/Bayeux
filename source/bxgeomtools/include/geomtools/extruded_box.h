// -*- mode: c++; -*-
/// \file geomtools/extruded_box.h
/* Author(s):     Arnaud Chapon <chapon@lpccaen.in2p3.fr>
 *                François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-10-27
 * Last modified: 2015-10-27
 *
 * License:
 *
 * Description:
 *  Interface:
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_EXTRUDED_BOX_H
#define GEOMTOOLS_EXTRUDED_BOX_H 1

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/object_configuration_description.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>
#include <geomtools/i_wires_3d_rendering.h>
#include <geomtools/i_wires_drawer.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  /// \brief The 3D shape model for an extruded box
  class extruded_box : public i_shape_3d ,
                       public i_stackable,
                       public i_wires_3d_rendering
  {

  public:

    /// Return the extruded_box shape label
    static const std::string & extruded_box_label();

    ///  \brief Masks used for the 6 faces of the extruded_box
    enum faces_mask_type {
      FACE_NONE          = geomtools::FACE_NONE,
      FACE_BACK          = datatools::bit_mask::bit00,
      FACE_FRONT         = datatools::bit_mask::bit01,
      FACE_LEFT          = datatools::bit_mask::bit02,
      FACE_RIGHT         = datatools::bit_mask::bit03,
      FACE_BOTTOM        = datatools::bit_mask::bit04,
      FACE_TOP           = datatools::bit_mask::bit05,
      FACE_INSIDE_BACK   = datatools::bit_mask::bit06,
      FACE_INSIDE_FRONT  = datatools::bit_mask::bit07,
      FACE_INSIDE_LEFT   = datatools::bit_mask::bit08,
      FACE_INSIDE_RIGHT  = datatools::bit_mask::bit09,
      FACE_INSIDE_BOTTOM = datatools::bit_mask::bit10,
      FACE_INSIDE_TOP    = datatools::bit_mask::bit11,
      FACE_ALL           = (FACE_BACK
                            | FACE_FRONT
                            | FACE_LEFT
                            | FACE_RIGHT
                            | FACE_BOTTOM
                            | FACE_TOP
                            | FACE_INSIDE_BACK
                            | FACE_INSIDE_FRONT
                            | FACE_INSIDE_LEFT
                            | FACE_INSIDE_RIGHT
                            | FACE_INSIDE_BOTTOM
                            | FACE_INSIDE_TOP)
    };

  public:

    /// Return the min X coordinates (bounding box)
    double get_xmin() const;

    /// Return the max X coordinates (bounding box)
    double get_xmax() const;

    /// Return the min Y coordinates (bounding box)
    double get_ymin() const;

    /// Return the max Y coordinates (bounding box)
    double get_ymax() const;

    /// Return the min Z coordinates (bounding box)
    double get_zmin() const;

    /// Return the max Z coordinates (bounding box)
    double get_zmax() const;

    /// Return the X dimension
    double get_x () const;

    /// Set the X dimension
    void set_x(double);

    /// Return the half the X dimension
    double get_half_x() const;

    /// Set the half the X dimension
    void set_half_x(double);

    /// Return the Y dimension
    double get_y() const;

    /// Set the Y dimension
    void set_y(double);

    /// Return the half the Y dimension
    double get_half_y() const;

    /// Set the half the Y dimension
    void set_half_y(double);

    /// Return the Z dimension
    double get_z() const;

    /// Set the Z dimension
    void set_z(double);

    /// Return the half the Z dimension
    double get_half_z() const;

    /// Set the half the Z dimension
    void set_half_z(double);

    /// Set all dimensions
    void set(double, double, double, double);

    /// Set the halves of all dimensions
    void set_half(double, double, double, double);

    /// Return the thickness of extruded box
    double get_thickness() const;

    /// Set the thickness of extruded box
    void set_thickness(double);

    /// Return if extruded box has a bottom
    bool has_bottom() const;

    /// Set a bottom to the extruded box
    void set_bottom(bool);

    /// Add a bottom to the extruded box
    void add_bottom();

    /// Remove the bottom of the extruded box
    void remove_bottom();

    /// Return if extruded box has a top
    bool has_top() const;

    /// Set a top to the extruded box
    void set_top(bool);

    /// Add a top to the extruded box
    void add_top();

    /// Remove the top of the extruded box
    void remove_top();

  public:

    /// Default constructor
    extruded_box();

    /// Constructor
    extruded_box(double x_, double y_, double z_, double t_,
                 bool has_top_ = true, bool has_bottom_ = true);

    /// Destructor
    virtual ~extruded_box();

    /// Return the name of the shape
    virtual std::string get_shape_name() const;

    // Check the validity of the object
    bool is_valid() const;

    /// Initialize the extruded_box from properties
    virtual void initialize(const datatools::properties &,
                            const handle_dict_type *);

    /// Reset
    virtual void reset();

    /// Return the value of some parameter given by name
    double get_parameter(const std::string &) const;

    /// Return the cumulated surface of the extruded_box given a list of faces
    virtual double get_surface(uint32_t mask_ = FACE_ALL_BITS) const;

    /// Return the volume of the extruded_box
    virtual double get_volume(uint32_t flags_ = 0) const;

    /// Check if a point is inside the extruded_box
    virtual bool is_inside(const vector_3d &,
                           double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check if a point is inside the extruded_box
    virtual bool is_outside(const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check if a point is on some surface of the extruded_box
    /// If 'skin' < 0 no skin is taken into account.
    virtual bool is_on_surface(const vector_3d &,
                               int mask_    = FACE_ALL_BITS,
                               double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Return the vector normal to the surface at some position
    virtual vector_3d get_normal_on_surface(const vector_3d & position_) const;

    /// Print operator
    friend std::ostream & operator<<( std::ostream &, const extruded_box & );

    /// Read operator
    friend std::istream & operator>>( std::istream &, extruded_box & );

    /// Find the intercept point of a segment with the extruded_box
    virtual bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                intercept_t & intercept_,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Smart print
    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const;

    /// Generate a list of polylines representing the contour of the shape (for display clients)
    virtual void generate_wires(std::list<polyline_3d> &,
                                const placement &,
                                uint32_t options_ = 0) const;

    /// \brief Special Gnuplot rendering
    struct wires_drawer : public i_wires_drawer
    {
      //! Constructor
      wires_drawer(const extruded_box & eb_);
      //! Destructor
      virtual ~wires_drawer();
      //! Output
      virtual void generate_wires(std::ostream & out_,
                                  const geomtools::vector_3d & position_,
                                  const geomtools::rotation_3d & rotation_);
    private:
      const extruded_box * _exbox_; //!< Handle to the target solid
      boost::scoped_ptr<std::list<polyline_3d> > _wires_ptr_; //!< Local cache
    };

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default attributes
    void _set_default();

  private:

    double _x_;          //!< Width (in arbitrary units)
    double _y_;          //!< Length (in arbitrary units)
    double _z_;          //!< Height (in arbitrary units)
    double _thickness_;  //!< Thickness (in arbitrary units)
    bool   _has_top_;    //!< Flag if extruded box has a top
    bool   _has_bottom_; //!< Flag if extruded box has a bottom

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(extruded_box);

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::extruded_box the name the class with registered OCD support
DOCD_CLASS_DECLARATION(geomtools::extruded_box)

#endif // GEOMTOOLS_EXTRUDED_BOX_H
