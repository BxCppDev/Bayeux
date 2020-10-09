/// \file dpp/output_module.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-08-16
 * Last modified : 2014-01-15
 *
 * Copyright (C) 2013-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   An output data processing module.
 *
 */

#ifndef DPP_OUTPUT_MODULE_H
#define DPP_OUTPUT_MODULE_H 1

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/smart_filename.h>

// This project:
#include <dpp/base_module.h>

namespace dpp {

  // Forward declarations
  class i_data_sink;
  class io_common;

  /// \brief A output data processing module for automated I/O operations
  class output_module
    : public base_module
  {
  public:

    /// Constructor
    output_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    ~output_module() override;

    /// Initialization
    void initialize(const ::datatools::properties & /* config_ */,
                            datatools::service_manager & /* service_mgr_ */,
                            dpp::module_handle_dict_type & /* modules_map_ */) override;

    /// Reset
    void reset() override;

    /// Data record processing
    process_status process(::datatools::things & /* data_ */) override;

    /// Set limits
    void set_limits(int max_record_total_,
                    int max_record_per_file_ = 0,
                    int max_files_ = -1);

    /// Set the filename of a single output file
    void set_single_output_file(const std::string & filepath_);

    /// Set the filenames of a list of output files
    void set_list_of_output_files(const std::vector<std::string> & filepaths_,
                                  bool allow_duplicate_ = false);

    /// Set the incremented filenames of output files
    void set_incremental_output_files(const std::string & path_,
                                      const std::string & prefix_,
                                      const std::string & extension_,
                                      unsigned int stop_,
                                      unsigned int start_ = 0,
                                      int increment_ = 1);

    /// Set the flag for preserving existing output file (prevent from file overwriting)
    void set_preserve_existing_output(bool preserve_existing_output);

    /// Check if an embedded metadata store exists
    bool has_metadata_store() const;

    /// Return a const reference to the existing embedded metadata store
    const datatools::multi_properties & get_metadata_store() const;

    /// Return a mutable reference to the embedded metadata store, create it if it does not exist
    datatools::multi_properties & grab_metadata_store();

    /// Clear the embedded metadata store
    void clear_metadata_store();

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const override;

    /// Check output termination
    bool is_terminated() const;

    /// Return a reference to the non mutable internal I/O data structure
    const io_common & get_common() const;

    /// Set the label/name of the context service to be used
    void set_context_label(const std::string &);

    /// Export a metadata section with a given name from the context service to the output stream
    void export_context_metadata(const std::string &);

    /// Export all metadata sections from the context service to the output stream
    void set_export_context_all(bool);

  protected:

    /// Store a data record
    process_status _store(const datatools::things & data_record_);

    /// Set default values before explicit settings and initialization
    void _set_defaults();

    /// Return a reference to the mutable internal I/O data structure
    io_common & _grab_common();

  private:

    /// Open output file
    base_module::process_status _open_sink_();

    /// Process metadata, if any
    void _store_metadata_();

  private:

    bool                         _preserve_existing_output_; //!< Flag to preserve existing output files
    boost::scoped_ptr<io_common> _common_; //!< Common data structure
    i_data_sink                * _sink_ = nullptr; //!< Abstract data writer

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(output_module)

  };

} // namespace dpp

#endif // DPP_OUTPUT_MODULE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
