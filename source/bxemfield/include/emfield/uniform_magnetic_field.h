/// \file emfield/uniform_magnetic_field.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-24
 * Last modified: 2013-02-24
 *
 * License:
 *
 * Description:
 *
 *   Uniform magnetic field
 *
 * History:
 *
 */

#ifndef EMFIELD_UNIFORM_MAGNETIC_FIELD_H
#define EMFIELD_UNIFORM_MAGNETIC_FIELD_H 1

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /** Class representing an uniform (in space) and constant (in time)
   *  magnetic field
   */
  class uniform_magnetic_field : public base_electromagnetic_field
  {
  public:

    /// Default constructor
    uniform_magnetic_field(uint32_t = 0);

    /// Destructor
    virtual ~uniform_magnetic_field();

    /// Initialization
    virtual void initialize(const ::datatools::properties &,
                            ::datatools::service_manager &,
                            ::emfield::base_electromagnetic_field::field_dict_type &);

    /// Reset
    virtual void reset();

    /// Compute electric field
    virtual int compute_electric_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       ::geomtools::vector_3d & electric_field_) const;

    /// Compute magnetic field
    virtual int compute_magnetic_field(const ::geomtools::vector_3d & position_,
                                       double time_,
                                       geomtools::vector_3d & magnetic_field_) const;


    const geomtools::vector_3d & get_uniform_magnetic_field() const;

    void set_uniform_magnetic_field(const geomtools::vector_3d & b_);

    /// Smart print
    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_               = false) const;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    geomtools::vector_3d _uniform_magnetic_field_; //!< The uniform magnetic field

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(uniform_magnetic_field)

  };

} // end of namespace emfield

#endif // EMFIELD_UNIFORM_MAGNETIC_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
