/// \file emfield/uniform_electric_field.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-25
 * Last modified: 2013-02-24
 *
 * License:
 *
 * Description:
 *
 *   Uniform electric field
 *
 * History:
 *
 */

#ifndef EMFIELD_UNIFORM_ELECTRIC_FIELD_H
#define EMFIELD_UNIFORM_ELECTRIC_FIELD_H 1

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /** Class representing an uniform (in space) and constant (in time)
   *  electric field
   */
  class uniform_electric_field : public base_electromagnetic_field
  {
  public:

    /// Default constructor
    uniform_electric_field(uint32_t = 0);

    /// Destructor
    ~uniform_electric_field() override;

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

    const geomtools::vector_3d & get_uniform_electric_field () const;

    void set_uniform_electric_field (const geomtools::vector_3d & b_);

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const override;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    geomtools::vector_3d _uniform_electric_field_; //!< Uniform electric field

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(uniform_electric_field)

  };

} // end of namespace emfield

#endif // EMFIELD_UNIFORM_ELECTRIC_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
