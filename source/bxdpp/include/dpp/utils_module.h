/// \file dpp/utils_module.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-01-09
 * Last modified: 2013-05-15
 *
 * License:
 *
 * Description:
 *
 *   Utility processing module
 *
 */

#ifndef DPP_UTILS_MODULE_H
#define DPP_UTILS_MODULE_H 1

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <dpp/base_module.h>

namespace dpp {

  /// \brief A data processing module for automated utility operations (bank removal)
  class utils_module
    : public base_module
  {
  public:

    enum mode_type {
      MODE_INVALID         = 0, //!< Undefined mode
      MODE_CLEAR           = 0x1, //!< Clear all banks mode
      MODE_REMOVE_ONE_TYPED_BANK = 0x2, //!< Remove one single bank, optionaly with a given type (serial tag)
      MODE_REMOVE_BANKS    = 0x4, //!< Remove several banks by label (name)
      MODE_ADD_PROPERTY    = 0x8 //!< Add a property in a bank
    };

    /// Constructor
    utils_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    ~utils_module() override;

    /// Initialization
    void initialize(const ::datatools::properties & /* config_ */,
                            datatools::service_manager & /* service_mgr_ */,
                            dpp::module_handle_dict_type & /* modules_map_ */) override;

    /// Reset
    void reset() override;

    /// Data record processing
    process_status process(::datatools::things & /* data_ */) override;

  protected:

    /// Set default values for attributes
    void _set_defaults();

    /// Special method to clear all banks
    void _process_clear(datatools::things & a_event_record_);

    /// Special method to remove one single bank by label and type (optional)
    void _process_remove_one_typed_bank(datatools::things & a_event_record_);

    /// Special method to remove several banks by label (name)
    void _process_remove_banks(datatools::things & a_event_record_);

    /// Special method to add some property in a 'datatools::properties' bank
    void _process_add_property (datatools::things & a_event_record_);

  private:

    mode_type                _mode_; //!< Processing mode
    std::string              _remove_one_typed_bank_label_; //!< Labels of (optionnaly typed) bank to be removed
    std::string              _remove_one_typed_bank_type_;  //!< Type (serial tag) of the bank to be removed
    std::vector<std::string> _remove_bank_labels_; //!< Labels of the banks to be removed
    std::string              _add_property_bank_label_; //!< Label of a 'datatools::property' bank to be enriched with a property
    std::string              _add_property_key_; //!< Key of the property to be added in a 'datatools::property' bank
    datatools::properties    _add_property_value_; //!< Value of the property to be added in a 'datatools::property' bank
    std::string              _add_property_description_; //!< Description of the property to be added in a 'datatools::property' bank
    bool                     _add_property_update_; //!< Flag to allow update of an existing property in a 'datatools::property' bank

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(utils_module)

  };

} // end of namespace dpp

#endif // DPP_UTILS_MODULE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
