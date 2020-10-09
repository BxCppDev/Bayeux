/// \file emfield/polynomial_magnetic_field.h
/* Author(s):     Xavier Garrido <garrido@lal.in2p3.fr>
 * Creation date: 2015-02-14
 * Last modified: 2015-02-14
 *
 * License:
 *
 * Description:
 *
 *   Magnetic field with polynomial variations
 *    - Bx = pxx0 + pxx1*x + pxx2*x²
 *    - By = pyx0 + pyx1*x + pyy0 + pyy1*y
 *    - Bz = 0
 *
 * History:
 *
 */

#ifndef EMFIELD_POLYNOMIAL_MAGNETIC_FIELD_H
#define EMFIELD_POLYNOMIAL_MAGNETIC_FIELD_H 1

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /** Class representing a magnetic field with polynomial variations (in space)
   *  and constant (in time)
   */
  class polynomial_magnetic_field : public base_electromagnetic_field
  {
  public:

    /// Typedef for polynomial parameters
    typedef std::vector<double> polynomial_parameters_type;

    /// Typedef for space limits
    typedef std::pair<double,double> coordinate_limits_type;

    /// Internal structure to embed polynomial parameters
    struct magnetic_field_coordinate {
      magnetic_field_coordinate();

      polynomial_parameters_type px;
      polynomial_parameters_type py;
      polynomial_parameters_type pz;

      coordinate_limits_type xlimits;
      coordinate_limits_type ylimits;
      coordinate_limits_type zlimits;
    };

    /// Contructor
    polynomial_magnetic_field(uint32_t flags_ = 0);

    /// Destructor
    ~polynomial_magnetic_field() override;

    /// Initialization
    void initialize(const datatools::properties & setup_,
                            datatools::service_manager & service_manager_,
                            emfield::base_electromagnetic_field::field_dict_type & dict_) override;

    /// Reset
    void reset() override;

    /// Main function to compute electric field
    int compute_electric_field(const geomtools::vector_3d & position_,
                                       double time_,
                                       geomtools::vector_3d & electric_field_) const override;

    /// Main function to compute magnetic field
    int compute_magnetic_field(const geomtools::vector_3d & position_,
                                       double time_,
                                       geomtools::vector_3d & magnetic_field_) const override;

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

  private:

    double _magnetic_field_unit_;   //!< Magnetic field unit
    magnetic_field_coordinate _bx_; //!< Bx field coordinate
    magnetic_field_coordinate _by_; //!< By field coordinate
    magnetic_field_coordinate _bz_; //!< Bz field coordinate

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(polynomial_magnetic_field)

  };

} // end of namespace emfield

#endif // EMFIELD_POLYNOMIAL_MAGNETIC_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
