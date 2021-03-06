// -*- mode: c++; -*-
/// \file raw_generator_module.h

#ifndef RAW_GENERATOR_MODULE_H
#define RAW_GENERATOR_MODULE_H

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/dpp:
#include <dpp/base_module.h> // The parent module class

namespace dpp_ex01 {

  /// \brief A module to randomly generate some raw_data within the data record
  class raw_generator_module : public dpp::base_module
  {
  public:

    /// Default string for the raw data bank label
    static const std::string DEFAULT_RD_BANK_LABEL;

    /// Constructor
    raw_generator_module(datatools::logger::priority logging_priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    virtual ~raw_generator_module();

    /// Initialization method
    virtual void initialize(const datatools::properties & a_config,
                             datatools::service_manager & a_service_manager,
                             dpp::module_handle_dict_type & a_module_dictionnary);

    /// Termination method
    virtual void reset();

    /// Main data record processing method
    virtual dpp::base_module::process_status process(datatools::things & a_data_record);

    void set_raw_data_bank_label(const std::string &);

    const std::string & get_raw_data_bank_label() const;

    void set_mean_number_of_hits(double);

    double get_mean_number_of_hits() const;

    void set_mean_energy(double);

    double get_mean_energy() const;

    void set_sigma_energy(double);

    double get_sigma_energy() const;

  protected:

    void _set_defaults();

  private:

    /// The name of the data bank where
    /// the generated raw_data object
    /// must be stored in the data record
    std::string _raw_data_bank_label_;
    double _mean_number_of_hits_; ///< The mean number of hits
    double _mean_energy_; ///< The mean energy per hit
    double _sigma_energy_; ///< The energy standart deviation per hit
    mygsl::rng _prng_; ///< Embedded PRNG

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(raw_generator_module);

  };

} // namespace dpp_ex01

// Object configuration description (OCD) support :
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(dpp_ex01::raw_generator_module)

#endif // RAW_GENERATOR_MODULE_H
