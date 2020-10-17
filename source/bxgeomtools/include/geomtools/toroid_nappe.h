/// \file geomtools/toroid_nappe.h
/* Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-02-18
 * Last modified: 2016-02-18
 *
 * License:
 *
 * Description:
 *
 *   Toroid nappe
 *
 */

#ifndef GEOMTOOLS_TOROID_NAPPE_H
#define GEOMTOOLS_TOROID_NAPPE_H 1

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/angular_range.h>

namespace geomtools {

  /// \brief A toroid nappe (2D shape)
  class toroid_nappe : public i_shape_2d
  {
  public:

    /// Return the identifier label for the class
    static const std::string & toroid_nappe_label();

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check if the rectangle is valid
    bool is_valid() const override;

    /// Set the sweep radius
    void set_sweep_radius(double new_value_);

    /// Return the sweep radius
    double get_sweep_radius() const;

    /// Set the radius
    void set_radius(double new_value_);

    /// Return the radius
    double get_radius() const;

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
    toroid_nappe();

    /// Constructor
    toroid_nappe(double sweep_radius_, double radius_);

    /// Constructor
    toroid_nappe(double sweep_radius_,
                 double radius_,
                 double start_phi_,
                 double delta_phi_);

    /// Destructor
    ~toroid_nappe() override;

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
    enum toroid_nappe_wires_rendering_option_type {
      WR_TOROID_NAPPE_NO_START_PHI_EDGE = (WR_BASE_LAST << 1),                 //!< Do not render the start theta edge
      WR_TOROID_NAPPE_NO_STOP_PHI_EDGE  = (WR_BASE_LAST << 2),                 //!< Do not render the stop theta edge
      WR_TOROID_NAPPE_LAST              = (WR_TOROID_NAPPE_NO_STOP_PHI_EDGE),  //!< Last defined bit
      WR_TOROID_NAPPE_MASK              = (WR_TOROID_NAPPE_NO_START_PHI_EDGE
                                           | WR_TOROID_NAPPE_NO_STOP_PHI_EDGE) //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

  protected:

    /// Set default attributes
    void _set_defaults();

    /// Action at initialization
    void _at_init();

  private:

    double _sweep_radius_; //!< The sweep radius
    double _radius_;       //!< The radius
    angular_range _phi_domain_; //!< Angular spread of the toroid nappe

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(toroid_nappe)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_TOROID_NAPPE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
