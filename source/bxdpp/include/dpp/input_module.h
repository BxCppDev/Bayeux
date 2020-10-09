/// \file dpp/input_module.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-08-16
 * Last modified : 2013-12-13
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
 *   An input data processing module.
 *
 */

#ifndef DPP_INPUT_MODULE_H
#define DPP_INPUT_MODULE_H 1

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/smart_filename.h>

// This project:
#include <dpp/base_module.h>


namespace dpp {

  // Forward declarations:
  class i_data_source;
  class io_common;

  /// \brief An input data processing module for automated I/O operations
  class input_module
    : public base_module
  {
  public:

    /// Set limits
    void set_limits(int max_record_total_,
                    int max_record_per_file_ = 0,
                    int max_files_ = -1);

    /// Set the filename of a single input file
    void set_single_input_file(const std::string & filepath_);

    /// Set the filenames of a list of input files
    void set_list_of_input_files(const std::vector<std::string> & filepaths_,
                                 bool allow_duplicate_ = false);

    /// Set the incremented filenames of input files
    void set_incremental_input_files(const std::string & path_,
                                     const std::string & prefix_,
                                     const std::string & extension_,
                                     unsigned int stop_,
                                     unsigned int start_ = 0,
                                     int increment_ = 1);

    /// Set the label/name of the context service to be used
    void set_context_label(const std::string &);

    /// Import a metadata section with given label from the input stream to the context service
    void import_context_metadata(const std::string &);

    /// Import all metadata sections from the input stream to the context service
    void set_import_context_all(bool);

    /// Constructor
    input_module(datatools::logger::priority = datatools::logger::PRIO_FATAL);

    /// Destructor
    ~input_module() override;

    /// Initialization
    void initialize(const ::datatools::properties & /* config_ */,
                            datatools::service_manager & /* service_mgr_ */,
                            dpp::module_handle_dict_type & /* modules_map_ */) override;

    /// Reset
    void reset() override;

    /// Data record processing
    process_status process(::datatools::things & /* data_ */) override;

    /// Smart print
    void tree_dump(std::ostream & a_out         = std::clog,
                           const std::string & a_title  = "",
                           const std::string & a_indent = "",
                           bool a_inherit          = false) const override;

    /// Check input termination
    bool is_terminated() const;

    /// Return a reference to the non mutable internal I/O data structure
    const io_common & get_common() const;

    /// Check if an embedded metadata store exists
    bool has_metadata_store() const;

    /// Set clear record flag
    void set_clear_record(bool);

    /// Check the Set clear record flag
    bool is_clear_record() const;

    // /// Set the preload flag
    // void set_preload_metadata(bool);

    /// Return a const reference to the existing embedded metadata store
    const datatools::multi_properties & get_metadata_store() const;

    /// Clear the embedded metadata store
    void clear_metadata_store();

    /// Check is metadata has been possibly updated after last data record processing
    bool metadata_was_updated() const;

    /// Check source
    bool has_source() const;

    /// Get source
    const i_data_source & get_source() const;

  protected:

    /// Load a data record
    process_status _load(datatools::things & a_data_record);

    /// Open the data record source
    process_status _open_source();

    /// Check the limits on input file(s)
    void _check_limits();

    /// Set default values before explicit settings and initialization
    void _set_defaults();

    /// Return a reference to the mutable internal I/O data structure
    io_common & _grab_common();

  private:

    /// Process metadata, if any
    void _load_metadata_();

  private:

    bool                         _clear_record_; //!< A flag to automatically clear the data record before processing
    boost::scoped_ptr<io_common> _common_; //!< Common data structure
    i_data_source              * _source_ = nullptr; //!< Abstract data reader
    bool                         _metadata_updated_ = false; //!< Flag for possible metadata update

    // Macro to automate the registration of the module :
    DPP_MODULE_REGISTRATION_INTERFACE(input_module)

  };

} // namespace dpp

#endif // DPP_INPUT_MODULE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
