/// \file dpp/io_common.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

#ifndef DPP_IO_COMMON_H
#define DPP_IO_COMMON_H 1

// Standard library:
#include <string>
#include <vector>

// Third party library:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/smart_filename.h>
#include <datatools/multi_properties.h>

namespace dpp {

  class context_service;
  class i_data_sink;
  class i_data_source;

  /// \brief Some common data structure for I/O modules
  class io_common
  {
  public:

    /// \brief Input/output
    enum io_type {
      IO_INVALID = -1,
      IO_OUTPUT  = 0,
      IO_INPUT   = 1
    };

    /// \brief Format of the storage
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

    static const std::string & metadata_key();

    static const std::string & metadata_meta();

    static const std::string & metadata_rank();

    static const std::string & metadata_topic();

    static const std::string & context_key();

    static const std::string & context_meta();

    static const std::string & context_rank();

    static const std::string & context_topic();

    io_common(datatools::logger::priority & logging_,
              const std::string & module_name_);

    virtual ~io_common();

    void set_module_name (const std::string &);

    const std::string & get_module_name() const;

    format_type get_format() const;

    void set_format(format_type);

    io_type get_io() const;

    void set_io(io_type);

    void set_context_label (const std::string &);

    const std::string & get_context_label() const;

    void set_context_service (dpp::context_service & context_service_,
                              const std::string & ctx_label_ = "Ctx");

    bool has_context_service() const;

    const dpp::context_service & get_context_service() const;

    dpp::context_service & grab_context_service();

    void add_context_metadata(const std::string &);

    void set_context_all(bool val_);

    void clear_context_metadata();

    void set_max_files (int max_files_);

    int get_max_files() const;

    void set_max_record_total (int max_record_total_);

    int get_max_record_total() const;

    void set_max_record_per_file (int max_record_per_file_);

    int get_max_record_per_file() const;

    void init_filenames (const datatools::properties & setup_);

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

    void initialize(const datatools::properties & config_,
                    datatools::service_manager & service_manager_);

    void reset();

    /// Return an embedded metadata store (const reference)
    const datatools::multi_properties & get_metadata_store() const;

    /// Return an embedded metadata store (mutable reference)
    datatools::multi_properties & grab_metadata_store();

    /// Clear the contents of the embedded metadata store
    void clear_metadata_store();

    /// Return a const reference to the list of context metadata labels
    const std::vector<std::string> & get_context_metadata() const;

    /// Check if all context metadata are accepted
    bool is_context_all() const;

    virtual void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const;

  protected:

    void _set_defaults ();

  private:

    // Configuration attributes:
    std::string _module_name_;               //!< Name of the associated module
    io_type     _io_ = IO_INVALID;           //!< I/O flag
    format_type _format_ = FORMAT_INVALID;   //!< Format flag
    datatools::logger::priority * _logging_ = nullptr; //!< Handle to the logging priority of the associated module
    int _max_record_per_file_ = 0;               //!< Maximum number of event records per file
    int _max_record_total_ = 0;                  //!< Maximum number of event records to be processed
    int _max_files_ = -1;                         //!< Maximum number of data files to be processed
    datatools::smart_filename _filenames_;   //!< Smart list of data filenames

    // Running/dynamic attributes:
    bool _terminated_ = false;            //!< Termination flag
    int _file_record_counter_ = 0;    //!< Event record counter in the current file
    int _record_counter_ = 0;         //!< Total event record counter
    int _file_index_ = -1;             //!< Index of the current datafile index

    // Metadata support:
    datatools::multi_properties _metadata_store_; //!< Embedded metadata container

    // Context service (external metadata container):
    std::string              _Ctx_label_;        //!< The label/name of the context service
    dpp::context_service *   _Ctx_service_ = nullptr;      //!< The handle to the context service
    std::vector<std::string> _Ctx_metadata_;     //!< The list of labels of the metadata to be taken into account
    bool                     _Ctx_metadata_all_ = false; //!< Flag to accept all metadata

  };

} // namespace dpp

#endif // DPP_IO_COMMON_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
