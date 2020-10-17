/// \file emfield/oscillating_field.h
/* Author (s):    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-05-22
 * Last modified: 2015-03-07
 *
 * License:
 *
 * Description:
 *
 *   Placement field
 *
 * History:
 *
 */

#ifndef EMFIELD_OSCILLATING_FIELD_H
#define EMFIELD_OSCILLATING_FIELD_H 1

// Third party:
// - Bayeux/geomtools:
#include <geomtools/placement.h>

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /** \brief Class representing a weighted electric/magnetic field with some
   *         oscillating coefficient.
   *
   *  Given a field F(x, y, z, t), one computes the resulting weighted
   *  field F'(x, y, z, t) :
   *
   *  F'(x, y, z, t) = F(x, y, z, t) * (ped + sca * osc(2*pi*fre + pha))
   *
   *  Default values are:
   *
   *   - ped : pedestal (dimensionless value, default: 0.0)
   *   - sca : scale (dimensionless value, default: 1.0)
   *   - fre : frequency (value with frequency unit, default: 0.0)
   *   - pha : phase (value with angle unit, default: 0.0)
   *
   */
  class oscillating_field : public base_electromagnetic_field
  {
  public:

    enum mode_sin_cos_type {
      SC_MODE_INVALID = 0, //!< Invalid sine/cosine mode
      SC_MODE_COSINE  = 1, //!< Cosine mode
      SC_MODE_SINE    = 2, //!< Sine mode
      SC_MODE_DEFAULT = SC_MODE_COSINE //!< Default mode
    };

    /// Default constructor
    oscillating_field(uint32_t = 0);

    /// Destructor
    ~oscillating_field() override;

    /// Initialization
    void initialize(const ::datatools::properties &,
                            ::datatools::service_manager &,
                            ::emfield::base_electromagnetic_field::field_dict_type &) override;

    /// Reset
    void reset() override;

    /// Compute electric field
    int compute_electric_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       ::geomtools::vector_3d & electric_field_) const override;

    /// Compute magnetic field
    int compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       geomtools::vector_3d & magnetic_field_) const override;

    void set_sine_cosine_mode(mode_sin_cos_type);

    bool is_cosine_mode() const;

    bool is_sine_mode() const;

    double get_frequency () const;

    double get_period () const;

    double get_phase () const;

    double get_scale () const;

    double get_pedestal () const;

    void set_frequency (double);

    void set_phase (double);

    void set_scale (double);

    void set_pedestal (double);

    void set_field (base_electromagnetic_field::handle_type &);

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

  protected:

    void _set_defaults();

    double _get_coefficient(double time_) const;

  private:

    base_electromagnetic_field::handle_type _field_; //!< Handle to the field to be oscillating
    mode_sin_cos_type _sin_cos_mode_; //!< Sine/cosine mode
    double _frequency_; //!< Oscillation frequency (default value 0 Hz)
    double _phase_;     //!< Phase (default value 0 degree)
    double _pedestal_;  //!< Pedestal (default value 0)
    double _scale_;     //!< Scale (default value 1)

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(oscillating_field)

  };

} // end of namespace emfield

#endif // EMFIELD_OSCILLATING_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
