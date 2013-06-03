// -*- mode: c++ ; -*-
/* linear_combination_field.h
 * Author (s):    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-04-26
 * Last modified: 2013-04-24
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

#include <emfield/base_electromagnetic_field.h>

#include <vector>
#include <map>

#ifndef EMFIELD_LINEAR_COMBINATION_FIELD_H_
#define EMFIELD_LINEAR_COMBINATION_FIELD_H_ 1

namespace emfield {

  /// \brief Class representing linear combination of many EM fields
  EMFIELD_CLASS_DECLARE(linear_combination_field)
  {
  public:

    /// \brief Internal class for linear_combination_field
    struct combined_field_entry
    {
      std::string                             label;
      base_electromagnetic_field::handle_type field_handle;
      double                                  weight;
      combined_field_entry ();
      ~combined_field_entry ();
    };

    typedef std::map<std::string,combined_field_entry> combined_field_dict_type;

    EMFIELD_INTERFACE_CTOR_DTOR(linear_combination_field);

    void add_combined_field (const std::string & label_,
                             base_electromagnetic_field::handle_type & field_handle_,
                             double weight_ = 1.0,
                             bool force_combined_ = false);

  private:

    combined_field_dict_type _combined_fields_; /// Dictionnary of combined fields

    // Macro to automate the registration of the EM field :
    EMFIELD_REGISTRATION_INTERFACE(linear_combination_field);

  };

} // end of namespace emfield

#endif // EMFIELD_LINEAR_COMBINATION_FIELD_H_

// end of linear_combination_field.h
