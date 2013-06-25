/* simulated_data_input_module.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-04
 * Last modified : 2013-06-22
 *
 * Description:
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef MCTOOLS_SIMULATED_DATA_INPUT_MODULE_H_
#define MCTOOLS_SIMULATED_DATA_INPUT_MODULE_H_ 1

#include <datatools/smart_filename.h>

#include <dpp/base_module.h>
#include <dpp/module_macros.h>

#include <mctools/simulated_data.h>

namespace mctools {

  class simulated_data_reader;

  DPP_MODULE_CLASS_DECLARE(simulated_data_input_module)
  {
  public:

    static const std::string DEFAULT_SD_BANK_LABEL;

    // Constructor :
    simulated_data_input_module(datatools::logger::priority logging_priority = datatools::logger::PRIO_FATAL);

    // Destructor :
    virtual ~simulated_data_input_module ();

    // This macro setup the module standard interface (initialize/reset/process) :
    DPP_MODULE_INTERFACE ();

    bool is_terminated () const;

  protected:

    int _load (datatools::things & a_event_record);

  private:

    bool _overwrite_existing_sd_bank_label_; //!< Flag to overwrite an existing simulated data bank
    std::string _sd_bank_label_; //!< The label of the simulated data bank
    boost::scoped_ptr<mctools::simulated_data_reader> _reader_; //!< Reader

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(simulated_data_input_module);

  };

}  // end of namespace mctools

// Object configuration description (OCD) support :
#include <datatools/ocd_macros.h>
DOCD_CLASS_DECLARATION(mctools::simulated_data_input_module)

#endif // MCTOOLS_SIMULATED_DATA_INPUT_MODULE_H_

// end of simulated_data_input_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
