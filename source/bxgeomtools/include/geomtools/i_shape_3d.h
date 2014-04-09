// -*- mode: c++; -*-
/* i_shape_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2012-04-10
 *
 * License:
 *
 * Description:
 *  Interface for 3D shaped volumes
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_SHAPE_3D_H
#define GEOMTOOLS_I_SHAPE_3D_H 1

// Standard library:
#include <string>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>
#include <geomtools/i_stackable.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  /// \brief Mother abstract class for all solid shapes
  class i_shape_3d : public i_object_3d
  {
  public:
    static const double DEFAULT_SKIN;
    static const double USING_PROPER_SKIN;

  public:

    static double get_default_skin ();
    static double get_proper_skin ();

    /* Check if a 3D-shape can be stacked using some
     *  stacking algorithms. There are 2 checks:
     *  check #1: checks if the instance owns a valid
     *  embedded 'stackable_data' instance.
     *  check #2: if check #1 fails, check if the instance inherits
     *  the 'i_stackable' interface.
     */
    static bool is_stackable (const i_shape_3d &);

    static bool is_xmin_stackable (const i_shape_3d &);

    static bool is_xmax_stackable (const i_shape_3d &);

    static bool is_ymin_stackable (const i_shape_3d &);

    static bool is_ymax_stackable (const i_shape_3d &);

    static bool is_zmin_stackable (const i_shape_3d &);

    static bool is_zmax_stackable (const i_shape_3d &);

    /* Initialize a 'stackable_data' instance
     *  from stackable data attached to the 3D-shape.
     */
    static bool pickup_stackable (const i_shape_3d &, stackable_data &);

    static bool pickup_stackable_with_properties (const i_shape_3d & a_shape,
                                                  stackable_data & a_stackable_data);

    /// Return stackable data
    const stackable_data & get_stackable_data () const;

    /// Check if some stackable data are available
    bool has_stackable_data () const;

    /// Set external stackable data
    void set_stackable_data (const stackable_data & a_stackable_data);

    /// Set internal stackable data
    void set_stackable_data (const stackable_data * a_stackable_data);

    /// Reset stackable data
    void reset_stackable_data();

    /// Return the dimension (3)
    virtual int get_dimensional () const;

    /// Return the skin tolerance
    double get_skin () const;

    ///Set the skin tolerance
    void set_skin (double a_skin);

    /// Constructor
    i_shape_3d ();

    /// Constructor with skin tolerance
    i_shape_3d (double a_skin);

    /// Destructor
    virtual ~i_shape_3d ();

    /// Initialize the 3D-shape from properties
    void initialize(const datatools::properties &);

    /// Reset
    void reset();

    /// Check if the solid is composite
    virtual bool is_composite () const;

    /// Check if the number of faces is known
    virtual bool has_number_of_faces() const;

    /// Return the number of faces
    virtual unsigned int get_number_of_faces() const;

    /// Check is the volume is known
    virtual bool has_volume(uint32_t flags_ = 0) const;

    /// Return the volume
    virtual double get_volume(uint32_t flags_ = 0) const;

    /// Check is the surface is known
    virtual bool has_surface(uint32_t flags_ = FACE_ALL_BITS) const;

    /// Return the surface cumulated by faces
    virtual double get_surface(uint32_t flags_ = FACE_ALL_BITS) const;

    /// Check is a point is inside the solid
    virtual bool is_inside (const vector_3d &,
                            double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Check is a point is on a surface of the solid
    virtual bool is_on_surface (const vector_3d &,
                                int a_surface_mask = FACE_ALL_BITS,
                                double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Return the normal at a point of the surface
    virtual vector_3d get_normal_on_surface (const vector_3d & a_position) const = 0;

    /// Check is a point is outside the solid
    virtual bool is_outside (const vector_3d &,
                             double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Find intercept of a segment on the surface of the solid
    virtual bool find_intercept (const vector_3d & a_from,
                                 const vector_3d & a_direction,
                                 intercept_t & a_intercept,
                                 double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Smart print
    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit               = false) const;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  private:

    bool                   _owns_stackable_data_; //!< Ownership flag for stackable data
    const stackable_data * _stackable_data_;      //!< Handle to stackable data

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_SHAPE_3D_H
