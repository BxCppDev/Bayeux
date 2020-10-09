/// \file mctools/simulated_data_input_module.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-04
 * Last modified : 2013-06-22
 *
 * Description:
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef MCTOOLS_SIMULATED_DATA_INPUT_MODULE_H
#define MCTOOLS_SIMULATED_DATA_INPUT_MODULE_H 1

// Third party:
// - Bayeux/datatools:
#include <datatools/smart_filename.h>
// - Bayeux/dpp:
#include <dpp/base_module.h>

// This project:
#include <mctools/simulated_data.h>

namespace mctools {

  class simulated_data_reader;

  /// \brief Simulation data input module
  class simulated_data_input_module
    : public dpp::base_module
  {
  public:

    static const std::string DEFAULT_SD_BANK_LABEL;
    static const std::string & default_sd_bank_label();

    // Constructor :
    simulated_data_input_module(datatools::logger::priority logging_priority_ = datatools::logger::PRIO_FATAL);

    // Destructor :
    ~simulated_data_input_module() override;

    /// Initialization
    void initialize(const datatools::properties & /* config_ */,
                            datatools::service_manager & /* service_mgr_ */,
                            dpp::module_handle_dict_type & /* modules_map_ */) override;

    /// Reset
    void reset() override;

    /// Data record processing
    dpp::base_module::process_status process(datatools::things & /* data_ */) override;

    bool is_terminated() const;

    void set_single_input_file(const std::string & filepath_);

    void set_list_of_input_files(const std::vector<std::string> & filepaths_,
                                 bool allow_duplicate_ = false);

    void set_incremental_input_files(const std::string & path_,
                                     const std::string & prefix_,
                                     const std::string & extension_,
                                     unsigned int stop_,
                                     unsigned int start_ = 0,
                                     int increment_ = 1);

    void set_sd_bank_label(const std::string & bank_label_);

    void set_overwrite_existing_sd_bank_label(bool);

    void set_limits(int max_record_total_, int max_record_per_file_ = 0, int max_files_ = -1);

  protected:

    dpp::base_module::process_status _load(datatools::things & event_record_);

  private:

    bool _overwrite_existing_sd_bank_label_; //!< Flag to overwrite an existing simulated data bank
    std::string _sd_bank_label_; //!< The label of the simulated data bank
    datatools::properties _reader_setup_; //!< Reader setup
    boost::scoped_ptr<mctools::simulated_data_reader> _reader_; //!< Reader

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(simulated_data_input_module)

  };

}  // end of namespace mctools

// Object configuration description (OCD) support :
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::simulated_data_input_module)

#endif // MCTOOLS_SIMULATED_DATA_INPUT_MODULE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
