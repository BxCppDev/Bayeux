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

    /// Contructor
    polynomial_magnetic_field(uint32_t flags_ = 0);

    /// Destructor
    virtual ~polynomial_magnetic_field();

    /// Initialization
    virtual void initialize(const ::datatools::properties & setup_,
                            ::datatools::service_manager & service_manager_,
                            ::emfield::base_electromagnetic_field::field_dict_type & dict_);

    /// Reset
    virtual void reset();

    virtual int compute_electric_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       ::geomtools::vector_3d & electric_field_) const;

    virtual int compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       ::geomtools::vector_3d & magnetic_field_) const;

  private:

    geomtools::vector_3d _uniform_magnetic_field_; //!< The uniform magnetic field

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(polynomial_magnetic_field);

  };

} // end of namespace emfield

#endif // EMFIELD_POLYNOMIAL_MAGNETIC_FIELD_H

// Local Variables: --
// mode: c++ --
// End: --
