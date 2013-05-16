/* io_module.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-09-10
 * Last modified : 2013-05-15
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 * Description:
 *
 *   A data processing module.
 *
 * History:
 *
 */

#ifndef DPP_IO_MODULE_H_
#define DPP_IO_MODULE_H_ 1

#include <dpp/base_module.h>    // data processing module abstract base class

#include <datatools/smart_filename.h>

namespace dpp {

  class i_data_sink;
  class i_data_source;
  class simple_data_sink;
  class simple_data_source;
  class simple_brio_data_sink;
  class simple_brio_data_source;
  class context_service;

  /// \brief A data processing module for automated I/O operations
  DPP_MODULE_CLASS_DECLARE(io_module)
  {
  public:

    DPP_MODULE_INTERFACE_CTOR_DTOR(io_module);

    enum rw_mode_type
    {
      RW_MODE_INVALID = -1,
      RW_MODE_INPUT   =  0,
      RW_MODE_OUTPUT  =  1
    };

    bool is_terminated () const;

    bool is_input () const;

    bool is_output () const;

    void set_context_service (dpp::context_service & a_context_service,
                              const std::string & a_ctx_label = "Ctx");
    void set_rw_mode (rw_mode_type a_rw_mode);
    void set_preserve_existing_output (bool a_preserve_existing_output);
    void set_max_files (int a_max_files);
    void set_max_record_total (int a_max_record_total);
    void set_max_record_per_file (int a_max_record_per_file);
    void set_filenames (const datatools::properties & a_setup);

    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

    void print (std::ostream & a_out = std::clog) const;

  protected:

    int _load (datatools::things & a_event_record);

    int _store (const datatools::things & a_event_record);

    void _set_defaults ();

  private:

    // Configuration attributes:
    int _rw_mode_;                   //!< Read/write mode (input/output)
    bool _preserve_existing_output_; //!< Flag to preserve existing output files
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
    i_data_source           * _source_;      //!< abstract data reader
    simple_data_source      * _bio_source_;  //!< Boost data reader
    simple_brio_data_source * _brio_source_; //!< Boost/ROOT (brio) data reader

    // Writers:
    i_data_sink             * _sink_;        //!< abstract data writer
    simple_data_sink        * _bio_sink_;    //!< Boost data writer
    simple_brio_data_sink   * _brio_sink_;   //!< Boost/ROOT (brio) data writer

    // Services:
    std::string                             _Ctx_label_;      //!< The label of the context service
    dpp::context_service * _Ctx_service_;    //!< The context service

    /*
    // Future ?
    string                                  _IO_label_;      //!< The label of the I/O service
    dpp::io_service           * _IO_service_;    //!< The I/O service

    */

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(io_module);

  };

} // namespace dpp

#endif // DPP_IO_MODULE_H_

// end of io_module.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
