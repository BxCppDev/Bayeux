/// \file emfield/linear_combination_field.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-26
 * Last modified: 2015-04-19
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

#ifndef EMFIELD_LINEAR_COMBINATION_FIELD_H
#define EMFIELD_LINEAR_COMBINATION_FIELD_H 1

// Standard library:
#include <vector>
#include <map>

// This project:
#include <emfield/base_electromagnetic_field.h>

namespace emfield {

  /// \brief Class representing linear combination of many EM fields
  class linear_combination_field : public base_electromagnetic_field
  {
  public:

    /// \brief Internal class for linear_combination_field
    struct combined_field_entry
    {
      std::string                             label;        //!< Label of the field
      base_electromagnetic_field::handle_type field_handle; //!< Handle to the field
      double                                  weight;       //!< Weight associated to the field
      combined_field_entry ();
      ~combined_field_entry ();
    };

    typedef std::map<std::string,combined_field_entry> combined_field_dict_type;

    /// Default constructor
    linear_combination_field(uint32_t = 0);

    /// Destructor
    ~linear_combination_field() override;

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

    /// Add a combined field
    void add_combined_field (const std::string & label_,
                             base_electromagnetic_field::handle_type & field_handle_,
                             double weight_ = 1.0,
                             bool force_combined_ = false);

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

  private:

    combined_field_dict_type _combined_fields_; //!< Dictionnary of combined fields

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(linear_combination_field)

  };

} // end of namespace emfield

#endif // EMFIELD_LINEAR_COMBINATION_FIELD_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
