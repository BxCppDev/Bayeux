/// \file geomtools/composite_surface.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-03-17
 * Last modified: 2015-03-17
 *
 * License:
 *
 * Description:
 *
 *  A 2D surface composed of elementary 2D surfaces
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_COMPOSITE_SURFACE_H
#define GEOMTOOLS_COMPOSITE_SURFACE_H 1

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/face_info.h>

namespace geomtools {

  /// \brief The abstract base class for all 2D shapes/surfaces
  class composite_surface : public i_shape_2d
  {
  public:

    /// Return the identifier label for the class
    static const std::string & composite_surface_label();

    /// Return the identifier/name of the shape
    virtual std::string get_shape_name() const;

    /// Check if the surface is valid
    bool is_valid() const;

    /// Default constructor
    composite_surface();

    /// Constructor
    composite_surface(double tolerance_);

    /// Constructor
    composite_surface(double tolerance_, double angular_tolerance_);

    /// Destructor
    virtual ~composite_surface();

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0);

    /// Reset
    void reset();

    /// Return the number of embedded surfaces
    unsigned int get_number_of_surfaces() const;

    /// Return the array of surface items
    const face_info_collection_type & get_surfaces() const;

    /// Add a surface
    face_info & add();

    /// Remove last surface
    void remove_last();

    /// Get the surface item at given index
    const face_info & get(unsigned int index_) const;

    /// Return the surface
    virtual double get_surface(uint32_t flags_ = ALL_PIECES) const;

    /// Return the perimeter
    virtual double get_perimeter(uint32_t flags_ = ALL_PIECES) const;

    /// Check is a given point belongs to the surface of the 2D shape
    ///
    /// @arg position_ The position to be checked
    /// @arg skin_ The tolerance (effective thickness of the surface)
    /// @return true if the checked position belongs to the surface
    virtual bool is_on_surface(const vector_3d & position_,
                               double skin_ = GEOMTOOLS_PROPER_TOLERANCE
                               ) const;

    /// Return the normal direction at some position on the 2D shape's path
    ///
    /// @arg position_ The position to be checked
    /// @return the normal 3D vector at the checked position that is assumes
    ///         belonging to the surface
    virtual vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = false,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Find the intercept point with one of the component face
    virtual bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Smart print
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const;

    /// Generate a sequence of polylines for wires 3D rendering
    virtual void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const;

  private:

    face_info_collection_type _surfaces_; //!< Array of surface components with their shapes and positionings

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_COMPOSITE_SURFACE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
