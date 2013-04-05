/* plain_simulated_data_input_module.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-04
 * Last modified : 2012-04-10
 *
 * Description:
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 */

#ifndef MCTOOLS_PLAIN_SIMULATED_DATA_INPUT_MODULE_H_
#define MCTOOLS_PLAIN_SIMULATED_DATA_INPUT_MODULE_H_ 1

#include <datatools/smart_filename.h>

#include <dpp/base_module.h>
#include <dpp/module_macros.h>

#include <mctools/simulated_data.h>

namespace datatools {
  class data_reader;
}

namespace brio {
  class reader;
}

namespace mctools {

    DPP_MODULE_CLASS_DECLARE(plain_simulated_data_input_module)
    {

    public:

      bool is_terminated () const;

      void set_max_files (int a_max_files);

      void set_max_record_total (int a_max_record_total);

      void set_max_record_per_file (int a_max_record_per_file);

      void set_filenames (const datatools::properties & a_setup);

      // Constructor :
      plain_simulated_data_input_module(bool debug_ = false);

      // Destructor :
      virtual ~plain_simulated_data_input_module ();

      // This macro setup the module standard interface (initialize/reset/process) :
      DPP_MODULE_INTERFACE ();

    protected:
      
      int _load (datatools::things & a_event_record);

      void _set_defaults ();

    private:

      int _max_record_per_file_;       //!< Maximum number of event records per file
      int _max_record_total_;          //!< Maximum number of event records to be processed
      int _max_files_;                 //!< Maximum number of data files to be processed
      
      datatools::smart_filename _filenames_; //!< Smart list of data filenames
      
      /* Running/dynamic attributes: */
      bool _terminated_;            //!< Termination flag
      int _file_record_counter_;    //!< Event record counter in the current file
      int _record_counter_;         //!< Total event record counter
      int _file_index_;             //!< Index of the current datafile index

      // Readers:
      boost::scoped_ptr<datatools::data_reader> _bio_source_;
      boost::scoped_ptr<brio::reader>           _brio_source_;
      
      // Macro to automate the registration of the module :
      DPP_MODULE_REGISTRATION_INTERFACE(plain_simulated_data_input_module);

    };

}  // end of namespace mctools

#endif // MCTOOLS_PLAIN_SIMULATED_DATA_INPUT_MODULE_H_

// end of plain_simulated_data_input_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
