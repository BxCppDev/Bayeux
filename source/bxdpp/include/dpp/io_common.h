/* io_common.h
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-08-16
 * Last modified : 2013-08-16
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Base class for I/O modules.
 *
 * History:
 *
 */

#ifndef DPP_IO_COMMON_H_
#define DPP_IO_COMMON_H_ 1

#include <datatools/smart_filename.h>
#include <datatools/logger.h>

namespace dpp {

  class context_service;
  class i_data_sink;
  class i_data_source;

  /// \brief Some common data structure for I/O modules
  class io_common
  {
  public:

    enum format_type {
      FORMAT_INVALID = -1,
      FORMAT_BIO     = 0,
      FORMAT_BRIO    = 1
    };

    static format_type guess_format_from_filename(const std::string &);

    static i_data_source * allocate_reader(const std::string &,
                                           datatools::logger::priority);

    static i_data_sink * allocate_writer(const std::string &,
                                         datatools::logger::priority);

    io_common(datatools::logger::priority & logging_,
              const std::string & module_name_);

    virtual ~io_common();

    void set_module_name (const std::string &);

    const std::string & get_module_name() const;

    void set_context_label (const std::string &);

    const std::string & get_context_label() const;

    void set_context_service (dpp::context_service & a_context_service,
                              const std::string & a_ctx_label = "Ctx");

    bool has_context_service() const;

    const dpp::context_service & get_context_service() const;

    dpp::context_service & grab_context_service();

    void set_max_files (int a_max_files);

    int get_max_files() const;

    void set_max_record_total (int a_max_record_total);

    int get_max_record_total() const;

    void set_max_record_per_file (int a_max_record_per_file);

    int get_max_record_per_file() const;

    void init_filenames (const datatools::properties & a_setup);

    const datatools::smart_filename & get_filenames() const;

    datatools::smart_filename & grab_filenames();

    bool is_terminated () const;

    void set_terminated (bool);

    int get_file_record_counter() const;

    int get_record_counter() const;

    int get_file_index() const;

    void set_file_record_counter(int);

    void set_record_counter(int);

    void set_file_index(int);

    void initialize(const datatools::properties & a_config,
                    datatools::service_manager & a_service_manager);

    void reset();

    virtual void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const;

  protected:

    void _set_defaults ();

  private:

    // Configuration attributes:
    std::string _module_name_;               //!< Name of the associated module
    datatools::logger::priority * _logging_; //!< Handle to the logging priority of the associated module
    int _max_record_per_file_;       //!< Maximum number of event records per file
    int _max_record_total_;          //!< Maximum number of event records to be processed
    int _max_files_;                 //!< Maximum number of data files to be processed
    datatools::smart_filename _filenames_; //!< Smart list of data filenames

    /* Running/dynamic attributes: */
    bool _terminated_;            //!< Termination flag
    int _file_record_counter_;    //!< Event record counter in the current file
    int _record_counter_;         //!< Total event record counter
    int _file_index_;             //!< Index of the current datafile index

    // Services:
    std::string            _Ctx_label_;      //!< The label of the context service
    dpp::context_service * _Ctx_service_;    //!< The context service

  };

} // namespace dpp

#endif // DPP_IO_COMMON_H_

// end of io_common.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
