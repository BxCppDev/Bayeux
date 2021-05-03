// \file geomtools/i_shape_3d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2017-02-22
 *
 * Description:
 *
 *  Interface for all 3D shaped volumes
 *
 */

#ifndef GEOMTOOLS_I_SHAPE_3D_H
#define GEOMTOOLS_I_SHAPE_3D_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/utility.hpp>
#include <boost/optional.hpp>
// - Bayeux/datatools:
#include <datatools/handle.h>
#include <datatools/ocd_macros.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/bounding_data.h>
#include <geomtools/i_find_intercept.h>
#include <geomtools/face_info.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  // Forward class declaration:
  class i_shape_2d;

  /// \brief Mother abstract class for all 3D solid shapes
  class i_shape_3d
    : public i_object_3d
    , public i_wires_3d_rendering
    , public i_find_intercept
  {
  public:

    static const double DEFAULT_SKIN;
    static const double ZERO_SKIN;
    static const double USING_PROPER_SKIN;

    /// Return the default skin value
    static double get_default_skin();

    /// Return the zero skin value (0 == no thickness)
    static double get_zero_skin();

    /// Return the special skin value that tells to use the intrinsic skin of the shape itself (< 0)
    static double get_proper_skin();

    // Return the property key for the volume property
    static const std::string & volume_key();

    // Return the property key for the surface property
    static const std::string & surface_key();

    /* Check if a 3D-shape can be stacked using some
     *  stacking algorithms. There are 2 checks:
     *  check #1: checks if the instance owns a valid
     *  embedded 'stackable_data' instance.
     *  check #2: if check #1 fails, check if the instance inherits
     *  the 'i_stackable' interface.
     */
    // static bool is_stackable(const i_shape_3d &);

    // Check if the shape is natively stackable (i.e. inherits the 'geomtools::i_stackable' interface)
    bool is_native_stackable() const;

    /// Initialize a 'stackable_data' instance from stackable data attached to the 3D-shape
    static bool pickup_stackable(const i_shape_3d &, stackable_data &);

    /// Check the stackability of the shape (from native or enforced stackable infos)
    static bool check_stackability(const i_shape_3d &, const stackable::stackability_mode);

    /// Return the enforced stackable data associated to the shape
    const stackable_data & get_stackable_data() const;

    /// Check if some enforced stackable data are available
    bool has_stackable_data() const;

    /// Check if some enforced stackable data are available and owned by the shape itself
    bool owns_stackable_data() const;

    /// Set external enforced stackable data
    void set_stackable_data(const stackable_data & a_stackable_data);

    /// Set internal enforced stackable data
    void set_stackable_data(const stackable_data * a_stackable_data);

    /// Reset enforced stackable data
    void reset_stackable_data();

    /// Return the dimension (3)
    int get_dimensional() const override;

    /// Return the effective skin tolerance associated to the 3D shape
    double get_skin(double a_skin) const;

    /// Return the intrinsic skin tolerance
    double get_skin() const;

    /// Set the intrinsic skin tolerance
    void set_skin(double a_skin);

    /// Constructor
    i_shape_3d();

    /// Constructor with skin tolerance
    i_shape_3d(double a_skin);

    /// Constructor
    i_shape_3d(double a_skin_, double angular_tolerance_);

    /// Copy constructor
    i_shape_3d(const i_shape_3d &);

    /// Assignment operator
    i_shape_3d & operator=(const i_shape_3d &);

    /// Destructor
    ~i_shape_3d() override;

    /// Check if the solid is composite
    bool is_composite() const override;

    /// Check if the face identification scheme is based on face bits
    virtual bool using_face_id_bits() const;

    /// Check if the face identification scheme is based on face index
    virtual bool using_face_id_index() const;

    /// Check if the face identification scheme uses part index
    virtual bool using_face_id_part_index() const;

    /// Build a face identifier any mask
    virtual void make_any_face(face_identifier &) const;

    /// Check if the number of faces is known
    virtual bool has_number_of_faces() const;

    /// Return the number of faces
    virtual unsigned int get_number_of_faces() const;

    /// Check if a forced volume can be set
    virtual bool volume_can_be_forced() const;

    /// Check is a forced volume is set
    virtual bool has_forced_volume() const;

    /// Force the volume
    void set_forced_volume(double volume_);

    /// Return the forced volume
    double get_forced_volume() const;

    /// Check is the volume is known
    virtual bool has_volume(uint32_t flags_ = 0) const;

    /// Return the volume
    virtual double get_volume(uint32_t flags_ = 0) const;

    /// Return the total effective volume
    double get_effective_volume() const;

    /// Check is the surface is known
    virtual bool has_surface(uint32_t flags_ = face_identifier::FACE_BITS_ANY) const;

    /// Return the surface cumulated by faces
    virtual double get_surface(uint32_t flags_ = face_identifier::FACE_BITS_ANY) const;

    /// Check if a point is inside the solid
    bool check_inside(const vector_3d & position_,
                      double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check if a point is outside the solid
    bool check_outside(const vector_3d & position_,
                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check is a point is located on a given surface
    face_identifier check_surface(const vector_3d & position_,
                                  const face_identifier & a_surface_mask = face_identifier::FACE_BITS_ANY,
                                  double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check if a point is on the surface of the solid
    bool check_surface(const vector_3d & position_,
                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Primitive check is a point is inside the solid
    virtual bool is_inside(const vector_3d & position_,
                           double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Primitive check is a point is outside the solid
    virtual bool is_outside(const vector_3d &,
                            double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Primitive check is a point is located on a given surface
    virtual face_identifier on_surface(const vector_3d & position_,
                                       const face_identifier & a_surface_mask = face_identifier::FACE_BITS_ANY,
                                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Return the normal at a point of the surface given its surface identifier
    virtual vector_3d get_normal_on_surface(const vector_3d & a_position,
                                            const face_identifier & a_surface_bit) const = 0;

    /// Check is a point is on a surface of the solid
    bool is_on_surface(const vector_3d & position_,
                       const face_identifier & a_surface_mask = face_identifier::FACE_BITS_ANY,
                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Localize a point with respect to the solid
    virtual shape_domain_flags_type where_is(const vector_3d & position_, double a_skin) const;

    /// Return a collection of face info objects
    virtual unsigned int compute_faces(face_info_collection_type & faces_) const;

    /// Check if the embedded collection of faces has been computed
    bool has_computed_faces() const;

    /// Return the collection of faces
    const face_info_collection_type & get_computed_faces() const;

    /// Reset the computed faces
    void reset_computed_faces();

    /// Check the bounding data
    bool has_bounding_data() const;

    /// Return the bounding data
    const bounding_data & get_bounding_data() const;

    /// Reset the bounding data
    void reset_bounding_data();

    /// Set the bounding data
    void set_bounding_data(const bounding_data &);

    /// Build default bounding data
    void build_default_bounding_data();

    /// Smart print
    void tree_dump(std::ostream & a_out = std::clog,
                   const std::string & a_title = "",
                   const std::string & a_indent = "",
                   bool a_inherit = false) const override;

    /// Check the lock flag
    bool is_locked() const;

    /// Lock the shape, performing some internal calculations
    void lock();

    /// Unlock the shape
    void unlock();

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default values for attributes
    void _set_defaults();

    /// Executed at lock stage
    virtual void _at_lock();

    /// Executed at unlock stage
    virtual void _at_unlock();

    /// Initialize from properties
    void _initialize(const datatools::properties &, const handle_dict_type * = nullptr);

    /// Reset
    void _reset();

    /// Build the bounding data
    virtual void _build_bounding_data();

    /// Parse bounding data from a properties container
    /**
     * Example of an enforced bounding box:
     *  @code
     *  enforce_bounding_data : boolean = true
     *  bounding_data.type : string = "box"
     *  bounding_data.xmin : real as length = 3.5 cm
     *  bounding_data.xmax : real as length = 2.0 cm
     *  bounding_data.ymin : real as length = 31.0 cm
     *  bounding_data.ymax : real as length = 16.4 cm
     *  bounding_data.zmin : real as length = 7.0 cm
     *  bounding_data.zmax : real as length = 0.4 cm
     *  @endcode
     *
     * Example of an enforced bounding cylinder:
     *  @code
     *  enforce_bounding_data : boolean = true
     *  bounding_data.type : string = "cylinder"
     *  bounding_data.rmax : real as length = 3.4 cm
     *  bounding_data.zmin : real as length = 7.0 cm
     *  bounding_data.zmax : real as length = 0.4 cm
     *  @endcode
     *
     */
    void _initialize_bounding_data(const datatools::properties & config_);

    /// Parse stackable data from a properties container
    /**
     * Example of an enforced stackable data for all axis:
     *  @code
     *  enforce_stackable_data : boolean = true
     *  stackable.xmin : real as length = 3.5 cm
     *  stackable.xmax : real as length = 2.0 cm
     *  stackable.ymin : real as length = 31.0 cm
     *  stackable.ymax : real as length = 16.4 cm
     *  stackable.zmin : real as length = 7.0 cm
     *  stackable.zmax : real as length = 0.4 cm
     *  @endcode
     *
     */
    void _initialize_stackable_data(const datatools::properties & config_);

    /// Reset the bounding data
    void _reset_bounding_data();

    /// Return the bounding data
    bounding_data & _grab_bounding_data();

  private:

    // Work data:
    bool                   _locked_ = false;              //!< Lock flag
    bool                   _owns_stackable_data_ = false; //!< Ownership flag for stackable data
    const stackable_data * _stackable_data_ = nullptr;    //!< Handle to stackable data
    bounding_data          _bounding_data_;       //!< Bounding data
    boost::optional<double> _forced_volume_;      //!< Forced volume
    datatools::handle<face_info_collection_type> _computed_faces_; //!< Computed faces

    // Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION()

  };

  // class wrapper_shaped_3d : public i_shape_3d
  // {
  // public:

  //   wrapper_shaped_3d(i_shape_3d & shape_)
  //     : _shape_ref_(shape_) {}

  //   operator i_shape_3d() const { return _shape_ref_; }
    
  // private:

  //   i_shape_3d & _shaped_ref_;
    
  // };

} // end of namespace geomtools

/*
// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::i_shape_3d, 0)
*/

#endif // GEOMTOOLS_I_SHAPE_3D_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
