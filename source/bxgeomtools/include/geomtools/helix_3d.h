/// \file geomtools/helix_3d.h
/* Author(s): Francois Mauger <mauger@lpccaen.in2p3.fr>
 *            Arnaud Chapon   <chapon@lpccaen.in2p3.fr>
 * Creation date: 2008-12-18
 * Last modified: 2014-06-25
 *
 * License:
 *
 * Description:
 *
 *   Helix 3D model (along Z axis)
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_HELIX_3D_H
#define GEOMTOOLS_HELIX_3D_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>
#include <datatools/bit_mask.h>

// This project:
#include <geomtools/i_shape_1d.h>
#include <geomtools/utils.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  /// \brief An helix (1D shape) in a 3D space
  class helix_3d : public i_shape_1d,
                   public i_wires_3d_rendering
  {
  public:

    /// Return the identifier label for the class
    static const std::string & helix_3d_label();

    /// Check if the shape is normal
    bool is_normal();

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check if the helix is valid
    bool is_valid() const override;

    /// Invalidate the helix
    void invalidate();

    /// Return a reference to the non mutable center
    const vector_3d & get_center() const;

    /// Return a reference to the mutable center
    vector_3d & grab_center();

    /// Set the center
    void set_center(const vector_3d &);

    /// Set the center
    void set_center(double x_, double y_, double z_);

    /// Return the radius
    double get_radius() const;

    /// Set the radius
    void set_radius(double);

    /// Return the step
    double get_step() const;

    /// Set the step
    void set_step(double);

    /// Return the normalized start angle
    double get_t1() const;

    /// Set the normalized start angle
    void set_t1(double);

    /// Return the normalized stop angle
    double get_t2() const;

    /// Set the normalized stop angle
    void set_t2(double);

    /// Set the start angle
    void set_angle1(double);

    /// Set the stop angle
    void set_angle2(double);

    /// Return the start angle
    double get_angle1() const;

    /// Return the stop angle
    double get_angle2() const;

    /// Return the length
    double get_length(uint32_t flags_ = PATH_ALL_BITS) const override;

    /// Return the curvilinear position from the normalized angle
    double get_curvilinear_position(double t_) const;

    /// Check if a given normalized angle is valid
    bool   t_is_valid(double t_) const;

    /// Return the angle associated to given (x,y) position in the reference frame of the helix
    double get_theta_from_xy(double x_, double y_) const;

    /// Return the normalized angle associated to a position in the reference frame of the helix
    double get_t(vector_3d point_) const;

    /// Return the normalized angle associated to a (x,y,z) position triplet in the reference frame of the helix
    double get_t_from_xyz(double x_, double y_, double z_) const;

    /// Return the normalized angle associated to a z position in the reference frame of the helix
    double get_t_from_z(double z_) const;

    /// Return the z position associated to a normalized angle
    double get_z_from_t(double t_) const;

    /// Return the position on the helix for a given normalized angle
    vector_3d get_point(double t_) const;

    /// Return the position on the helix for a given angle
    vector_3d get_point_xy(double theta_) const;

    /// Invalidate helix's angles
    void invalidate_angles();

    /// Check if angles are valid
    bool angles_are_valid() const;

    /// Return the start position on the helix
    vector_3d get_first() const;

    /// Return the stop position on the helix
    vector_3d get_last() const;

    /// Default constructor
    helix_3d();

    /// Destructor
    ~helix_3d() override;

    /// Reset
    void reset() override;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

    /// Basic print
    void dump() const;

    /// Compute a collection of vertexes representing the helix
    void make_vertex_collection(basic_polyline_3d &, double angular_step_ = 0.0) const;

    /// Return a collection of vertexes representing the helix
    basic_polyline_3d make_vertex_collection() const;

    /// Convert parametric to angle in radian:
    static double t_to_angle(double t_);

    /// Convert angle in radian to parametric:
    static double angle_to_t(double angle_);

    enum print_flags_type {
      PRINT_XYZ_EXPAND1 = datatools::bit_mask::bit00,
      PRINT_XYZ_EXPAND2 = datatools::bit_mask::bit01,
      PRINT_XYZ_CENTER = datatools::bit_mask::bit02
    };

    /// Print approximated data (x,y,z) triplets representing the helix
    static void print_xyz(std::ostream & out_,
                          const helix_3d & helix_,
                          double step_angle_ = 0.0,
                          unsigned int flags_ = 0);

    /// Check if a position in on the curve
    bool is_on_curve(const vector_3d & position_,
                             double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the direction ar some position in on the curve
    vector_3d get_direction_on_curve(const vector_3d & position_) const override;

    /// \brief 3D rendering options
    enum helix_wires_rendering_option_type {
      WR_HELIX_NO_ANGULAR_SAMPLE_BOOST = (WR_BASE_LAST << 1),                 //!< Do not boost angular sampling
      WR_HELIX_LAST                    = (WR_HELIX_NO_ANGULAR_SAMPLE_BOOST),  //!< Last defined bit
      WR_HELIX_MASK                    = (WR_HELIX_NO_ANGULAR_SAMPLE_BOOST)   //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;


  protected:

    /// Set the default attributes values
    void _set_defaults();

  private:

    double    _radius_; //!< The radius of the helix
    vector_3d _center_; //!< The center of the helix
    double    _step_;   //!< The pitch of the helix
    double    _t1_;     //!< The start angle divided by (2 pi)
    double    _t2_;     //!< The stop angle divided by (2 pi)

    // Serialization interface:
    DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::helix_3d, "geomtools::helix_3d")

// Explicit class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::helix_3d, 2)

#endif // GEOMTOOLS_HELIX_3D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
