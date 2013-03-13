// -*- mode: c++ ; -*-
/* utils_module.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-01-09
 * Last modified: 2013-02-18
 *
 * License:
 *
 * Description:
 *
 *   Utility processing module
 *
 * History:
 *
 */

#ifndef DPP_UTILS_MODULE_H_
#define DPP_UTILS_MODULE_H_ 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

#include <datatools/properties.h>

#include <dpp/base_module.h>

namespace dpp {

  /// \brief A data processing module for automated utility operations (bank removal)
  class utils_module : public base_module
  //  DPP_MODULE_CLASS_DECLARE(utils_module)
  {
  public:

    /// Constructor
    utils_module (int a_debug_level = 0);

    /// Destructor
    virtual ~utils_module ();

    DPP_MODULE_INTERFACE ();

  protected:

    /// Set default values for attributes
    void _set_defaults ();

    /// Special method to clear all banks
    void _process_clear (datatools::things & a_event_record);

    /// Special method to remove one single bank by label and type (optional)
    void _process_remove_one_typed_bank (datatools::things & a_event_record);

    /// Special method to remove several banks by label (name)
    void _process_remove_banks (datatools::things & a_event_record);

  public:

    enum mode_type {
      MODE_INVALID         = 0, //!< Undefined mode
      MODE_CLEAR           = 0x1, //!< Clear all banks mode
      MODE_REMOVE_ONE_TYPED_BANK = 0x2, //!< Remove one single bank, optionaly with a given type (serial tag) 
      MODE_REMOVE_BANKS    = 0x4, //!< Remove several banks by label (name)
    };

  private:

    uint32_t                 _mode_; //!< Processing mode
    std::string              _remove_one_typed_bank_label_; //!< Labels of (optionnaly typed) bank to be removed
    std::string              _remove_one_typed_bank_type_;  //!< Type (serial tag) of the bank to be removed
    std::vector<std::string> _remove_bank_labels_; //!< Labels of the banks to be removed
 
    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(utils_module);

  };

} // end of namespace dpp

#endif // DPP_UTILS_MODULE_H_

// end of utils_module.h
