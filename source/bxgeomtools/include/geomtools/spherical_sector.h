/// \file geomtools/spherical_sector.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-02-23
 * Last modified: 2015-02-23
 *
 * License:
 *
 * Description:
 *
 *   Spherical sector
 *
 */

#ifndef GEOMTOOLS_SPHERICAL_SECTOR_H
#define GEOMTOOLS_SPHERICAL_SECTOR_H 1

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/angular_range.h>

namespace geomtools {

  /// \brief A sperical sector (2D shape)
  class spherical_sector : public i_shape_2d
  {
  public:

    /// Return the identifier label for the class
    static const std::string & spherical_sector_label();

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check if the rectangle is valid
    bool is_valid() const override;

    /// Set the radius
    void set_radius(double new_value_);

    /// Return the radius
    double get_radius() const;

    /// Return the theta domain (polar angle)
    const angular_range & get_theta_domain() const;

    /// Check if the sector has partial theta angle
    bool has_partial_theta() const;

    /// Check the start theta angle
    bool has_start_theta() const;

    /// Set the start theta angle
    void set_start_theta(double);

    /// Return the start theta angle
    double get_start_theta() const;

    /// Check the delta theta angle
    bool has_delta_theta() const;

    /// Set the delta theta angle
    void set_delta_theta(double);

    /// Return the delta angle
    double get_delta_theta() const;

    /// Return the phi domain (azimuthal angle)
    const angular_range & get_phi_domain() const;

    /// Check if the sector has partial phi angle
    bool has_partial_phi() const;

    /// Check the start phi angle
    bool has_start_phi() const;

    /// Set the start phi angle
    void set_start_phi(double);

    /// Return the start phi angle
    double get_start_phi() const;

    /// Check the delta phi angle
    bool has_delta_phi() const;

    /// Set the delta phi angle
    void set_delta_phi(double);

    /// Return the delta angle
    double get_delta_phi() const;

    /// Return the surface
    double get_surface(uint32_t flags_ = ALL_PIECES) const override;

    /// Default constructor
    spherical_sector();

    /// Constructor
    spherical_sector(double radius_);

    /// Constructor
    spherical_sector(double radius_,
                     double start_theta_,
                     double delta_theta_,
                     double start_phi_,
                     double delta_phi_);

    /// Destructor
    ~spherical_sector() override;

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset
    void reset() override;

    /// Check if a point is on the surface
    bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the normal at a given position of the surface
    vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = true,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Find the intercept point of a segment with the surface
    bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const override;

    /// \brief 3D rendering options
    enum sphesec_wires_rendering_option_type {
      WR_SPHSEC_NO_START_THETA_EDGE = (WR_BASE_LAST << 1),            //!< Do not render the start theta edge
      WR_SPHSEC_NO_STOP_THETA_EDGE  = (WR_BASE_LAST << 2),            //!< Do not render the stop theta edge
      WR_SPHSEC_NO_START_PHI_EDGE   = (WR_BASE_LAST << 3),            //!< Do not render the start phi edge
      WR_SPHSEC_NO_STOP_PHI_EDGE    = (WR_BASE_LAST << 4),            //!< Do not render the stop phi edge
      WR_SPHSEC_Z_SAMPLING          = (WR_BASE_LAST << 5),            //!< Use Z sampling rather than theta sampling for parallel arcs
      WR_SPHSEC_LAST                = (WR_SPHSEC_Z_SAMPLING),         //!< Last defined bit
      WR_SPHSEC_MASK                = (WR_SPHSEC_NO_START_THETA_EDGE
                                       | WR_SPHSEC_NO_STOP_THETA_EDGE
                                       | WR_SPHSEC_NO_START_PHI_EDGE
                                       | WR_SPHSEC_NO_STOP_PHI_EDGE
                                       | WR_SPHSEC_Z_SAMPLING)        //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

  protected:

    /// Set default attributes
    void _set_defaults();

  private:

    double        _radius_;       //!< The radius
    angular_range _theta_domain_; //!< Angular spread of the polar arc
    angular_range _phi_domain_;   //!< Angular spread of the azimuthal arc

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(spherical_sector)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SPHERICAL_SECTOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
